#include <sstream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <stdlib.h>

#include "TSystem.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TList.h"
#include "TObject.h"
#include "TString.h"
#include "TMath.h"
#include "RooHistError.h"

#include "interface/MT2Config.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"
#include "interface/MT2EstimateSyst.h"
#include "interface/MT2EstimateSigSyst.h"
#include "interface/MT2EstimateAllSigSyst.h"
#include "interface/MT2EstimateSigContSyst.h"

using namespace std;

// All options below should not be changed
bool use_hybrid = true;
bool doSignalContamination = false; // MG now set to false, because not supported in llepControlRegion
bool doSimultaneousFit = false;
bool doGenAverage = false; // MG also off,  it's what this syst takes into account in absence of signal contamination
bool addSigLepSF= true;
bool doQCDEstimate = true; // add QCD background to datacards and tables, default is true

// Edit these options
bool doBlind = false; // if true will write sum of prediction instead of observed number of events in data
bool doOnlySig = false; // set to true for signal scans on the batch, default false
bool do2016Sig = false;
bool includeSignalUnc = true; // signal lep eff commented out till available, currently set to false because requires too much memory


int Round(float d) {
  return (int)(floor(d + 0.5));
}

std::string getSimpleSignalName( const std::string& longName );
//std::string gammaConvention( float yieldSR, int yieldCR, int position, const std::string& corrName, const std::string& uncorrName="", float testAlpha=1. );
std::string gammaConventionCombined(float yieldSR16, float yieldSR17, float yieldSR18, int yieldCR, int position, const std::string& corrName, const std::string& uncorrName="", float testAlpha16=1., float testAlpha17=1., float testAlpha18=1. );


int main( int argc, char* argv[] ) {


  std::cout << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|         Running createDatacards_combined           |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc != 5 && argc != 9 && argc != 10 && argc != 11) {
    std::cout << "USAGE: ./createDatacards_combined [configFileName16] [configFileName17] [configFileName18] [model] [m1] [m2] [m11] [m22] ([label]) ([cardsDir])" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

  std::string configFileName16(argv[1]);
  std::string configFileName17(argv[2]);
  std::string configFileName18(argv[3]);

  MT2Config cfg16(configFileName16);
  MT2Config cfg17(configFileName17);
  MT2Config cfg18(configFileName18);

  float m1=0.;
  float m2=2000.;

  float m11=0.;
  float m22=2000;

  std::string model(argv[4]);

  if( argc == 9 || argc == 10 || argc == 11){

    m1  = std::stof( argv[5] );
    m2  = std::stof( argv[6] );

    m11  = std::stof( argv[7] );
    m22  = std::stof( argv[8] );
  }

  std::string label;
  std::string path_sig;
  if (argc==10){
    label = argv[9];
    path_sig = "";
  } else if (argc==11){
    label = argv[9];
    path_sig = argv[10];
  } else {
    label = "";
    path_sig = "";
  }

  std::cout << "Will produce datacards for parent mass between " << m1 << " and " << m2 << ", and LSP mass between " << m11 <<  " and " << m22 << std::endl;

  std::string dir16 = cfg16.getEventYieldDir();
  std::string dir17 = cfg17.getEventYieldDir();
  std::string dir18 = cfg18.getEventYieldDir();

  std::string mc_fileName = dir16 + "/analyses.root";
  //float lumiComb = cfg16.lumi() + cfg17.lumi() + cfg18.lumi(); // not needed ATM
  //std::cout << "Will normalize signals by combined luminosity " << lumiComb << std::endl; // not needed ATM

  //  float err_qcd_uncorr  = 1.0; // 100% of QCD MC yield, if use MC for QCD

  float err_llep_shape = 0.40;
  //OLD  float err_llep_lepEff = 0.12; // Uncertainty on llep estimate from lepton efficiency (12%)
  //float err_llep_lepEff = 0.05; // Uncertainty on llep estimate from lepton efficiency (12%)
  float err_llep_mtcut  = 0.03;
  float err_llep_tauEff  = 0.01;
  float err_llep_btagEff_heavy_7j1b = 0.01;
  float err_llep_btagEff_heavy_7j2b = 0.02;
  float err_llep_btagEff_heavy_7j3b = 0.04;
  float err_llep_btagEff_light_7j3b = 0.04;
  float err_llep_renorm = 0.01;//gio said to use 1 % because we don't want it moved, want bin by byin ideally

  float err_lep_eff = 0.05;//

  float err_zinv_shape = 0.40;
  //float err_zinv_uncorr_2b = 1.0;

  float err_lumi_corr   = 0.026; // Uncertainty on luminosity (2.6% for 2016 pp run)
  float err_sig_PU   = 0.046;

  float err_jec_llep = 0.02;
  float err_jec_zinv= 0.02; //special case for VL in line

  // First create template datacards
  std::string path_templ = dir16 + "/datacard_templates_combined"; // writing in a different directory than before
  if(do2016Sig) path_templ= dir16+ "/datacard_templates";
  system(Form("mkdir -p %s", path_templ.c_str()));


  if (!doOnlySig){ // MG


  // Reading data analysis (in search region)
  MT2Analysis<MT2Estimate>* data16  = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/analyses.root", "data" );
  MT2Analysis<MT2Estimate>* data17  = MT2Analysis<MT2Estimate>::readFromFile( dir17 + "/analyses.root", "data" );
  MT2Analysis<MT2Estimate>* data18  = MT2Analysis<MT2Estimate>::readFromFile( dir18 + "/analyses.root", "data" );
  MT2Analysis<MT2Estimate>* data = new MT2Analysis<MT2Estimate>( *(data16) );
  (*data) += (*(data17));
  (*data) += (*(data18));

  // Reading invisible Z estimate // FIXME: for the moment reading three times the same thing
  MT2Analysis<MT2Estimate>* zinv_zll16 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "ZinvEstimateFromZll_hybrid1");
  MT2Analysis<MT2Estimate>* zinv_zll17 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "ZinvEstimateFromZll_hybrid2");
  MT2Analysis<MT2Estimate>* zinv_zll18 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "ZinvEstimateFromZll_hybrid3");
  MT2Analysis<MT2Estimate>* zinv_zll = new MT2Analysis<MT2Estimate>( *(zinv_zll16) ); // also build the sum of the three
  (*zinv_zll) += (*(zinv_zll17));
  (*zinv_zll) += (*(zinv_zll18));

  MT2Analysis<MT2Estimate>* zinv_zll_alpha16 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "alpha1");
  MT2Analysis<MT2Estimate>* zinv_zll_alpha17 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "alpha2");
  MT2Analysis<MT2Estimate>* zinv_zll_alpha18 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "alpha3");
  MT2Analysis<MT2Estimate>* zinv_zll_alpha = new MT2Analysis<MT2Estimate>( *(zinv_zll_alpha16) );    // FIXME: add in computeZinvFromZll_combined
  (*zinv_zll_alpha) += (*(zinv_zll_alpha17));
  (*zinv_zll_alpha) += (*(zinv_zll_alpha18));

  MT2Analysis<MT2Estimate>* zinvCR_zll = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "dataCR");
  MT2Analysis<MT2Estimate>* purity_zll = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "purity_forHybrid");
  MT2Analysis<MT2Estimate>* zinv_zll_bin_extrapol = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/zinvFromZllCombined.root", "bin_extrapol");
  //  MT2Analysis<MT2Estimate>* purity_zll_err;

  //zinv->addToFile( mc_fileName, true ); // Optionally, to add estimate used for invisible Z estimate to analyses.root

  // Reading lost lepton estimate
  MT2Analysis<MT2Estimate>* llep16 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "llepEstimate16" );
  MT2Analysis<MT2Estimate>* llep17 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "llepEstimate17" );
  MT2Analysis<MT2Estimate>* llep18 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "llepEstimate18" );
  MT2Analysis<MT2Estimate>* llep = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "llepEstimate" );

  MT2Analysis<MT2Estimate>* llepCR = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "hybrid_llepCR" );
  MT2Analysis<MT2Estimate>* llep_bin_extrapol = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "extrapol_bin" );
  MT2Analysis<MT2Estimate>* llep_ratio = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "RatioMC" );
  MT2Analysis<MT2Estimate>* llep_ratio16 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "RatioMC16" );
  MT2Analysis<MT2Estimate>* llep_ratio17 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "RatioMC17" );
  MT2Analysis<MT2Estimate>* llep_ratio18 = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "RatioMC18" );

  // also reading MC cr and MC sr
  MT2Analysis<MT2Estimate>* llep_MCcr = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "MCcr");
  MT2Analysis<MT2Estimate>* llep_MCsr = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "/llepEstimateCombined.root", "MCsr");

  //llep->addToFile( mc_fileName, true ); // Optionally, to add estimate used for invisible Z estimate to analyses.root

  // Reading QCD estimate
  MT2Analysis<MT2Estimate>* qcd_nominal;
  MT2Analysis<MT2Estimate>* qcd_syst_jer;
  MT2Analysis<MT2Estimate>* qcd_syst_nbjetshape;
  MT2Analysis<MT2Estimate>* qcd_syst_njetshape;
  MT2Analysis<MT2Estimate>* qcd_syst_tail;
  MT2Analysis<MT2Estimate>* qcd_syst_sigmasoft;

  if(doQCDEstimate){
    qcd_nominal = MT2Analysis<MT2Estimate>::readFromFile( "/work/mratti/QCD/qcdEstimate_V0.root", "nominal" );
    qcd_syst_jer = MT2Analysis<MT2Estimate>::readFromFile( "/work/mratti/QCD/qcdEstimate_V0.root", "syst_jer" );
    qcd_syst_nbjetshape = MT2Analysis<MT2Estimate>::readFromFile( "/work/mratti/QCD/qcdEstimate_V0.root", "syst_nbjetshape" );
    qcd_syst_njetshape = MT2Analysis<MT2Estimate>::readFromFile( "/work/mratti/QCD/qcdEstimate_V0.root", "syst_njetshape" );
    qcd_syst_sigmasoft = MT2Analysis<MT2Estimate>::readFromFile( "/work/mratti/QCD/qcdEstimate_V0.root", "syst_sigmasoft" );
    qcd_syst_tail = MT2Analysis<MT2Estimate>::readFromFile( "/work/mratti/QCD/qcdEstimate_V0.root", "syst_tail" );
  }


  std::cout << "Read in all the inputs" << std::endl;

  // Getting region set used (from data)
  std::set<MT2Region> regions = data->getRegions();
  std::cout << "Defined Regions " << std::endl;

  // Start loop over TOPOLOGICAL regions
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    ///////////////////////////////////////////////////////
    // Get histograms from all input estimates
    ///////////////////////////////////////////////////////
//    std::cout << "debug region before" << iR->getName() << std::endl;
//    if(iR->getName()!="HT1500toInf_j7to9_b4toInf" && iR->getName()!="HT1500toInf_j7to9_b3") continue;
    //std::cout << "debug region " << iR->getName() << std::endl;
    // Getting data yield histogram
    //TH1D* this_data16 = data16->get(*iR)->yield;
    //TH1D* this_data17 = data17->get(*iR)->yield;
    //TH1D* this_data18 = data18->get(*iR)->yield;
    TH1D* this_data = data->get(*iR)->yield;
    int nBins = this_data->GetNbinsX(); // Getting total number of bins for this topological region

    // Get histograms for invisible Z estimate
    TH1D* this_zinv_zll16 = zinv_zll16->get(*iR)->yield;
    TH1D* this_zinv_zll17 = zinv_zll17->get(*iR)->yield;
    TH1D* this_zinv_zll18 = zinv_zll18->get(*iR)->yield;
    TH1D* this_zinv_zll = zinv_zll->get(*iR)->yield;
    TH1D* this_zinv_zll_alpha =  zinv_zll_alpha->get(*iR)->yield;
    TH1D* this_zinv_zll_alpha16 =  zinv_zll_alpha16->get(*iR)->yield;
    TH1D* this_zinv_zll_alpha17 =  zinv_zll_alpha17->get(*iR)->yield;
    TH1D* this_zinv_zll_alpha18 =  zinv_zll_alpha18->get(*iR)->yield;
    TH1D* this_zinvCR_zll  = zinvCR_zll->get(*iR)->yield;
    TH1D* this_zinv_purity_zll = purity_zll->get(*iR)->yield;
    TH1D* this_zinv_zll_bin_extrapol = zinv_zll_bin_extrapol->get(*iR)->yield;
    int zinv_zll_hybridBin = this_zinv_zll_bin_extrapol->GetBinContent(1);

    int Nzll=0; // Initialize variable for number of events in zll CR

    // Get histograms for lost lepton estimate
    TH1D* this_llep16 = llep16->get(*iR)->yield;
    TH1D* this_llep17 = llep17->get(*iR)->yield;
    TH1D* this_llep18 = llep18->get(*iR)->yield;
    TH1D* this_llep = llep->get(*iR)->yield;
    TH1D* this_llep_ratio = llep_ratio->get(*iR)->yield;
    TH1D* this_llep_ratio16 = llep_ratio16->get(*iR)->yield;
    TH1D* this_llep_ratio17 = llep_ratio17->get(*iR)->yield;
    TH1D* this_llep_ratio18 = llep_ratio18->get(*iR)->yield;
    TH1D* this_llepCR = llepCR->get(*iR)->yield;
    TH1D* this_llep_bin_extrapol = llep_bin_extrapol->get(*iR)->yield;
    TH1D* this_llep_MCcr = llep_MCcr->get(*iR)->yield;
    TH1D* this_llep_MCsr = llep_MCsr->get(*iR)->yield;

    int llep_hybridBin = this_llep_bin_extrapol->GetBinContent(1);

    float N_llep_CR = this_llepCR->Integral();

    // Regions with N(J)>=7 and N(B)>=1 share same control region (1-2 b jets)
    std::string llepCR_name;
    MT2Region* thisCR;
    if(iR->nJetsMin()>=7 && iR->nBJetsMin()>=1){
      thisCR = new MT2Region(iR->htMin(), iR->htMax(), iR->nJetsMin(), iR->nJetsMax(), 1, 2);
      llepCR_name = thisCR->getName();
    } else if (iR->nJetsMin()>=7 && iR->nBJetsMin()==0){
      thisCR = new MT2Region(iR->htMin(), iR->htMax(), iR->nJetsMin(), iR->nJetsMax(), 0, 0);
      llepCR_name = thisCR->getName();
    } else
      llepCR_name = iR->getName();

    //std::cout << "DEBUG: signal region name=" << iR->getName() << "  control region name="<< llepCR_name << std::endl;

    // Getting QCD yield histograms, plus uncertainties from QCD estimate
    TH1D* this_qcd;
    TH1D* this_qcd_syst_jer;
    TH1D* this_qcd_syst_nbjetshape;
    TH1D* this_qcd_syst_njetshape;
    TH1D* this_qcd_syst_sigmasoft;
    TH1D* this_qcd_syst_tail;

    if(doQCDEstimate){
        this_qcd = qcd_nominal->get(*iR)->yield;
        this_qcd_syst_jer = qcd_syst_jer->get(*iR)->yield;
        this_qcd_syst_nbjetshape = qcd_syst_nbjetshape->get(*iR)->yield;
        this_qcd_syst_njetshape = qcd_syst_njetshape->get(*iR)->yield;
        this_qcd_syst_sigmasoft = qcd_syst_sigmasoft->get(*iR)->yield;
        this_qcd_syst_tail = qcd_syst_tail->get(*iR)->yield;
    }

    // some specific systematics depending on ht
    if( iR->htMin()==250 && iR->htMax()==450 )
      err_jec_zinv = 0.05;
    else
      err_jec_zinv = 0.02;

    ///////////////////////////////////////////////////////
    // Calculation of uncertainty on hybrid shape
    ///////////////////////////////////////////////////////

    // Calculating shape uncertainty for invisible Z (default: linear extrapolation)
    // for the moment will assume same uncertainty for all three years (fully correlated)
    float shapeErr_zinv=0.;
    for( int iBin=1; iBin<this_data->GetNbinsX()+1; ++iBin ) {
      float relativeErr = 0.0;
      int extrapol_bin = ( use_hybrid ) ?  zinv_zll_hybridBin : 1;

      if( extrapol_bin >= nBins ) continue;

      if( fabs(this_zinv_zll->GetBinContent(iBin))>0 ) // use the total count here
        if(  iBin >=  extrapol_bin ) //nec for hybrid
          relativeErr = err_zinv_shape / (nBins - extrapol_bin) * (iBin - extrapol_bin);

      shapeErr_zinv+=relativeErr*fabs(this_zinv_zll->GetBinContent(iBin));
      //std::cout << shapeErr_zinv << std::endl;
    }
    int extrapol_bin = ( use_hybrid ) ?  zinv_zll_hybridBin : 1;
    int flag_fixshapeerr_zinv=0;
    if(shapeErr_zinv>fabs(this_zinv_zll->GetBinContent(extrapol_bin))){
      if(shapeErr_zinv<=fabs(this_zinv_zll->GetBinContent(extrapol_bin))+fabs(this_zinv_zll->GetBinContent(extrapol_bin+1)))
        flag_fixshapeerr_zinv=1;
      else if(shapeErr_zinv<=fabs(this_zinv_zll->GetBinContent(extrapol_bin))+fabs(this_zinv_zll->GetBinContent(extrapol_bin+1))+fabs(this_zinv_zll->GetBinContent(extrapol_bin+2)))flag_fixshapeerr_zinv=2;
      else if(shapeErr_zinv<=fabs(this_zinv_zll->GetBinContent(extrapol_bin))+fabs(this_zinv_zll->GetBinContent(extrapol_bin+1))+fabs(this_zinv_zll->GetBinContent(extrapol_bin+2))+fabs(this_zinv_zll->GetBinContent(extrapol_bin+3)))flag_fixshapeerr_zinv=3;
      else flag_fixshapeerr_zinv=4;
    }
     

    // Calculating shape uncertainty for lost lepton (default: linear extrapolation)
    // for the moment will assume same uncertainty for all three years (fully correlated)
    float shapeErr_llep=0.;
    for( int iBin=1; iBin<this_data->GetNbinsX()+1; ++iBin ) {
      float relativeErr = 0.0;
      int extrapol_bin = ( use_hybrid ) ?  llep_hybridBin : 1;
      if( extrapol_bin >= nBins ) continue;

      if( this_llep->GetBinContent(iBin)>0 )
        if(  iBin >=  extrapol_bin  )
          relativeErr = err_llep_shape / (nBins-extrapol_bin) * (iBin-extrapol_bin);

      shapeErr_llep+=relativeErr*this_llep->GetBinContent(iBin);
    }

    extrapol_bin = ( use_hybrid ) ?  llep_hybridBin : 1;
    int flag_fixshapeerr_llep=0;
    if(shapeErr_llep>fabs(this_llep->GetBinContent(extrapol_bin))){
      if(shapeErr_llep<=fabs(this_llep->GetBinContent(extrapol_bin))+fabs(this_llep->GetBinContent(extrapol_bin+1)))
        flag_fixshapeerr_llep=1;
      else if (shapeErr_llep<=fabs(this_llep->GetBinContent(extrapol_bin))+fabs(this_llep->GetBinContent(extrapol_bin+1))+fabs(this_llep->GetBinContent(extrapol_bin+2)))flag_fixshapeerr_llep=2;
      else if (shapeErr_llep<=fabs(this_llep->GetBinContent(extrapol_bin))+fabs(this_llep->GetBinContent(extrapol_bin+1))+fabs(this_llep->GetBinContent(extrapol_bin+2))+fabs(this_llep->GetBinContent(extrapol_bin+3)))flag_fixshapeerr_llep=3;
      else flag_fixshapeerr_llep=4;
    }

    float lastR_zinvZll; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
    float lastR_zinvZll16; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
    float lastR_zinvZll17; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
    float lastR_zinvZll18; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
    float lastR_llep; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
    float lastR_llep16; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
    float lastR_llep17; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
    float lastR_llep18; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.

    ///////////////////////////////////////////////////////
    // Start loop over MT2 bins
    ///////////////////////////////////////////////////////
    for( int iBin=1; iBin<this_data->GetNbinsX()+1; ++iBin ) {

      //bool includeCR=false; // In case do simultaneous fit of control region (for signal contamination), need to include CR into datacard
      //if(iBin==1) includeCR=true;
      //if(iR->nJetsMin()>=7 && iR->nBJetsMin()>1) includeCR=false;

      // If MT2 bin low edge > max HT continue (should never happen by construction)
      if(this_data->GetBinLowEdge( iBin ) > iR->htMax() && iR->htMax()>0 ) continue;

      // Getting MT2 bin edges
      float mt2Min = this_data->GetBinLowEdge( iBin );
      float mt2Max = (iBin==this_data->GetNbinsX()) ?  -1. : this_data->GetBinLowEdge( iBin+1 );

      // Getting bin name
      std::string binName;

      if( mt2Max>=0. ) binName = std::string( Form("%s_m%.0fto%.0f", iR->getName().c_str(), mt2Min, mt2Max) );
      else binName = std::string( Form("%s_m%.0ftoInf", iR->getName().c_str(), mt2Min) );

      // Getting bin name for special cases of >=7 jets
      std::string binName_7j;
      if(iR->nJetsMin()>=7 && iR->nBJetsMin()>=1){
        MT2Region* thisCR = new MT2Region(iR->htMin(), iR->htMax(), iR->nJetsMin(), iR->nJetsMax(), 1, 2);
        if( mt2Max>=0. ) binName_7j = std::string( Form("%s_m%.0fto%.0f", thisCR->getName().c_str(), mt2Min, mt2Max) );
        else binName_7j = std::string( Form("%s_m%.0ftoInf", thisCR->getName().c_str(), mt2Min) );
      }else if (iR->nJetsMin()>=7 && iR->nBJetsMin()==0){
        MT2Region* thisCR = new MT2Region(iR->htMin(), iR->htMax(), iR->nJetsMin(), iR->nJetsMax(), 0, 0);
        if( mt2Max>=0. ) binName_7j = std::string( Form("%s_m%.0fto%.0f", thisCR->getName().c_str(), mt2Min, mt2Max) );
        else binName_7j = std::string( Form("%s_m%.0ftoInf", thisCR->getName().c_str(), mt2Min) );
      }else {
         binName_7j = binName;
      }
      //std::cout << "debug binName=" << binName << "  binName7j=" << binName_7j << std::endl;

      // Getting HT region name
      std::string htName;
      htName = iR->htRegion()->getName();

      // Getting NJ name
      int nJetsMin = iR->nJetsMin();
      int nJetsMax = iR->nJetsMax();
      std::string jName;
      jName = iR->sigRegion()->getSingleJetString( "j", nJetsMin,  nJetsMax  );

      // Getting NB name
      int nBJetsMin = iR->nBJetsMin();
      int nBJetsMax = iR->nBJetsMax();
      std::string bName;
      bName = iR->sigRegion()->getSingleJetString( "b", nBJetsMin,  nBJetsMax  );

      // Set template datacard name for this bin
      std::string datacardName( Form("%s/datacard_%s.txt", path_templ.c_str(), binName.c_str()) );

      std::ifstream thisDatacard( datacardName.c_str() );

      //if( thisDatacard.good() ) continue; // If template already exists, move on

      if(iR->htMin()==1500 && iR->nJetsMin()>1 && mt2Min==200 ) continue; //don't even write the first bin for extreme HT

      std::ofstream datacard( datacardName.c_str() );

      // Initializing table for estimates + observation
      std::string tableName( Form("%s/table_%s.txt", path_templ.c_str(), binName.c_str()) );
      std::ofstream table( tableName.c_str() );
      table << std::setprecision(3);

      // Set name for 1L CR (needed for simultaneous fit)
      //std::string binName1L( Form("%s_1L", binName.c_str()) );

      // Start writing template card
      // Read background estimates values
      float yield_zinv_zll16 = fabs(this_zinv_zll16->GetBinContent(iBin));
      float yield_zinv_zll17 = fabs(this_zinv_zll17->GetBinContent(iBin));
      float yield_zinv_zll18 = fabs(this_zinv_zll18->GetBinContent(iBin));
      float yield_zinv_zll = yield_zinv_zll16 + yield_zinv_zll17 + yield_zinv_zll18;

      float yield_llep16 = fabs(this_llep16->GetBinContent(iBin));
      float yield_llep17 = fabs(this_llep17->GetBinContent(iBin));
      float yield_llep18 = fabs(this_llep18->GetBinContent(iBin));
      float yield_llep = yield_llep16 + yield_llep17 + yield_llep18;

      float yield_qcd(0.);
      float yield_qcd_syst_jer(0.);
      float yield_qcd_syst_nbjetshape(0.);
      float yield_qcd_syst_njetshape(0.);
      float yield_qcd_syst_sigmasoft(0.);
      float yield_qcd_syst_tail(0.);

      // Read observed data value
      int nData;
      if (!doBlind) nData = Round(this_data->GetBinContent(iBin));
      else nData = Round(yield_zinv_zll+yield_llep+yield_qcd);


      if(doQCDEstimate){
        yield_qcd = this_qcd ->GetBinContent(iBin);
        yield_qcd_syst_jer = this_qcd_syst_jer->GetBinContent(iBin);
        yield_qcd_syst_nbjetshape = this_qcd_syst_nbjetshape->GetBinContent(iBin);
        yield_qcd_syst_njetshape = this_qcd_syst_njetshape->GetBinContent(iBin);
        yield_qcd_syst_tail = this_qcd_syst_tail->GetBinContent(iBin);
        yield_qcd_syst_sigmasoft = this_qcd_syst_sigmasoft->GetBinContent(iBin);
      }

      //float N_obs = yield_llep + yield_qcd + yield_zinv_zll;

      datacard << "imax 1" << std::endl; //Number of bins
      datacard << "jmax 7" << std::endl; //Number of backgrounds 3 years  x 2 bkgs + 1 qcd
      datacard << "kmax *" << std::endl; //Number of nuisances
      datacard << "-------------" << std::endl;
      datacard << std::endl << std::endl;

      datacard << std::fixed;
      datacard << std::setprecision(3) << std::endl << std::endl;
      datacard << "bin  " << binName << std::endl;
      datacard << "observation  " << nData << std::endl;
      datacard << "-------------" << std::endl;
      datacard << std::endl << std::endl;

      // sig qcd zinv llep
      datacard << "bin \t" << binName << "\t" << binName << "\t" << binName << "\t" << binName << "\t" << binName << "\t" << binName << "\t" << binName << "\t" << binName << std::endl;
      datacard << "process \t sig \t zinv16 \t zinv17 \t zinv18 \t llep16 \t llep17 \t llep18 \t qcd" << std::endl;
      datacard << "process \t 0 \t 1 \t 2 \t 3 \t 4 \t 5 \t 6 \t 7" << std::endl;
      datacard << "rate \t XXX";
      //      datacard << "rate \t 0.01";
      datacard << " \t " << yield_zinv_zll16 << " \t " << yield_zinv_zll17 << " \t " << yield_zinv_zll18 << " \t " << yield_llep16 << " \t " << yield_llep17 << " \t " <<yield_llep18 << " \t " << yield_qcd << std::endl;
      datacard << "-------------" << std::endl;

      datacard << "lumi_syst    lnN    " << 1.+err_lumi_corr << " - - - - - - -" << std::endl;
      datacard << "PU_syst    lnN    " << 1.+err_sig_PU << " - - - - - - -" << std::endl;

      datacard << "lep_eff    lnN    - " << 1. + err_lep_eff << " " << 1. + err_lep_eff << " " << 1. + err_lep_eff << " " << 1. + err_lep_eff << " " << 1. + err_lep_eff << " " << 1. + err_lep_eff << "  - "  << std::endl;
      datacard << "jec    lnN    - "    << 1.+ err_jec_zinv  << " " << 1.+ err_jec_llep << " " << 1.+ err_jec_llep << " " << 1.+ err_jec_llep << " " << 1.+ err_jec_llep << " " << 1.+ err_jec_llep << "  - "  << std::endl;
      datacard << "renorm    lnN    - " << 1. + err_llep_renorm << " " << 1. + err_llep_renorm << " " << 1. + err_llep_renorm << " " << 1. + err_llep_renorm << " " << 1. + err_llep_renorm << " " << 1. + err_llep_renorm << "  - "  << std::endl;

      if(!includeSignalUnc)
        datacard << "sig_syst_" << binName << " lnN 1.2 - - - - - - -" << std::endl;
      else{
        datacard << "sig_MCstat_" << binName << " lnN UUU - - - - - - -" << std::endl;
        if (doGenAverage)
          datacard << "sig_gensyst lnU SSS - - - - - - -" << std::endl;

        datacard << "sig_isrSyst lnN III - - - - - - -" << std::endl;
        datacard << "sig_bTagHeavySyst lnN HHH - - - - - - -" << std::endl;
        datacard << "sig_bTagLightSyst lnN LLL - - - - - - -" << std::endl;
        if( addSigLepSF && ((model=="T2tt" || model=="T1tttt" || model == "T2bt" || model == "T2bW")) )
          datacard << "sig_lepEffSyst lnN EEE - - - - - - -" << std::endl; // Include lepton eff. uncertainty only for T2tt and T1tttt
      }

      // Initialize variables for tables
      float zinvZll_statUp = 0.;
      float zinvZll_statDn = 0.;
      float zinvZll_systUp = 0.;
      float zinvZll_systDn = 0.;
      int zinv_zll_nCR = 0;

      float llep_statUp = 0.;
      float llep_statDn = 0.;
      float llep_systUp = 0.;
      float llep_systDn = 0.;
      int llep_nCR = 0;

      float qcd_statUp = 0.; // for the moment zero
      float qcd_statDn = 0.; // for the moment zero
      float qcd_systUp = 0.; // total syst
      float qcd_systDn = 0.; // total syst
      int qcd_nCR  = 0; // should not be used in any case

      float summedErr = err_jec_zinv* err_jec_zinv +  err_jec_llep*err_jec_llep + err_llep_renorm*err_llep_renorm;
      zinvZll_systUp += summedErr;
      zinvZll_systDn += summedErr;
      llep_systUp    += summedErr;
      llep_systDn    += summedErr;

      std::string zinvCR_name;
      zinvCR_name = iR->getName(); // not taking >=7bjets merging into account ??

      //////////////////////////////////////
      // Z INVISIBLE SYSTEMATICS
      //////////////////////////////////////
      if( yield_zinv_zll>=0. ) { // par0

        Nzll = Round(this_zinvCR_zll->Integral());
        float p_zll = this_zinv_purity_zll->GetBinContent( iBin );
        float err_zinvZll_purity = (this_zinv_purity_zll->GetBinError(iBin));

        datacard << "zinvDY_Rsfof  lnN  - " << 1.+ 0.15*(1.-p_zll) << " " << 1.+ 0.15*(1.-p_zll) << " " <<  1.+ 0.15*(1.-p_zll) << " - - - -" << std::endl; // FIXME:
        zinvZll_systUp += (0.15*(1.-p_zll))*(0.15*(1.-p_zll));
        zinvZll_systDn += (0.15*(1.-p_zll))*(0.15*(1.-p_zll));

        //Syst uncerainty on purity
        if( Nzll>0 ){ // par4
          if( p_zll > 0. ){
            float p_zllUp = err_zinvZll_purity / p_zll ;
            float p_zllDn = err_zinvZll_purity / p_zll ;
            if( ( p_zllUp )>=1. ){
              p_zllUp = 1.0;
              p_zllDn = 1.0;
            }
            //std::cout << p_zll << "  " << p_zllUp << "  " << p_zllDn << std::endl;
            datacard << "zinvDY_purity_" << zinvCR_name << " lnN  - " << 1.+ p_zllUp << " " <<  1.+ p_zllUp << " " << 1.+ p_zllUp << " - - - -" << std::endl; // FIXME
            zinvZll_systUp += p_zllUp*p_zllUp;
            zinvZll_systDn += p_zllDn*p_zllDn;

          }else{ //purity = 0 //this happens at 36ifb still for a few regions
            datacard << "zinvDY_purity_" << zinvCR_name << " lnN  - 1.0/0.01 - - - -" << std::endl; // FIXME !
            zinvZll_systUp += 1.0;
            zinvZll_systDn += 1.0;
          }
        }//End of zll purity stuff // end par4

        // Get Z/Zll ratio
        float R_zll = fabs(this_zinv_zll_alpha->GetBinContent(iBin));
        float R_zll16 = fabs(this_zinv_zll_alpha16->GetBinContent(iBin));
        float R_zll17 = fabs(this_zinv_zll_alpha17->GetBinContent(iBin));
        float R_zll18 = fabs(this_zinv_zll_alpha18->GetBinContent(iBin));

        float relativeErr_zll = fabs(this_zinv_zll_alpha->GetBinError(iBin));

        // Keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
        if (R_zll>0)   lastR_zinvZll = R_zll;
        if (R_zll16>0) lastR_zinvZll16 = R_zll16;
        if (R_zll17>0) lastR_zinvZll17 = R_zll17;
        if (R_zll18>0) lastR_zinvZll18 = R_zll18;

        int extrapol_bin = ( use_hybrid ) ?  zinv_zll_hybridBin : 1;
        if( fabs(this_zinv_zll->GetBinContent(iBin))>0 && iBin >= zinv_zll_hybridBin )
          relativeErr_zll = err_zinv_shape / (nBins-extrapol_bin) * (iBin-extrapol_bin);
        else
          relativeErr_zll = 0.0;

        float testAlphaZll = (R_zll>0) ? R_zll : lastR_zinvZll;
        float testAlphaZll16 = (R_zll16>0) ? R_zll : lastR_zinvZll16;
        float testAlphaZll17 = (R_zll17>0) ? R_zll : lastR_zinvZll17;
        float testAlphaZll18 = (R_zll18>0) ? R_zll : lastR_zinvZll18;
        //datacard << "zinvDY_CRstat_" << std::setprecision(3) << gammaConvention( yield_zinv_zll, Nzll, 1, zinvCR_name, binName, (R_zll>0) ? R_zll : lastR_zinvZll ) << std::setprecision(3) << std::endl;
        datacard << "zinvDY_CRstat_" << std::setprecision(3) << gammaConventionCombined( yield_zinv_zll16, yield_zinv_zll17, yield_zinv_zll18, Nzll, 1, zinvCR_name, binName, testAlphaZll16, testAlphaZll17, testAlphaZll18 ) << std::setprecision(3) << std::endl;

        if( nBins>1 ){
          if( iBin==extrapol_bin && fabs(yield_zinv_zll)>0 ){
            if(!flag_fixshapeerr_zinv){
              datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.-shapeErr_zinv/fabs(yield_zinv_zll) << " " << 1.-shapeErr_zinv/fabs(yield_zinv_zll) << " " << 1.-shapeErr_zinv/fabs(yield_zinv_zll) << " - - - - " << std::endl;
              zinvZll_systUp += (shapeErr_zinv/fabs(yield_zinv_zll))*(shapeErr_zinv/fabs(yield_zinv_zll));
              zinvZll_systDn += (shapeErr_zinv/fabs(yield_zinv_zll))*(shapeErr_zinv/fabs(yield_zinv_zll));
            }
            else{
              cout<<"zinv: region "<<binName<<", shapeErr_zinv "<<shapeErr_zinv<<", yield_zinv_zll "<<yield_zinv_zll<<", need to correct this case in extrapol bin and next"<<endl;
              zinvZll_systUp += 1.0;
              zinvZll_systDn += 1.0;
            }


          }else{
            if( iBin > extrapol_bin ){ //for hybrid (made such that is works also for pure extrapolation

              if(iBin>extrapol_bin+4||!flag_fixshapeerr_zinv){
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll << " " << 1.+relativeErr_zll << " "  << 1.+relativeErr_zll  << " - - - - " << std::endl;
                zinvZll_systUp += relativeErr_zll*relativeErr_zll;
                zinvZll_systDn += relativeErr_zll*relativeErr_zll;
              }
              else if( iBin==extrapol_bin+1 &&flag_fixshapeerr_zinv==1){

                float compen_res=shapeErr_zinv-fabs(this_zinv_zll->GetBinContent(extrapol_bin));
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll-compen_res/yield_zinv_zll << " " << 1.+relativeErr_zll-compen_res/yield_zinv_zll << " "  << 1.+relativeErr_zll-compen_res/yield_zinv_zll  << " - - - - " << std::endl;
                zinvZll_systUp += (relativeErr_zll-compen_res/yield_zinv_zll)*(relativeErr_zll-compen_res/yield_zinv_zll);
                zinvZll_systDn += (relativeErr_zll-compen_res/yield_zinv_zll)*(relativeErr_zll-compen_res/yield_zinv_zll);
                cout<<"region "<<binName<<", zinv corrected relative error "<<relativeErr_zll-compen_res/yield_zinv_zll<<endl;
                cout<<"zinv: corrected the bin next to extrapolbin"<<endl;

              }
              else if ( iBin==extrapol_bin+1 && flag_fixshapeerr_zinv>=2){
                zinvZll_systUp += 1.0;
                zinvZll_systDn += 1.0;
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << relativeErr_zll << " " << relativeErr_zll << " "  << relativeErr_zll  << " - - - - " << std::endl;
                cout<<"zinv: still need to correct in the second bin next to extropolbin"<<endl;
              }
              else if (iBin==extrapol_bin+2 && flag_fixshapeerr_zinv<2 ){
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll << " " << 1.+relativeErr_zll << " "  << 1.+relativeErr_zll  << " - - - - " << std::endl;
                zinvZll_systUp += relativeErr_zll*relativeErr_zll;
                zinvZll_systDn += relativeErr_zll*relativeErr_zll;
              }
              else if (iBin==extrapol_bin+2 && flag_fixshapeerr_zinv==2){
                float compen_res=shapeErr_zinv-fabs(this_zinv_zll->GetBinContent(extrapol_bin))-fabs(this_zinv_zll->GetBinContent(extrapol_bin+1));
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll-compen_res/yield_zinv_zll << " " << 1.+relativeErr_zll-compen_res/yield_zinv_zll << " "  << 1.+relativeErr_zll-compen_res/yield_zinv_zll  << " - - - - " << std::endl;
               zinvZll_systUp += (relativeErr_zll-compen_res/yield_zinv_zll)*(relativeErr_zll-compen_res/yield_zinv_zll);
               zinvZll_systDn += (relativeErr_zll-compen_res/yield_zinv_zll)*(relativeErr_zll-compen_res/yield_zinv_zll);
               cout<<"region "<<binName<<", zinv corrected relative error "<<(relativeErr_zll-compen_res/yield_zinv_zll)<<endl; 
               cout<<"zinv: corrected the second bin next to extrapolbin"<<endl;
              }
              else if ( iBin==extrapol_bin+2 && flag_fixshapeerr_zinv>2){
                zinvZll_systUp += 1.0;
                zinvZll_systDn += 1.0;
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << relativeErr_zll << " " << relativeErr_zll << " "  << relativeErr_zll  << " - - - - " << std::endl;
                cout<<"zinv: still need to correct in the third bin next to extropolbin"<<endl;
              }
              else if(iBin==extrapol_bin+3 && flag_fixshapeerr_zinv<3){
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll << " " << 1.+relativeErr_zll << " "  << 1.+relativeErr_zll  << " - - - - " << std::endl;
                zinvZll_systUp += relativeErr_zll*relativeErr_zll;
                zinvZll_systDn += relativeErr_zll*relativeErr_zll;
              }
              else if(iBin==extrapol_bin+3 && flag_fixshapeerr_zinv==3){
                float compen_res=shapeErr_zinv-fabs(this_zinv_zll->GetBinContent(extrapol_bin))-fabs(this_zinv_zll->GetBinContent(extrapol_bin+1))-fabs(this_zinv_zll->GetBinContent(extrapol_bin+2));
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll-compen_res/yield_zinv_zll << " " << 1.+relativeErr_zll-compen_res/yield_zinv_zll << " "  << 1.+relativeErr_zll-compen_res/yield_zinv_zll  << " - - - - " << std::endl;
               zinvZll_systUp += (relativeErr_zll-compen_res/yield_zinv_zll)*(relativeErr_zll-compen_res/yield_zinv_zll);
               zinvZll_systDn += (relativeErr_zll-compen_res/yield_zinv_zll)*(relativeErr_zll-compen_res/yield_zinv_zll);
               cout<<"region "<<binName<<", zinv corrected relative error "<<(relativeErr_zll-compen_res/yield_zinv_zll)<<endl;
               cout<<"zinv: corrected the third bin next to extrapolbin"<<endl;
              }
              else if (iBin==extrapol_bin+3 && flag_fixshapeerr_zinv>3){

                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << relativeErr_zll << " " << relativeErr_zll << " "  << relativeErr_zll  << " - - - - " << std::endl;
                cout<<"zinv: still need to correct in the fourth bin next to extropolbin"<<endl;

              }

              else if(iBin==extrapol_bin+4 && flag_fixshapeerr_zinv<4){
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll << " " << 1.+relativeErr_zll << " "  << 1.+relativeErr_zll  << " - - - - " << std::endl;
                zinvZll_systUp += relativeErr_zll*relativeErr_zll;
                zinvZll_systDn += relativeErr_zll*relativeErr_zll;
              }

              else if(iBin==extrapol_bin+4 && flag_fixshapeerr_zinv==4){
                float compen_res=shapeErr_zinv-fabs(this_zinv_zll->GetBinContent(extrapol_bin))-fabs(this_zinv_zll->GetBinContent(extrapol_bin+1))-fabs(this_zinv_zll->GetBinContent(extrapol_bin+2))-fabs(this_zinv_zll->GetBinContent(extrapol_bin+3));
                datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll-compen_res/yield_zinv_zll << " " << 1.+relativeErr_zll-compen_res/yield_zinv_zll << " "  << 1.+relativeErr_zll-compen_res/yield_zinv_zll  << " - - - - " << std::endl;
               zinvZll_systUp += (relativeErr_zll-compen_res/yield_zinv_zll)*(relativeErr_zll-compen_res/yield_zinv_zll);
               zinvZll_systDn += (relativeErr_zll-compen_res/yield_zinv_zll)*(relativeErr_zll-compen_res/yield_zinv_zll);
               cout<<"region "<<binName<<", zinv corrected relative error "<<(relativeErr_zll-compen_res/yield_zinv_zll)<<endl;
               cout<<"zinv: corrected the fourth bin next to extrapolbin"<<endl;
               if (1.+relativeErr_zll-compen_res/yield_zinv_zll<0) cout<<"zinvDY_shape still negative:"<< 1.+relativeErr_zll-compen_res/yield_zinv_zll<<", need to correct the fifth bin next to extrapolbin, please check"<<endl;
              }
              else{cout<<"still something wrong, please check"<<endl;}
            }
          }
        }

        double yield_zinv_zll_up, yield_zinv_zll_dn;
        RooHistError::instance().getPoissonInterval(Nzll,yield_zinv_zll_dn,yield_zinv_zll_up,1.);
        yield_zinv_zll_up *= (Nzll>0) ? yield_zinv_zll/Nzll : testAlphaZll;
        yield_zinv_zll_dn *= (Nzll>0) ? yield_zinv_zll/Nzll : testAlphaZll;

        zinvZll_statUp = yield_zinv_zll_up - yield_zinv_zll;
        zinvZll_statDn = yield_zinv_zll    - yield_zinv_zll_dn;

        // Uncertainty on transfer factor - I don't know what this contains
        float alphaErr_zll;
        if ( R_zll>0 ) alphaErr_zll= this_zinv_zll_alpha->GetBinError(iBin)/R_zll;
        else alphaErr_zll=1.0;
        datacard << "zinvDY_alphaErr_" << binName << " lnN  - " << 1.+alphaErr_zll  << " " <<  1.+alphaErr_zll << " " << 1.+alphaErr_zll << " - - - -" << std::endl; // FIXME

        zinvZll_systUp += alphaErr_zll*alphaErr_zll;
        zinvZll_systDn += alphaErr_zll*alphaErr_zll;

        zinv_zll_nCR = Nzll; // Just for table

      } // if zinv end par0

      //////////////////////////////////////
      // LOST LEPTON SYSTEMATICS
      //////////////////////////////////////
      if( yield_llep>=0. ) { // yield_llep>=0.

        //The bin content of the data is already integrated in the extrapolation region
        N_llep_CR = this_llepCR->GetBinContent(iBin);
        // Get TF for lost-lepton
        float Rllep = this_llep_ratio->GetBinContent(iBin);
        if( Rllep > 0 ) lastR_llep = Rllep; // Keep track of last non-zero TF. Will use it for 'next' bin if TF will be zero.
        float Rllep16 = this_llep_ratio16->GetBinContent(iBin);
        if( Rllep16 > 0 ) lastR_llep16 = Rllep16;
        float Rllep17 = this_llep_ratio17->GetBinContent(iBin);
        if( Rllep17 > 0 ) lastR_llep17 = Rllep17;
        float Rllep18 = this_llep_ratio18->GetBinContent(iBin);
        if( Rllep18 > 0 ) lastR_llep18 = Rllep18;

        int extrapol_bin_llep = ( use_hybrid ) ?  llep_hybridBin : 1;
        float relativeErr_llep;
        if( fabs( this_llep->GetBinContent(iBin))>0 && iBin >= extrapol_bin_llep )
          relativeErr_llep = err_llep_shape / (nBins - extrapol_bin_llep) * (iBin - extrapol_bin_llep);
        else
          relativeErr_llep = 0.0;

        // mt cut syst
        datacard << "llep_mtcut lnN  - - - - " << 1.+err_llep_mtcut << " " << 1.+err_llep_mtcut << " " << 1.+err_llep_mtcut << " -" << std::endl;
        llep_systUp += err_llep_mtcut*err_llep_mtcut;
        llep_systDn += err_llep_mtcut*err_llep_mtcut;

        // tau eff syst
        datacard << "llep_taueff lnN  - - - - " << 1.+err_llep_tauEff << " " << 1.+err_llep_tauEff << " " << 1.+err_llep_tauEff << " -" << std::endl;
        llep_systUp += err_llep_tauEff*err_llep_tauEff;
        llep_systDn += err_llep_tauEff*err_llep_tauEff;

        // btag syst
        if( iR->nJetsMin()>=7 &&  iR->nBJetsMin()>= 1 ){
          float uncert_heavy = err_llep_btagEff_heavy_7j1b;
          if(  iR->nBJetsMin()==2 )      uncert_heavy =err_llep_btagEff_heavy_7j2b;
          else if( iR->nBJetsMin()==3 )  uncert_heavy = err_llep_btagEff_heavy_7j3b;

          // same for zinv and llep
          datacard << "btageff_heavy lnN  - " << 1.+uncert_heavy << " " << 1.+uncert_heavy << " " << 1.+uncert_heavy << " " << 1.+uncert_heavy << " " << 1.+uncert_heavy << " " << 1.+uncert_heavy << " -" << std::endl;
          llep_systUp += uncert_heavy*uncert_heavy;
          llep_systDn += uncert_heavy*uncert_heavy;

          // same for zinv and llep
          if( iR->nBJetsMin()==3 ) {//only 1 uncert for light for >=3b
            datacard << "btageff_light lnN  - " << 1.+err_llep_btagEff_light_7j3b << " " << 1.+err_llep_btagEff_light_7j3b << " " << 1.+err_llep_btagEff_light_7j3b << " " << 1.+err_llep_btagEff_light_7j3b << " " << 1.+err_llep_btagEff_light_7j3b << " " << 1.+err_llep_btagEff_light_7j3b << " -" << std::endl;
            llep_systUp += err_llep_btagEff_light_7j3b*err_llep_btagEff_light_7j3b;
            llep_systDn += err_llep_btagEff_light_7j3b*err_llep_btagEff_light_7j3b;
          }
        } // end btag syst

         // Gamma function
        float testAlphaLlep16=(Rllep16>0) ? ( (Rllep16>3) ? 2 : Rllep16 ) : lastR_llep16;
        float testAlphaLlep17=(Rllep17>0) ? ( (Rllep17>3) ? 2 : Rllep17 ) : lastR_llep17;
        float testAlphaLlep18=(Rllep18>0) ? ( (Rllep18>3) ? 2 : Rllep18 ) : lastR_llep18;
        float testAlphaLlep=(Rllep>0) ? ( (Rllep>3) ? 2 : Rllep ) : lastR_llep;

        if( iBin < extrapol_bin_llep ) //BIN BY BIN!
          datacard << "llep_CRstat_" << gammaConventionCombined( yield_llep16, yield_llep17, yield_llep18, Round(N_llep_CR), 4,  binName_7j, binName, testAlphaLlep16, testAlphaLlep17, testAlphaLlep18  ) << std::endl;
        else //Extrapolation, so just 1 name
          datacard << "llep_CRstat_" << gammaConventionCombined( yield_llep16, yield_llep17, yield_llep18, Round(N_llep_CR), 4,  llepCR_name, binName, testAlphaLlep16, testAlphaLlep17, testAlphaLlep18 ) << std::endl;

        // Get Poisson uncertainty for table
        double yield_llep_up, yield_llep_dn;
        RooHistError::instance().getPoissonInterval(Round(N_llep_CR),yield_llep_dn,yield_llep_up,1.);
        yield_llep_up *= (Round(N_llep_CR)>0) ? yield_llep/Round(N_llep_CR) : testAlphaLlep;
        yield_llep_dn *= (Round(N_llep_CR)>0) ? yield_llep/Round(N_llep_CR) : testAlphaLlep;
        llep_statUp = yield_llep_up-yield_llep;
        llep_statDn = yield_llep-yield_llep_dn;

        // MC stat. uncertainty, two independent contributions CR, SR
        //float err_llep_mcstat;
        float err_llep_mcstat_cr;
        float err_llep_mcstat_sr;

        if (this_llep_MCcr->GetBinContent(iBin)>0) err_llep_mcstat_cr = this_llep_MCcr->GetBinError(iBin)/this_llep_MCcr->GetBinContent(iBin);
        else err_llep_mcstat_cr = 1.0;
        if (this_llep_MCsr->GetBinContent(iBin)>0) err_llep_mcstat_sr = this_llep_MCsr->GetBinError(iBin)/this_llep_MCsr->GetBinContent(iBin);
        else err_llep_mcstat_sr = 1.0;

        datacard << "llep_MCstat_cr_" << binName << " lnN  - - - - " << 1.+err_llep_mcstat_cr << " " << 1.+err_llep_mcstat_cr << " " << 1.+err_llep_mcstat_cr << " -" << std::endl;
        llep_systUp += err_llep_mcstat_cr*err_llep_mcstat_cr;
        llep_systDn += err_llep_mcstat_cr*err_llep_mcstat_cr;

        datacard << "llep_MCstat_sr_" << binName << " lnN  - - - - " << 1.+err_llep_mcstat_sr << " " << 1.+err_llep_mcstat_sr << " " << 1.+err_llep_mcstat_sr << " -" << std::endl;
        llep_systUp += err_llep_mcstat_sr*err_llep_mcstat_sr;
        llep_systDn += err_llep_mcstat_sr*err_llep_mcstat_sr;

        // Shape uncertainty
        if( nBins > 1 ){
          if( iBin==extrapol_bin_llep && fabs(yield_llep)>0 ){ 
            if(!flag_fixshapeerr_llep){
              datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.-shapeErr_llep/fabs(yield_llep) << " " << 1.-shapeErr_llep/fabs(yield_llep) << " " << 1.-shapeErr_llep/fabs(yield_llep) << " - " << std::endl;
              llep_systUp += (shapeErr_llep/fabs(yield_llep))*(shapeErr_llep/fabs(yield_llep));
              llep_systDn += (shapeErr_llep/fabs(yield_llep))*(shapeErr_llep/fabs(yield_llep));
            }
            else{
              cout<<"llep: region "<<binName<<", shapeErr_llep "<<shapeErr_llep<<", yield_llep "<<yield_llep<<", need to correct this case in extrapol bin and next"<<endl;
              llep_systUp += 1.0;
              llep_systDn += 1.0;
            }
          }
          else{
            if( iBin > extrapol_bin_llep ){ //for hybrid (made such that is works also for pure extrapolation

              if(iBin>extrapol_bin_llep+4||!flag_fixshapeerr_llep){
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.+relativeErr_llep << " " << 1.+relativeErr_llep << " "  << 1.+relativeErr_llep  << " - " << std::endl;
                llep_systUp += relativeErr_llep*relativeErr_llep;
                llep_systDn += relativeErr_llep*relativeErr_llep;
              }
              else if( iBin==extrapol_bin_llep+1 &&flag_fixshapeerr_llep==1){
                float compen_res=shapeErr_llep-fabs(this_llep->GetBinContent(extrapol_bin_llep));
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.+relativeErr_llep-compen_res/yield_llep << " " << 1.+relativeErr_llep-compen_res/yield_llep << " "  << 1.+relativeErr_llep-compen_res/yield_llep  << " - " << std::endl;
                llep_systUp += (relativeErr_llep-compen_res/yield_llep)*(relativeErr_llep-compen_res/yield_llep);
                llep_systDn += (relativeErr_llep-compen_res/yield_llep)*(relativeErr_llep-compen_res/yield_llep);
                cout<<"region "<<binName<<", llep corrected relative error "<<relativeErr_llep-compen_res/yield_llep<<endl;
                cout<<"zinv: corrected the bin next to extrapolbin"<<endl;

              }
              else if ( iBin==extrapol_bin_llep+1 && flag_fixshapeerr_llep>=2){
                llep_systUp += 1.0;
                llep_systDn += 1.0;
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << relativeErr_llep << " " << relativeErr_llep << " "  << relativeErr_llep  << " - " << std::endl;
                cout<<"llep: still need to correct in the second bin next to extropolbin"<<endl;
              }
              else if (iBin==extrapol_bin_llep+2 && flag_fixshapeerr_llep<2 ){
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.+relativeErr_llep << " " << 1.+relativeErr_llep << " "  << 1.+relativeErr_llep  << " - " << std::endl;
                llep_systUp += relativeErr_llep*relativeErr_llep;
                llep_systDn += relativeErr_llep*relativeErr_llep;
              }
              else if (iBin==extrapol_bin_llep+2 && flag_fixshapeerr_llep==2){
                float compen_res=shapeErr_llep-fabs(this_llep->GetBinContent(extrapol_bin_llep))-fabs(this_llep->GetBinContent(extrapol_bin_llep+1));
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.+relativeErr_llep-compen_res/yield_llep << " " << 1.+relativeErr_llep-compen_res/yield_llep << " "  << 1.+relativeErr_llep-compen_res/yield_llep  << " - " << std::endl;
                llep_systUp += (relativeErr_llep-compen_res/yield_llep)*(relativeErr_llep-compen_res/yield_llep);
                llep_systDn += (relativeErr_llep-compen_res/yield_llep)*(relativeErr_llep-compen_res/yield_llep);
               cout<<"region "<<binName<<", llep corrected relative error "<<(relativeErr_llep-(shapeErr_llep-fabs(this_llep->GetBinContent(extrapol_bin_llep))-fabs(this_llep->GetBinContent(extrapol_bin_llep+1)))/yield_llep)<<endl;
               cout<<"llep: corrected the second bin next to extrapolbin"<<endl;

              }

              else if(iBin==extrapol_bin_llep+2 && flag_fixshapeerr_llep>2){
                llep_systUp += 1.0;
                llep_systDn += 1.0;
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << relativeErr_llep << " " << relativeErr_llep << " "  << relativeErr_llep  << " - " << std::endl;
                cout<<"llep: still need to correct in the third bin next to extropolbin"<<endl;
              }

              else if(iBin==extrapol_bin_llep+3 && flag_fixshapeerr_llep<3){
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.+relativeErr_llep << " " << 1.+relativeErr_llep << " "  << 1.+relativeErr_llep  << " - " << std::endl;
                llep_systUp += relativeErr_llep*relativeErr_llep;
                llep_systDn += relativeErr_llep*relativeErr_llep;
              }
              else if(iBin==extrapol_bin_llep+3 && flag_fixshapeerr_llep==3){
                float compen_res=shapeErr_llep-fabs(this_llep->GetBinContent(extrapol_bin_llep))-fabs(this_llep->GetBinContent(extrapol_bin_llep+1))-fabs(this_llep->GetBinContent(extrapol_bin_llep+2));
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.+relativeErr_llep-compen_res/yield_llep << " " << 1.+relativeErr_llep-compen_res/yield_llep << " "  << 1.+relativeErr_llep-compen_res/yield_llep  << " - " << std::endl;
                llep_systUp += (relativeErr_llep-compen_res/yield_llep)*(relativeErr_llep-compen_res/yield_llep);
                llep_systDn += (relativeErr_llep-compen_res/yield_llep)*(relativeErr_llep-compen_res/yield_llep);
                cout<<"region "<<binName<<", llep corrected relative error "<<(relativeErr_llep-compen_res/yield_llep)<<endl;
                cout<<"llep: corrected the third bin next to extrapolbin"<<endl;
              }
              else if(iBin==extrapol_bin_llep+3 && flag_fixshapeerr_llep>3){
                llep_systUp += 1.0;
                llep_systDn += 1.0;
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << relativeErr_llep << " " << relativeErr_llep << " "  << relativeErr_llep  << " - " << std::endl;
                cout<<"llep: still need to correct in the fourth bin next to extropolbin"<<endl;
              }
              else if(iBin==extrapol_bin_llep+4 && flag_fixshapeerr_llep<4){
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.+relativeErr_llep << " " << 1.+relativeErr_llep << " "  << 1.+relativeErr_llep  << " - " << std::endl;
                llep_systUp += relativeErr_llep*relativeErr_llep;
                llep_systDn += relativeErr_llep*relativeErr_llep;
              }
              else if(iBin==extrapol_bin_llep+4 && flag_fixshapeerr_llep==4){
                float compen_res=shapeErr_llep-fabs(this_llep->GetBinContent(extrapol_bin_llep))-fabs(this_llep->GetBinContent(extrapol_bin_llep+1))-fabs(this_llep->GetBinContent(extrapol_bin_llep+2))-fabs(this_llep->GetBinContent(extrapol_bin_llep+3));
                datacard << "llep_shape_" << llepCR_name << " lnN - - - - " << 1.+relativeErr_llep-compen_res/yield_llep << " " << 1.+relativeErr_llep-compen_res/yield_llep << " "  << 1.+relativeErr_llep-compen_res/yield_llep  << " - " << std::endl;
                llep_systUp += (relativeErr_llep-compen_res/yield_llep)*(relativeErr_llep-compen_res/yield_llep);
                llep_systDn += (relativeErr_llep-compen_res/yield_llep)*(relativeErr_llep-compen_res/yield_llep);
                cout<<"region "<<binName<<", llep corrected relative error "<<(relativeErr_llep-compen_res/yield_llep)<<endl;
                cout<<"llep: corrected the fourth bin next to extrapolbin"<<endl;
               if (1.+relativeErr_llep-compen_res/yield_llep<0) cout<<"llep_shape still negative:"<< 1.+relativeErr_llep-compen_res/yield_llep<<", need to correct the fifth bin next to extrapolbin, please check"<<endl;
              }
              else{cout<<"llep:still something wrong, please check"<<endl;}
            }
          }
        } // end shape uncertainty

        llep_nCR = N_llep_CR; // Just for table (CR counts)

      } // yield_llep>=0.

      //////////////////////////////////////
      // QCD SYSTEMATICS
      //////////////////////////////////////
      if(doQCDEstimate){

        if( yield_qcd>0. ) {
          datacard << "qcd_syst_jer" << binName << " lnN - - - - - - - " <<  yield_qcd_syst_jer/yield_qcd  << std::endl;
          float this_var = yield_qcd_syst_jer/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;

          datacard << "qcd_syst_nbjetshape" << binName << " lnN - - - - - - - " <<  yield_qcd_syst_nbjetshape/yield_qcd  << std::endl;
          this_var = yield_qcd_syst_nbjetshape/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;

          datacard << "qcd_syst_njetshape" << binName << " lnN - - - - - - - " <<  yield_qcd_syst_njetshape/yield_qcd  << std::endl;
          this_var = yield_qcd_syst_njetshape/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;

          datacard << "qcd_syst_sigmasoft" << binName << " lnN - - - - - - - " <<  yield_qcd_syst_sigmasoft/yield_qcd  << std::endl;
          this_var = yield_qcd_syst_sigmasoft/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;

          datacard << "qcd_syst_tail" << binName << " lnN - - - - - - - " <<  yield_qcd_syst_tail/yield_qcd  << std::endl;
          this_var = yield_qcd_syst_tail/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;
        }

      } // end do QCD estimate

      datacard.close();

      std::cout << "-> Created template datacard: " << datacardName << std::endl;

      //////////////////////////////////////
      // Final configurations to write table
      //////////////////////////////////////

      // currently only dealing with all years summed up together TODO

      // Make absolute uncertainties for table
      zinvZll_systUp = yield_zinv_zll*sqrt(zinvZll_systUp);
      zinvZll_systDn = yield_zinv_zll*sqrt(zinvZll_systDn);

      llep_systUp = yield_llep*sqrt(llep_systUp);
      llep_systDn = yield_llep*sqrt(llep_systDn);

      if(doQCDEstimate){
        qcd_systUp = yield_qcd*sqrt(qcd_systUp);
        qcd_systDn = yield_qcd*sqrt(qcd_systDn);
      }


      // Print the table:
      table << "### bg_name yield statUp statDown systUp systDown" << std::endl;
      table << "zinv " << yield_zinv_zll << " " << zinvZll_statUp << " " << zinvZll_statDn << "  " << zinvZll_systUp << " " << zinvZll_systDn << std::endl;
      table << "llep    " << yield_llep << " " << llep_statUp << " " << llep_statDn << "  " << llep_systUp << " " << llep_systDn << std::endl;
      if(doQCDEstimate){
        table << "qcd     " << yield_qcd << " " << qcd_statUp << " " << qcd_statDn << "  " << qcd_systUp << " " << qcd_systDn << std::endl;
      }
      table << "data    " << nData  <<std::endl;
      // table << "zinv_nCR    " << std::setprecision(3) << zinv_nCR << std::setprecision(3) <<std::endl;
      table << "zinv_nCR " << std::setprecision(3) << zinv_zll_nCR << std::setprecision(3) <<std::endl;
      table << "llep_nCR    " << std::setprecision(3) << llep_nCR << std::setprecision(3) <<std::endl;
      if(doQCDEstimate){
        table << "qcd_nCR     " << std::setprecision(3) << qcd_nCR << std::setprecision(3)   <<std::endl;
      }

      table.close();

      std::cout << "-> Created BG table: " << tableName << std::endl;


    } // end for MT2 bins

  } // for topological regions
} // if doOnlySig

  ///////////////////////////////////////////////////
  // Signals
  ///////////////////////////////////////////////////
  // Do not do the signal part if the masses are equal
  if(m1==m2 && m11==m22) {
    std::cout << "-> Not going to produce any signal data card! " << std::endl;
    return 0;
  }

  std::vector<MT2Analysis<MT2EstimateAllSigSyst>*> signals;

  std::string modelName = model;
  std::string signal_path = "/analyses_signals_merged.root";
  if (do2016Sig) signal_path = "/analyses.root";
  //signals       = MT2Analysis<MT2EstimateSigContSyst>::readAllSystFromFile( "/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/signalScansFromDominick/"+modelName+"_eth.root", modelName, "isr" );
  signals       = MT2Analysis<MT2EstimateAllSigSyst>::readAllSystFromFile( dir16 + signal_path, modelName, "nominal" ); // the last string is just a nickname I assign to the analysis

  if (signals.size()==0) std::cout << "WARNING: Signal analysis is empty!" << std::endl;

  std::set<MT2Region> regions = signals[0]->getRegions();
  std::cout << "Defined Regions " << std::endl;

  for( unsigned  isig=0; isig<signals.size(); ++isig ) { // loops over all signals present in the file

    // Name convention
    std::string sigName;
    sigName = signals[isig]->getName();
    //sigName = getSimpleSignalName( signals[isig]->getName() );

    std::cout << "Processing signal model " << sigName << std::endl;

    std::string scont = "_sigcontam";
    std::string::size_type pos = sigName.find(scont);
    if(pos != std::string::npos) sigName.erase(pos,scont.length());

    // Local path for datacards
    std::string path_mass = dir16 + "/datacards_" + sigName;
    system(Form("mkdir -p %s", path_mass.c_str()));
    if(path_sig!="") path_mass = path_sig;

    // SE path for datacards
    std::string pathSE = "";
    if (label=="")
      pathSE = dir16 + "/datacards_" + sigName;
    else
      pathSE = dir16 + "/datacards_" + sigName + "_" + label;

    float xs_norm=1.;
    ////// If you need to renormalize T2qq xsec, uncomment
  //    if( signals[isig]->getName().find("T2qq") != std::string::npos )
  //      xs_norm=8./10.;


    // Start loop over topological regions
    std::cout << "  About to start loop over topological region for signal model " << sigName << std::endl;

    for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

      MT2Region* thisRegion = new MT2Region(*iR);
      int extrapol_bin_llep = 1;

      TH3D* this_signal3d_central;
      TH1D* this_signalParent;

      // Read signal analysis, and take 3D histogram if it exists for this region
      MT2EstimateAllSigSyst* thisSigSystCentral = signals[isig]->get(*iR);
      //std::cout << "debug thisSigSystCentral->yield3d" <<  thisSigSystCentral->yield3d << std::endl;
      if( thisSigSystCentral->yield3d!=0 ){
        this_signal3d_central = signals[isig]->get(*iR)->yield3d;
      }

      // Project SUSY parent mass on 1D histogram (to be used to loop over scan masses)
      this_signalParent = this_signal3d_central->ProjectionY("mParent");

      // Initialize histograms for signal systematics
      TH3D* this_signal3d_bTagHeavy_Up;
      TH3D* this_signal3d_bTagLight_Up;
      TH3D* this_signal3d_isr_Up;
      TH3D* this_signal3d_lepEff_Up;

      if( includeSignalUnc ){

        this_signal3d_isr_Up = (TH3D*) signals[isig]->get(*iR)->yield3d_isr_UP->Clone();
        //std::cout << "debug integral isr=" << this_signal3d_isr_Up->Integral() << std::endl;

        this_signal3d_bTagHeavy_Up       = (TH3D*) signals[isig]->get(*iR)->yield3d_btag_heavy_UP->Clone();
        //std::cout << "debug integral btagsf_heavy=" << this_signal3d_bTagHeavy_Up->Integral() << std::endl;

        this_signal3d_bTagLight_Up = (TH3D*) signals[isig]->get(*iR)->yield3d_btag_light_UP->Clone();
        //std::cout << "debug integral btagsf_light=" << this_signal3d_bTagLight_Up->Integral() << std::endl;

        if( addSigLepSF && (( model == "T2tt" || model == "T1tttt" || model == "T2bt" || model == "T2bW"))){
          this_signal3d_lepEff_Up = (TH3D*) signals[isig]->get(*iR)->yield3d_lepsf_UP->Clone();
        }

      } // includeSignalUnc

      // Start loop over SUSY parent mass
      for( int iBinY=1; iBinY<this_signalParent->GetNbinsX()+1; ++iBinY ){

        float mParent = this_signalParent->GetBinLowEdge(iBinY);

        if( !(mParent >= m1-1 && mParent < m2-1) ) {
          //std::cout << "    Skipping data card creation for mParent=" <<  mParent << " since outside requested boundaries" << std::endl;
          continue;
        }
        std::cout << "    Working on mParent=" << mParent << std::endl;

        // Get LSP masses for this parent mass
        TH1D* this_signalLSP = this_signal3d_central->ProjectionZ("mLSP", 0, -1, iBinY, iBinY);

        // Start loop over LSP masses
        for( int iBinZ=1; iBinZ < iBinY; ++iBinZ ) {

          float mLSP = this_signalLSP->GetBinLowEdge(iBinZ);

          if( !(mLSP >= m11-1 && mLSP < m22-1) ) {
            //std::cout << "      Skipping data card creation for mLSP=" <<  mLSP << " since outside requested boundaries" << std::endl;
            continue;
          }
          std::cout << "      Working on mLSP=" << mLSP << std::endl;

          // Get MT2 yield histogram for this mass point
          TH1D* this_signal      = this_signal3d_central->ProjectionX("mt2"     , iBinY, iBinY, iBinZ, iBinZ);
          TH1D* this_signal_reco = this_signal3d_central->ProjectionX("mt2_reco", iBinY, iBinY, iBinZ, iBinZ);

          std::cout << "debug mParent=" << mParent << " mLSP=" << mLSP << " topoReg="<< thisRegion->getName()<< " signal integral=" << this_signal->Integral() << std::endl;
          if( this_signal->Integral() <=0 ) continue;

          //Start loop over MT2 bins
          for( int iBin=1; iBin<this_signal->GetNbinsX()+1; ++iBin ) {
          std::cout << "        MT2 bin=" << iBin << " Region=" << iR->getName() << std::endl;

            //bool includeCR=false;
            //if(iBin==1) includeCR=true;
            //if(iR->nJetsMin()>=7 && iR->nBJetsMin()>1) includeCR=false;

            if( this_signal->GetBinLowEdge( iBin ) > iR->htMax() && iR->htMax()>0 ) continue;

            float mt2Min = this_signal->GetBinLowEdge( iBin );
            float mt2Max = (iBin==this_signal->GetNbinsX()) ?  -1. : this_signal->GetBinLowEdge( iBin+1 );

            if( iR->htMin()==1500 && iR->nJetsMin()>1 && mt2Min==200 ) continue; // why ? FIXME: understand

            // IMPORTANT If bin is empty, do not create card
            if( this_signal->GetBinContent(iBin) <= 0 ) ;
            else{
              std::string binName;
              if( mt2Max>=0. )
                binName = std::string( Form("%s_m%.0fto%.0f", iR->getName().c_str(), mt2Min, mt2Max) );
              else
                binName = std::string( Form("%s_m%.0ftoInf", iR->getName().c_str(), mt2Min) );


              // Get template card for this bin
              std::string templateDatacard( Form("%s/datacard_%s.txt", path_templ.c_str(), binName.c_str()) );

              // Create new card for this bin
              std::string newDatacard(  Form("%s/datacard_%s_%s_%.0f_%.0f.txt",        path_mass.c_str(), binName.c_str(), sigName.c_str(), mParent, mLSP) );
              //std::string helpDatacard( Form("%s/datacard_%s_%s_%.0f_%.0f_forSed.txt", path_mass.c_str(), binName.c_str(), sigName.c_str(), mParent, mLSP) );

              //std::ifstream thisNewDatacard( newDatacard.c_str() );
              //if( thisNewDatacard.good() ) continue; // if data-card exists do not overwrite

              float sig = this_signal->GetBinContent(iBin);
              float sig_syst = 0;
              float sig_reco = this_signal_reco->GetBinContent(iBin);

              if( !( sig_reco > 0) ) continue;
              float sigErr = this_signal_reco->GetBinError(iBin)/sig_reco;

              sig*=xs_norm; // To eventually rescale xsec.
              //sig *= cfg.lumi();   // not needed ATM

              float isrErr;
              float bTagErr_heavy;
              float bTagErr_light;
              float lepEffErr;

              if( includeSignalUnc ) {

                if( sig_reco > 0.){
                  isrErr = this_signal3d_isr_Up->GetBinContent(iBin, iBinY, iBinZ);
                  isrErr = isrErr/sig_reco; //before /sig, now we use the reco yield

                  bTagErr_heavy = this_signal3d_bTagHeavy_Up->GetBinContent(iBin, iBinY, iBinZ);
                  bTagErr_heavy = bTagErr_heavy/sig_reco;// before without the 1+

                  bTagErr_light = this_signal3d_bTagLight_Up->GetBinContent(iBin, iBinY, iBinZ);
                  bTagErr_light = bTagErr_light/sig_reco;// before without the 1+
                  //bTagErr_light = 1. + bTagErr_light/sig;// before without the 1+

                  if( addSigLepSF && (( model == "T2tt" || model == "T1tttt" || model == "T2bt" || model == "T2bW"))){
                    lepEffErr = this_signal3d_lepEff_Up->GetBinContent(iBin, iBinY, iBinZ);
                    lepEffErr = lepEffErr/sig_reco;
                  }
                }else{
                  std::cout << "Signal is zero here, you should not be here" << std::endl;
                  isrErr        = 1.0;
                  bTagErr_heavy = 1.0;
                  bTagErr_light = 1.0;
                  if( addSigLepSF && (( model == "T2tt" || model == "T1tttt" || model == "T2bt" || model == "T2bW"))){
                    lepEffErr   = 1.0;
                  }
                }
              } // end if includeSignalUnc

              float totUncorrErr = 1.+sqrt(sigErr*sigErr+0.05*0.05+0.05*0.05); // MC stat + scales (5%) + JEC (5%)
              //float totUncorrErrCont = 1.+sqrt(sigContErr*sigContErr+0.05*0.05+0.05*0.05); // MC stat + scales (5%) + JEC (5%)

              if(sig<0.) sig=0.;

              // only at the end multiply by lumi the yield
              //sig *= lumiComb; // not needed ATM

              std::string sedCommand( Form("sed 's/XXX/%.3f/' %s > %s", sig, templateDatacard.c_str(), newDatacard.c_str()) );
              system( sedCommand.c_str() );

              std::string sedCommand_uncErr( Form("sed -i 's/UUU/%.3f/' %s", totUncorrErr, newDatacard.c_str()) );
              system( sedCommand_uncErr.c_str() );

              std::string sedCommand_isrErr( Form("sed -i 's/III/%.3f/' %s", isrErr, newDatacard.c_str()) );
              std::string sedCommand_bTagHErr( Form("sed -i 's/HHH/%.3f/' %s", bTagErr_heavy, newDatacard.c_str()) );
              std::string sedCommand_bTagLErr( Form("sed -i 's/LLL/%.3f/' %s", bTagErr_light, newDatacard.c_str()) );
              std::string sedCommand_lepEffErr( Form("sed -i 's/EEE/%.3f/' %s", lepEffErr, newDatacard.c_str()) );

              std::string sedCommand_genErr( Form("sed -i 's/SSS/%.3f/' %s", sig_syst, newDatacard.c_str()) );

              if( includeSignalUnc ){
                system( sedCommand_isrErr.c_str() );
                system( sedCommand_bTagHErr.c_str() );
                system( sedCommand_bTagLErr.c_str() );

                if( addSigLepSF && (( model == "T2tt" || model == "T1tttt" || model == "T2bt" || model == "T2bW" )))
                  system( sedCommand_lepEffErr.c_str() );
              } // end do includeSignalUnc

            } // signal yield > 0
          } // for bins X (MT2)
        } // for bins Z (mLSP)
      } // for bins Y (mParent)
    } // for regions

    std::cout << "-> Created datacards in " << path_mass << std::endl;

  } // for signals
  return 0;

}



std::string getSimpleSignalName( const std::string& longName ) {

  TString longName_tstr(longName);

  longName_tstr.ReplaceAll( "_", " " );
  longName_tstr.ReplaceAll( "mStop", " " );
  longName_tstr.ReplaceAll( "mGluino", " " );
  longName_tstr.ReplaceAll( "mLSP", " " );

  std::istringstream iss(longName_tstr.Data());
  std::vector<std::string> parts;
  do {
    std::string sub;
    iss >> sub;
    parts.push_back(sub);
  } while (iss);

  // parts should be:
  // [0]: SMS
  // [1]: model
  // [2]: parent mass
  // [3]: lsp mass


  std::string simpleName = parts[1] + "_" + parts[2] + "_" + parts[3];

  return simpleName;

}


std::string gammaConventionCombined(float yieldSR16, float yieldSR17, float yieldSR18, int yieldCR, int position, const std::string& corrName, const std::string& uncorrName, float testAlpha16, float testAlpha17, float testAlpha18 ){

  std::string use_uncorrName(uncorrName);
  if( uncorrName=="" )
    use_uncorrName = corrName;

  int precision = 3;

  float yieldsSR[3]   = {yieldSR16, yieldSR17, yieldSR18};
  float testAlphas[3] = {testAlpha16, testAlpha17, testAlpha18};

  float alphas[3]; // this needs to be filled with calculated alpha per year
  std::stringstream line; // this needs to be filled with cr yield data , only one
  line << std::fixed;
  line << std::setprecision(3);
  //std::cout << "debug yieldSR16=" << yieldSR16 << " yieldSR17=" << yieldSR17 << " yieldSR18=" << yieldSR18 << " yieldCR="<< yieldCR << "  corrName=" <<corrName << " uncorrName=" << uncorrName << " testAlpha16=" << testAlpha16 << " testAlpha17=" << testAlpha17 << " testAlpha18=" << testAlpha18 << std::endl;
  for (int i=0; i<3; i++){
    if( yieldCR==0 && yieldsSR[i]==0.){
      if(i==0) line << corrName << "  gmN 0  ";
      alphas[i]=testAlphas[i];
    //} else if (yieldCR>0 && yieldsSR[i]==0.){
    //  if(i==0) line << use_uncorrName << "  gmN 0  ";
    //  alphas[i] = testAlphas[i];
    } else {
      if(i==0) line << corrName << "  gmN " << yieldCR << "   ";
      alphas[i]=yieldsSR[i]/((float)yieldCR);
      precision = 5;
    }
  }

  line << std::setprecision(precision);

  for( int i=0; i<position; ++i )
    line << " - ";

  line << alphas[0] << " " << alphas[1] << " " << alphas[2];
  //std::cout << "debug alphas=" << alphas[0] << " " << alphas[1] << " " << alphas[2] << std::endl;
  for( int i=position+1; i<6; ++i )
    line << " - ";

  std::string line_str = line.str();
  //std::cout << "debug " << line_str << std::endl;
  return line_str;



} // end gammaConventionCombined
