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
bool includeSignalUnc = true; // signal lep eff commented out till available
bool doGenAverage = false; // MG also off,  it's what this syst takes into account in absence of signal contamination
bool addSigLepSF= true;

// Edit these options
bool doQCDEstimate = false;
bool copy2SE = true; // copy datacards to SE

int Round(float d) {
  return (int)(floor(d + 0.5));
}

std::string getSimpleSignalName( const std::string& longName );
std::string gammaConvention( float yieldSR, int yieldCR, int position, const std::string& corrName, const std::string& uncorrName="", float testAlpha=1. );


int main( int argc, char* argv[] ) {


  std::cout << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|              Running createDatacards               |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc != 3 && argc != 7 && argc != 8) {
    std::cout << "USAGE: ./createDatacards [configFileName] [model] [m1] [m2] [m11] [m22] ([label])" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

  cout << "WARNING: for the moment, qcd estimates are not taken into account. Uncomment the corresponding lines when we get the necessary ntuples";

  std::string configFileName(argv[1]);
  MT2Config cfg(configFileName);

  float m1=0.;
  float m2=2000.;

  float m11=0.;
  float m22=2000;

  std::string model(argv[2]);

  if( argc == 7 || argc == 8 ){

    m1  = std::stof( argv[3] );
    m2  = std::stof( argv[4] );

    m11  = std::stof( argv[5] );
    m22  = std::stof( argv[6] );

  }

  std::string label;
  if( argc == 8 )
    label = argv[7];
  else
    label = "";

  std::cout << "Will produce datacards for parent mass between " << m1 << " and " << m2 << ", and LSP mass between " << m11 <<  " and " << m22 << std::endl;

  std::string dir = cfg.getEventYieldDir();
  std::string mc_fileName = dir + "/analyses.root";
  std::string data_fileName = dir + "/analyses.root";

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

  // Reading data analysis (in search region)
  // MT2Analysis<MT2Estimate>* data  = MT2Analysis<MT2Estimate>::readFromFile( data_fileName, "ZJets" );
  MT2Analysis<MT2Estimate>* data  = MT2Analysis<MT2Estimate>::readFromFile( data_fileName, "data" );

  // Reading QCD estimate
  MT2Analysis<MT2Estimate>* qcd_nominal;
  MT2Analysis<MT2Estimate>* qcd_syst_jer;
  MT2Analysis<MT2Estimate>* qcd_syst_nbjetshape;
  MT2Analysis<MT2Estimate>* qcd_syst_njetshape;
  MT2Analysis<MT2Estimate>* qcd_syst_tail;
  MT2Analysis<MT2Estimate>* qcd_syst_sigmasoft;

  if(doQCDEstimate){
      qcd_nominal = MT2Analysis<MT2Estimate>::readFromFile( "/shome/mratti/QCD/qcdEstimate_V0.root", "nominal" );
      qcd_syst_jer = MT2Analysis<MT2Estimate>::readFromFile( "/shome/mratti/QCD/qcdEstimate_V0.root", "syst_jer" );
      qcd_syst_nbjetshape = MT2Analysis<MT2Estimate>::readFromFile( "/shome/mratti/QCD/qcdEstimate_V0.root", "syst_nbjetshape" );
      qcd_syst_njetshape = MT2Analysis<MT2Estimate>::readFromFile( "/shome/mratti/QCD/qcdEstimate_V0.root", "syst_njetshape" );
      qcd_syst_sigmasoft = MT2Analysis<MT2Estimate>::readFromFile( "/shome/mratti/QCD/qcdEstimate_V0.root", "syst_sigmasoft" );
      qcd_syst_tail = MT2Analysis<MT2Estimate>::readFromFile( "/shome/mratti/QCD/qcdEstimate_V0.root", "syst_tail" );
  }

  // Reading invisible Z estimate
  MT2Analysis<MT2Estimate>* zinv_zll;
  MT2Analysis<MT2Estimate>* zinv_zll_alpha;
  MT2Analysis<MT2Estimate>* zinvCR_zll;
  MT2Analysis<MT2Estimate>* purity_zll;
  MT2Analysis<MT2Estimate>* zinv_zll_bin_extrapol;
  //  MT2Analysis<MT2Estimate>* purity_zll_err;

  zinvCR_zll      = MT2Analysis<MT2Estimate>::readFromFile( dir + "/zllControlRegion/data_forZinvEst.root", "data");
  zinv_zll_alpha  = MT2Analysis<MT2Estimate>::readFromFile( dir + "/zinvFromZll.root", "alpha");
  purity_zll      = MT2Analysis<MT2Estimate>::readFromFile( dir + "/zinvFromZll.root", "purity_forHybrid");
  zinv_zll        = MT2Analysis<MT2Estimate>::readFromFile( dir + "/zinvFromZll.root", "ZinvEstimateFromZll_hybrid");
  zinv_zll_bin_extrapol = MT2Analysis<MT2Estimate>::readFromFile( dir + "/zinvFromZll.root", "bin_extrapol");

  zinv_zll->setName("zinv");
  //zinv->addToFile( mc_fileName, true ); // Optionally, to add estimate used for invisible Z estimate to analyses.root

  // Reading lost lepton estimate
  MT2Analysis<MT2Estimate>* llep;
  MT2Analysis<MT2Estimate>* llepCR;
  MT2Analysis<MT2Estimate>* llep_ratio;
  MT2Analysis<MT2Estimate>* llep_bin_extrapol;

  llep = MT2Analysis<MT2Estimate>::readFromFile( dir + "/llepEstimate.root", "llepEstimate" );
  llep_ratio = MT2Analysis<MT2Estimate>::readFromFile( dir + "/llepEstimate.root", "llepRatioMC" );
  llepCR = MT2Analysis<MT2Estimate>::readFromFile( cfg.getEventYieldDir() + "/llepEstimate.root", "hybrid_llepCR" );
  llep_bin_extrapol = MT2Analysis<MT2Estimate>::readFromFile( dir + "/llepEstimate.root", "extrapol_bin" );

  llep->setName( "llep" );
  //llep->addToFile( mc_fileName, true ); // Optionally, to add estimate used for invisible Z estimate to analyses.root



  std::cout << "Read in all the inputs" << std::endl;

  // Getting region set used (from data)
  std::set<MT2Region> regions = data->getRegions();

  std::cout << "Defined Regions " << std::endl;


  // First create template datacards
  std::string path_templ = dir + "/datacard_templates";
  system(Form("mkdir -p %s", path_templ.c_str()));


  // Start loop over TOPOLOGICAL regions
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    ///////////////////////////////////////////////////////
    // Get histograms from all input estimates
    ///////////////////////////////////////////////////////

    // Getting data yield histogram
    TH1D* this_data = data->get(*iR)->yield;


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

    // Get histograms for invisible Z estimate
    TH1D* this_zinv_zll = zinv_zll->get(*iR)->yield;
    TH1D* this_zinv_zll_alpha =  zinv_zll_alpha->get(*iR)->yield;
    TH1D* this_zinvCR_zll  = zinvCR_zll->get(*iR)->yield;
    TH1D* this_zinv_purity_zll = purity_zll->get(*iR)->yield;
    TH1D* this_zinv_zll_bin_extrapol = zinv_zll_bin_extrapol->get(*iR)->yield;
    int zinv_zll_hybridBin = this_zinv_zll_bin_extrapol->GetBinContent(1);

    int Nzll=0; // Initialize variable for number of events in zll CR

     // Get histograms for lost lepton estimate
    TH1D* this_llep = llep->get(*iR)->yield;
    TH1D* this_llep_ratio = llep_ratio->get(*iR)->yield;
    TH1D* this_llepCR = llepCR->get(*iR)->yield;
    TH1D* this_llep_bin_extrapol = llep_bin_extrapol->get(*iR)->yield;
    int llep_hybridBin = this_llep_bin_extrapol->GetBinContent(1);

    float N_llep_CR = this_llepCR->Integral();
    // Regions with N(J)>=7 and N(B)>=1 share same control region (1-2 b jets)
    std::string llepCR_name;
    if(iR->nJetsMin()>=7 && iR->nBJetsMin()>=1){
      MT2Region* thisCR = new MT2Region(iR->htMin(), iR->htMax(), iR->nJetsMin(), iR->nJetsMax(), 1, 2);
      llepCR_name = thisCR->getName();
      std::cout << llepCR_name << std::endl;
    }
    else llepCR_name = iR->getName();

    int nBins = this_data->GetNbinsX(); // Getting total number of bins for this topological region

    // some specific systematics
    if( iR->htMin()==250 && iR->htMax()==450 )
      err_jec_zinv = 0.05;
    else
      err_jec_zinv = 0.02;


    ///////////////////////////////////////////////////////
    // Calculation of uncertainty on hybrid shape
    ///////////////////////////////////////////////////////

    // Calculating shape uncertainty for invisible Z (default: linear extrapolation)
    float shapeErr_zinv=0.;
    for( int iBin=1; iBin<this_data->GetNbinsX()+1; ++iBin ) {
      float relativeErr = 0.0;
      int extrapol_bin = ( use_hybrid ) ?  zinv_zll_hybridBin : 1;

      if( extrapol_bin >= nBins ) continue;

      if( fabs(this_zinv_zll->GetBinContent(iBin))>0 )
        if(  iBin >=  extrapol_bin ) //nec for hybrid
          relativeErr = err_zinv_shape / (nBins - extrapol_bin) * (iBin - extrapol_bin);

      shapeErr_zinv+=relativeErr*fabs(this_zinv_zll->GetBinContent(iBin));
      //std::cout << shapeErr_zinv << std::endl;
    }

    // Calculating shape uncertainty for lost lepton (default: linear extrapolation)
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

    float lastR_zinvZll; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
    float lastR_llep; //To keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.

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


      std::string binName_7j;
      if(iR->nJetsMin()>=7 && iR->nBJetsMin()>=1){
        MT2Region* thisCR = new MT2Region(iR->htMin(), iR->htMax(), iR->nJetsMin(), iR->nJetsMax(), 1, 2);
        if( mt2Max>=0. ) binName_7j = std::string( Form("%s_m%.0fto%.0f", thisCR->getName().c_str(), mt2Min, mt2Max) );
        else binName_7j = std::string( Form("%s_m%.0ftoInf", thisCR->getName().c_str(), mt2Min) );
        std::cout << binName_7j << std::endl;
      }else {
        binName_7j = binName;
      }

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
      std::string binName1L( Form("%s_1L", binName.c_str()) );

      // Start writing template card
      // Read background estimates values
      float yield_llep = fabs(this_llep->GetBinContent(iBin));
      float yield_qcd(0.);
      float yield_qcd_syst_jer(0.);
      float yield_qcd_syst_nbjetshape(0.);
      float yield_qcd_syst_njetshape(0.);
      float yield_qcd_syst_sigmasoft(0.);
      float yield_qcd_syst_tail(0.);

      if(doQCDEstimate){
        yield_qcd = this_qcd ->GetBinContent(iBin);
        std::cout << " region " << iR->getName() << std::endl;
        std::cout << " debug qcd " << yield_qcd << std::endl;
        yield_qcd_syst_jer = this_qcd_syst_jer->GetBinContent(iBin);
        yield_qcd_syst_nbjetshape = this_qcd_syst_nbjetshape->GetBinContent(iBin);
        yield_qcd_syst_njetshape = this_qcd_syst_njetshape->GetBinContent(iBin);
        yield_qcd_syst_tail = this_qcd_syst_tail->GetBinContent(iBin);
        yield_qcd_syst_sigmasoft = this_qcd_syst_sigmasoft->GetBinContent(iBin);
      }


      float yield_zinv = 0;

      float yield_zinv_zll = fabs(this_zinv_zll->GetBinContent(iBin));

      //float N_obs = yield_llep + yield_qcd + yield_zinv_zll;


      datacard << "imax 1" << std::endl; //Number of bins
      datacard << "jmax 3" << std::endl; //Number of backgrounds
      datacard << "kmax *" << std::endl; //Number of nuisances
      datacard << "-------------" << std::endl;
      datacard << std::endl << std::endl;


      datacard << std::fixed;
      datacard << std::setprecision(3) << std::endl << std::endl;
      datacard << "bin  " << binName << std::endl;
      datacard << "observation  " << this_data->GetBinContent(iBin) << std::endl;
      datacard << "-------------" << std::endl;
      datacard << std::endl << std::endl;

      // sig qcd zinv llep
      datacard << "bin \t" << binName << "\t" << binName << "\t" << binName << "\t" << binName << std::endl;
      datacard << "process \t sig \t zinv \t llep \t qcd" << std::endl;
      datacard << "process \t 0 \t 1 \t 2 \t 3" << std::endl;
      datacard << "rate \t XXX";
      //      datacard << "rate \t 0.01";
      datacard << " \t " << yield_zinv_zll << " \t " << yield_llep << " \t " << yield_qcd << std::endl;
      datacard << "-------------" << std::endl;

      datacard << "lumi_syst    lnN    " << 1.+err_lumi_corr << " - - -" << std::endl;
      datacard << "PU_syst    lnN    " << 1.+err_sig_PU << " - - -" << std::endl;

      datacard << "lep_eff    lnN    - " << 1. + err_lep_eff << " " << 1. + err_lep_eff << "  - "  << std::endl;
      datacard << "jec    lnN    - " << 1.+ err_jec_zinv  << " " << 1.+ err_jec_llep << "  - "  << std::endl;
      datacard << "renorm    lnN    - " << 1. + err_llep_renorm << " " << 1. + err_llep_renorm << "  - "  << std::endl;

      if(!includeSignalUnc)
        datacard << "sig_syst_" << binName << " lnN 1.2 - - -" << std::endl;
      else{
        datacard << "sig_MCstat_" << binName << " lnN UUU - - -" << std::endl;
        if (doGenAverage) datacard << "sig_gensyst lnU SSS - - -" << std::endl;
        datacard << "sig_isrSyst lnN III - - -" << std::endl;
        datacard << "sig_bTagHeavySyst lnN HHH - - -" << std::endl;
        datacard << "sig_bTagLightSyst lnN LLL - - -" << std::endl;
        if( addSigLepSF && ((model=="T2tt" || model=="T1tttt" || model == "T2bt" || model == "T2bW")) )
          datacard << "sig_lepEffSyst lnN EEE - - -" << std::endl; // Include lepton eff. uncertainty only for T2tt and T1tttt
      }


      // Initialize variables for tables
      float zinvZll_statUp = 0.;
      float zinvZll_statDn = 0.;
      float zinvZll_systUp = 0.;
      float zinvZll_systDn = 0.;

      float qcd_statUp = 0.; // for the moment zero
      float qcd_statDn = 0.; // for the moment zero
      float qcd_systUp = 0.; // total syst
      float qcd_systDn = 0.; // total syst

      float llep_statUp = 0.;
      float llep_statDn = 0.;
      float llep_systUp = 0.;
      float llep_systDn = 0.;

      int zinv_zll_nCR = 0;
      int qcd_nCR  = 0;
      int llep_nCR = 0;

      float summedErr = err_jec_zinv* err_jec_zinv +  err_jec_llep*err_jec_llep + err_llep_renorm*err_llep_renorm;
      zinvZll_systUp += summedErr;
      zinvZll_systDn += summedErr;
      llep_systUp    += summedErr;
      llep_systDn    += summedErr;

      std::string zinvCR_name;
      zinvCR_name = iR->getName();

      //////////////////////////////////////
      // Z INVISIBLE SYSTEMATICS
      //////////////////////////////////////
      if( yield_zinv>=0.  || yield_zinv_zll>=0. ) { // par0

              Nzll = Round(this_zinvCR_zll->Integral());

              float p_zll = this_zinv_purity_zll->GetBinContent( iBin );
              float err_zinvZll_purity = (this_zinv_purity_zll->GetBinError(iBin));

              datacard << "zinvDY_Rsfof  lnN  - " << 1.+ 0.15*(1.-p_zll)  << " - -" << std::endl;
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
                  std::cout << p_zll << "  " << p_zllUp << "  " << p_zllDn << std::endl;
                  datacard << "zinvDY_purity_" << zinvCR_name << " lnN  - " << 1.+ p_zllUp  << " - -" << std::endl;
                  zinvZll_systUp += p_zllUp*p_zllUp;
                  zinvZll_systDn += p_zllDn*p_zllDn;

                }else{ //purity = 0 //this happens at 36ifb still for a few regions
                  datacard << "zinvDY_purity_" << zinvCR_name << " lnN  - 1.0/0.01 - - - -" << std::endl;
                  zinvZll_systUp += 1.0;
                  zinvZll_systDn += 1.0;
                }
              }//End of zll purity stuff // end par4

              // Get Z/Zll ratio
              float R_zll = fabs(this_zinv_zll_alpha->GetBinContent(iBin));
              float relativeErr_zll = fabs(this_zinv_zll_alpha->GetBinError(iBin));

              // Keep information on last non-zero ratio. Will use last non-zero ratio if ratio for one bin is zero.
              if (R_zll>0) lastR_zinvZll = R_zll;

              int extrapol_bin = ( use_hybrid ) ?  zinv_zll_hybridBin : 1;
              if( fabs(this_zinv_zll->GetBinContent(iBin))>0 && iBin >= zinv_zll_hybridBin )
                relativeErr_zll = err_zinv_shape / (nBins-extrapol_bin) * (iBin-extrapol_bin);
              else
                relativeErr_zll = 0.0;


              datacard << "zinvDY_CRstat_" << std::setprecision(3) << gammaConvention( yield_zinv_zll, Nzll, 1, zinvCR_name, binName, (R_zll>0) ? R_zll : lastR_zinvZll ) << std::setprecision(3) << std::endl;

              if( nBins>1 ){
                if( iBin==extrapol_bin && fabs(yield_zinv_zll)>0 ){ //For shape uncertainty on 1st extrapolated MT2 bin, take 1-tot. unc. form other bins (PIVOT at second bin)
                  datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.-shapeErr_zinv/fabs(yield_zinv_zll) << " - - " << std::endl;
                  zinvZll_systUp += (shapeErr_zinv/fabs(yield_zinv_zll))*(shapeErr_zinv/fabs(yield_zinv_zll));
                  zinvZll_systDn += (shapeErr_zinv/fabs(yield_zinv_zll))*(shapeErr_zinv/fabs(yield_zinv_zll));
                }else{
                  if( iBin > extrapol_bin ){ //for hybrid (made such that is works also for pure extrapolation
                    datacard << "zinvDY_shape_" << zinvCR_name << " lnN  - " << 1.+relativeErr_zll << " - - " << std::endl;
                    zinvZll_systUp += relativeErr_zll*relativeErr_zll;
                    zinvZll_systDn += relativeErr_zll*relativeErr_zll;
                  }
                }
              }

              double yield_zinv_zll_up, yield_zinv_zll_dn;
              RooHistError::instance().getPoissonInterval(Nzll,yield_zinv_zll_dn,yield_zinv_zll_up,1.);
              yield_zinv_zll_up *= (Nzll>0) ? yield_zinv_zll/Nzll : (R_zll>0) ?  R_zll : lastR_zinvZll;
              yield_zinv_zll_dn *= (Nzll>0) ? yield_zinv_zll/Nzll : (R_zll>0) ?  R_zll : lastR_zinvZll;

              zinvZll_statUp = yield_zinv_zll_up - yield_zinv_zll;
              zinvZll_statDn = yield_zinv_zll    - yield_zinv_zll_dn;

                // Uncertainty on transfer factor
              float alphaErr_zll;
              if ( R_zll>0 ) alphaErr_zll= this_zinv_zll_alpha->GetBinError(iBin)/R_zll;
              else alphaErr_zll=1.0;
              datacard << "zinvDY_alphaErr_" << binName << " lnN  - " << 1.+alphaErr_zll << " - -" << std::endl;

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

        int extrapol_bin_llep = ( use_hybrid ) ?  llep_hybridBin : 1;
        float relativeErr_llep;
        if( fabs( this_llep->GetBinContent(iBin))>0 && iBin >= extrapol_bin_llep )
          relativeErr_llep = err_llep_shape / (nBins - extrapol_bin_llep) * (iBin - extrapol_bin_llep);
        else
          relativeErr_llep = 0.0;

        // mt cut syst
              datacard << "llep_mtcut lnN  - - " << 1.+err_llep_mtcut << " -" << std::endl;
        llep_systUp += err_llep_mtcut*err_llep_mtcut;
        llep_systDn += err_llep_mtcut*err_llep_mtcut;

        // tau eff syst
              datacard << "llep_taueff lnN  - - " << 1.+err_llep_tauEff << " -" << std::endl;
        llep_systUp += err_llep_tauEff*err_llep_tauEff;
        llep_systDn += err_llep_tauEff*err_llep_tauEff;

              // btag syst
        if( iR->nJetsMin()>=7 &&  iR->nBJetsMin()>= 1 ){
          float uncert_heavy = err_llep_btagEff_heavy_7j1b;
          if(  iR->nBJetsMin()==2 )      uncert_heavy =err_llep_btagEff_heavy_7j2b;
          else if( iR->nBJetsMin()==3 )  uncert_heavy = err_llep_btagEff_heavy_7j3b;

          datacard << "btageff_heavy lnN  - " << 1.+uncert_heavy << " " << 1.+uncert_heavy  << " -" << std::endl;
          llep_systUp += uncert_heavy*uncert_heavy;
          llep_systDn += uncert_heavy*uncert_heavy;

          if( iR->nBJetsMin()==3 ) {//only 1 uncert for light for >=3b
            datacard << "btageff_light lnN  - " << 1.+err_llep_btagEff_light_7j3b << " " << 1.+err_llep_btagEff_light_7j3b << " -" << std::endl;
            llep_systUp += err_llep_btagEff_light_7j3b*err_llep_btagEff_light_7j3b;
            llep_systDn += err_llep_btagEff_light_7j3b*err_llep_btagEff_light_7j3b;
                }
              } // end btag syst
        // missing theory uncertainties

         // Gamma function, or lnU for simultaneous fit
              if( iBin < extrapol_bin_llep ) //BIN BY BIN!
                datacard << "llep_CRstat_" << gammaConvention( yield_llep, Round(N_llep_CR), 2,  binName_7j, binName, (Rllep>0) ? ( (Rllep>3) ? 2 : Rllep ) : lastR_llep ) << std::endl;
        else //Extrapolation, so just 1 name
          datacard << "llep_CRstat_" << gammaConvention( yield_llep, Round(N_llep_CR), 2,  llepCR_name, binName, (Rllep>0) ? ( (Rllep>3) ? 2 : Rllep ) : lastR_llep ) << std::endl;

        // Get Poisson uncertainty for table
        double yield_llep_up, yield_llep_dn;
        RooHistError::instance().getPoissonInterval(Round(N_llep_CR),yield_llep_dn,yield_llep_up,1.);
        yield_llep_up *= (Round(N_llep_CR)>0) ? yield_llep/Round(N_llep_CR) : (Rllep>0) ? ( (Rllep>3) ? 2 : Rllep ) : lastR_llep;
        yield_llep_dn *= (Round(N_llep_CR)>0) ? yield_llep/Round(N_llep_CR) : (Rllep>0) ? ( (Rllep>3) ? 2 : Rllep ) : lastR_llep;
        llep_statUp = yield_llep_up-yield_llep;
        llep_statDn = yield_llep-yield_llep_dn;

        //if( yield_llep>=0. ) { // but we are already in this condition

        // MC stat. uncertainty
        float err_llep_mcstat;
        if(yield_llep>0) err_llep_mcstat = this_llep->GetBinError(iBin)/this_llep->GetBinContent(iBin);
        else err_llep_mcstat = 1.0;

        datacard << "llep_MCstat_" << binName << " lnN  - - " << 1.+err_llep_mcstat << " -" << std::endl;
        llep_systUp += err_llep_mcstat*err_llep_mcstat;
        llep_systDn += err_llep_mcstat*err_llep_mcstat;

        // Shape uncertainty
        if( nBins > 1 ){
          if( iBin==extrapol_bin_llep && yield_llep>0 ){
            datacard << "llep_shape_" << llepCR_name << " lnN - - " << 1.-shapeErr_llep/yield_llep << " - " << std::endl;
            llep_systUp += (shapeErr_llep/yield_llep)*(shapeErr_llep/yield_llep);
            llep_systDn += (shapeErr_llep/yield_llep)*(shapeErr_llep/yield_llep);
          }
          else{
            if( iBin > extrapol_bin_llep ){ //for hybrid (made such that is works also for pure extrapolation
              datacard << "llep_shape_" << llepCR_name << " lnN - - " << 1+relativeErr_llep << " - " << std::endl;
              llep_systUp += relativeErr_llep*relativeErr_llep;
              llep_systDn += relativeErr_llep*relativeErr_llep;
            }
          }
        } // end shape uncertainty

         //} // yield llep > 0
        llep_nCR = N_llep_CR; // Just for table (CR counts)

      } // yield_llep>=0.

      //////////////////////////////////////
      // QCD SYSTEMATICS
      //////////////////////////////////////
      if(doQCDEstimate){

        if( yield_qcd>=0. ) {
          datacard << "qcd_syst_jer" << binName << " lnN - - - " <<  yield_qcd_syst_jer/yield_qcd  << std::endl;
          float this_var = yield_qcd_syst_jer/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;

          datacard << "qcd_syst_nbjetshape" << binName << " lnN - - - " <<  yield_qcd_syst_nbjetshape/yield_qcd  << std::endl;
          this_var = yield_qcd_syst_nbjetshape/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;

          datacard << "qcd_syst_njetshape" << binName << " lnN - - - " <<  yield_qcd_syst_njetshape/yield_qcd  << std::endl;
          this_var = yield_qcd_syst_njetshape/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;

          datacard << "qcd_syst_sigmasoft" << binName << " lnN - - - " <<  yield_qcd_syst_sigmasoft/yield_qcd  << std::endl;
          this_var = yield_qcd_syst_sigmasoft/yield_qcd-1.;
          qcd_systUp += this_var*this_var;
          qcd_systDn += this_var*this_var;

          datacard << "qcd_syst_tail" << binName << " lnN - - - " <<  yield_qcd_syst_tail/yield_qcd  << std::endl;
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

      // Make absolute uncertainties for table
      zinvZll_systUp = yield_zinv_zll*sqrt(zinvZll_systUp);
      zinvZll_systDn = yield_zinv_zll*sqrt(zinvZll_systDn);

      llep_systUp = yield_llep*sqrt(llep_systUp);
      llep_systDn = yield_llep*sqrt(llep_systDn);

      if(doQCDEstimate){
        qcd_systUp = yield_qcd*sqrt(qcd_systUp);
        qcd_systDn = yield_qcd*sqrt(qcd_systDn);
      }

      int nData = Round(this_data->GetBinContent(iBin));

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


  ///////////////////////////////////////////////////
  // Signals
  ///////////////////////////////////////////////////
  // Do not do the signal part if the masses are equal
  if(m1==m2 && m11==m22) {
    std::cout << "-> Not going to produce any signal data card! " << std::endl;
    return 0;
  }

  std::vector<MT2Analysis<MT2EstimateSigContSyst>*> signals;
  std::vector<MT2Analysis<MT2EstimateAllSigSyst>*> signals_isr;
  std::vector<MT2Analysis<MT2EstimateAllSigSyst>*> signals_bTagHeavy;
  std::vector<MT2Analysis<MT2EstimateAllSigSyst>*> signals_bTagLight;
  std::vector<MT2Analysis<MT2EstimateAllSigSyst>*> signals_lepEff;

  std::string modelName = model;

  //signals       = MT2Analysis<MT2EstimateSigContSyst>::readAllSystFromFile( "/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/signalScansFromDominick/"+modelName+"_eth.root", modelName, "isr" );
  signals       = MT2Analysis<MT2EstimateSigContSyst>::readAllSystFromFile( dir + "/analyses.root", modelName, "isr" ); // the last string is just a nickname I assign to the analysis

  if( includeSignalUnc ){
    //signals_isr       = MT2Analysis<MT2EstimateSigSyst>::readAllSystFromFile( "/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/signalScansFromDominick/"+modelName+"_eth.root", modelName, "isr" );
    //signals_bTagHeavy = MT2Analysis<MT2EstimateSigSyst>::readAllSystFromFile( "/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/signalScansFromDominick/"+modelName+"_eth.root", modelName, "btagsf_heavy" );
    //signals_bTagLight = MT2Analysis<MT2EstimateSigSyst>::readAllSystFromFile( "/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/signalScansFromDominick/"+modelName+"_eth.root", modelName, "btagsf_light" );
    signals_isr       = MT2Analysis<MT2EstimateAllSigSyst>::readAllSystFromFile( dir + "/analyses.root", modelName, "isr" );
    signals_bTagHeavy = MT2Analysis<MT2EstimateAllSigSyst>::readAllSystFromFile( dir + "/analyses.root", modelName, "btagsf_heavy" );
    signals_bTagLight = MT2Analysis<MT2EstimateAllSigSyst>::readAllSystFromFile( dir + "/analyses.root", modelName, "btagsf_light" );

    if( addSigLepSF && (( model == "T2tt" || model == "T1tttt" || model == "T2bt" || model == "T2bW" )) ){
      //signals_lepEff = MT2Analysis<MT2EstimateSigSyst>::readAllSystFromFile( "/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/signalScansFromDominick/"+modelName+"_eth.root", modelName, "lepeff" );
      signals_lepEff = MT2Analysis<MT2EstimateAllSigSyst>::readAllSystFromFile( dir + "/analyses.root", modelName, "lepeff" );
    }
  }

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
    std::string path = dir + "/datacards_" + sigName;
    system(Form("mkdir -p %s", path.c_str()));

    // SE path for datacards
    std::string pathSE = "";
    if (label=="")
      pathSE = dir + "/datacards_" + sigName;
    else
      pathSE = dir + "/datacards_" + sigName + "_" + label;

    std::string path_mass = path;

    float xs_norm=1.;
    ////// If you need to renormalize T2qq xsec, uncomment
//    if( signals[isig]->getName().find("T2qq") != std::string::npos )
//      xs_norm=8./10.;


    // Start loop over topological regions
    std::cout << "  About to start loop over topological region for signal model " << sigName << std::endl;

    for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

      MT2Region* thisRegion = new MT2Region(*iR);
      //cout << "Region: " << thisRegion->getName() << endl;

      //For signal contamination
      TH1D* this_llep_bin_extrapol = llep_bin_extrapol->get(*iR)->yield;
      int llep_hybridBin = this_llep_bin_extrapol->GetBinContent(1);
      int extrapol_bin_llep = ( use_hybrid ) ?  llep_hybridBin : 1;

      TH3D* this_signal3d_central;
      TH1D* this_signalParent;

      // Read signal analysis, and take 3D histogram if it exists for this region
      MT2EstimateSigContSyst* thisSigSystCentral = signals[isig]->get(*iR);

      //MT2Estimate* thisSigSystCentral = signalVeto[isig]->get(*iR);

      if( thisSigSystCentral->yield3d!=0 ){
        this_signal3d_central        = signals[isig]->get(*iR)->yield3d;
      }

      // Project SUSY parent mass on 1D histogram (to be used to loop over scan masses)
      this_signalParent = this_signal3d_central->ProjectionY("mParent");

      // Initialize histograms for signal systematics
      TH3D* this_signal3d_bTagHeavy_Up;
      TH3D* this_signal3d_bTagLight_Up;
      TH3D* this_signal3d_isr_Up;
      TH3D* this_signal3d_lepEff_Up;

      if( includeSignalUnc ){

        this_signal3d_isr_Up = (TH3D*) signals_isr[isig]->get(*iR)->yield3d_isr_UP->Clone();
        //std::cout << "debug integral isr=" << this_signal3d_isr_Up->Integral() << std::endl;

        this_signal3d_bTagHeavy_Up       = (TH3D*) signals_bTagHeavy[isig]->get(*iR)->yield3d_btag_heavy_UP->Clone();
        //std::cout << "debug integral btagsf_heavy=" << this_signal3d_bTagHeavy_Up->Integral() << std::endl;

        this_signal3d_bTagLight_Up = (TH3D*) signals_bTagLight[isig]->get(*iR)->yield3d_btag_light_UP->Clone();
        //std::cout << "debug integral btagsf_light=" << this_signal3d_bTagLight_Up->Integral() << std::endl;

        if( addSigLepSF && (( model == "T2tt" || model == "T1tttt" || model == "T2bt" || model == "T2bW"))){
          this_signal3d_lepEff_Up = (TH3D*) signals_lepEff[isig]->get(*iR)->yield3d_lepsf_UP->Clone();
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
          TH1D* this_signal_syst = this_signal3d_central->ProjectionX("mt2_syst", iBinY, iBinY, iBinZ, iBinZ);
          TH1D* this_signal_reco = this_signal3d_central->ProjectionX("mt2_reco", iBinY, iBinY, iBinZ, iBinZ);

          //std::cout << "got integrated signal yield of " << this_signal_reco->Integral() << std::endl;

          if (doGenAverage && signals[isig]->get(*iR)->yield3d_genmet!=0) {
            TH3D* this_signal3d_central_genmet = signals[isig]->get(*iR)->yield3d_genmet;
            TH1D* this_signal_genmet = this_signal3d_central_genmet->ProjectionX("mt2_genmet", iBinY, iBinY, iBinZ, iBinZ);

            std::string SignalRegionName = iR->getName();

            this_signal->Add(this_signal_genmet);
            this_signal->Scale(0.5);

            this_signal_syst->Add(this_signal_genmet, -1.0);
            this_signal_syst->Scale(0.5); // half difference between gen and reco

          }

          //for( int iBin=1; iBin<this_signal->GetNbinsX()+1; ++iBin ) {
            //std::cout << "debug iBin=" << iBin << " iBinY=" << iBinY << " iBinZ=" << iBinZ << " sigYield="<< this_signal->GetBinContent(iBin) << std::endl;
          //}


          if( this_signal->Integral() <=0 ) continue;


          ////// Signal contamination
          TH1D* this_signalContamination;
          TH1D* this_signalContamination_syst;

          TH3D* this_signal3d_crsl;
          TH1D* this_signal_crsl;
          TH1D* this_signal_crsl_syst;
          TH1D* this_signal_alpha;

          // MG: leaving this part but switchiing off doSignalContamination
          if( (model == "T2tt" || model == "T1tttt" || model == "T2bt" || model == "T2bW" ) && doSignalContamination ){
            this_signal3d_crsl        = (TH3D*) signals[isig]->get(*iR)->yield3d_crsl       ->Clone();
            //this_signal3d_crsl = (TH3D*) signalVeto_1lCR[isig]->get(*iR)->yield3d->Clone();
            this_signal_crsl        = this_signal3d_crsl       ->ProjectionX("mt2_crsl", iBinY, iBinY, iBinZ, iBinZ);
            this_signal_alpha  = (TH1D*) signals[isig]->get(*iR)->yield_alpha->Clone();

            if (doGenAverage && signals[isig]->get(*iR)->yield3d_crsl_genmet!=0){
              TH3D *this_signal3d_crsl_genmet = (TH3D*) signals[isig]->get(*iR)->yield3d_crsl_genmet->Clone();
              TH1D *this_signal_crsl_genmet = this_signal3d_crsl_genmet->ProjectionX("mt2_crsl", iBinY, iBinY, iBinZ, iBinZ);

              std::string SignalRegionName = iR->getName();
              std::cout << "Averaging SC in region " << SignalRegionName << std::endl;
              std::cout << this_signal_crsl->GetBinContent(1) << std::endl;
              std::cout << this_signal_crsl_genmet->GetBinContent(1) << std::endl;

              this_signal_crsl->Add(this_signal_crsl_genmet);
              this_signal_crsl->Scale(0.5);

              this_signal_crsl_syst= this_signal3d_crsl->ProjectionX("mt2_crsl_syst", iBinY, iBinY, iBinZ, iBinZ);
              this_signal_crsl_syst->Add(this_signal_crsl_genmet,-1.0);
              this_signal_crsl_syst->Scale(0.5);


            }else{
              std::string SignalRegionName = iR->getName();
               std::cout << "Not averaging SC in region " << SignalRegionName << std::endl;
              std::cout << this_signal_crsl->GetBinContent(1) << std::endl;

            }
            //	  //If want to replace yield in 1l CR for signal contamination
            //	  TH1D* this_signal_veto_1lCR = (TH1D*) signalVeto_1lCR[isig]->get(*iR)->yield->Clone();
            //	  // Then, below replace this_signal_crsl with this histogram;

            if( doSimultaneousFit )
              this_signalContamination = (TH1D*) this_signal_crsl->Clone();
            else{
              std::string SignalRegionName = iR->getName();
              std::cout << "scaling by alpha in region " << SignalRegionName << std::endl;

              this_signalContamination = (TH1D*) this_signal_alpha->Clone();
              if(doGenAverage)
                this_signalContamination_syst = (TH1D*) this_signal_alpha->Clone("mt2_cont_syst");

              TH1D* alpha_copy = (TH1D*) this_signal_alpha->Clone();

              //Hybrid method for llep also for signal contamination
              for( int iBin=1; iBin<this_signal->GetNbinsX()+1; ++iBin ) {

                if(iBin < extrapol_bin_llep){ // BIN BY BIN CASE
                  this_signalContamination->SetBinContent( iBin, this_signalContamination->GetBinContent(iBin) * this_signal_crsl->GetBinContent(iBin) );
                  if(doGenAverage){
                    this_signalContamination_syst->SetBinContent( iBin, this_signalContamination_syst->GetBinContent(iBin) * this_signal_crsl_syst->GetBinContent(iBin)  );
                  }
                }else{ //EXTRAPOLATION CASE
                  this_signalContamination->SetBinContent( iBin, alpha_copy->GetBinContent(iBin) * alpha_copy->GetBinContent(iBin)/alpha_copy->Integral(extrapol_bin_llep, -1) * this_signal_crsl->Integral(extrapol_bin_llep, -1) );
                  if(doGenAverage){
                    this_signalContamination_syst->SetBinContent( iBin, alpha_copy->GetBinContent(iBin) * alpha_copy->GetBinContent(iBin)/alpha_copy->Integral(extrapol_bin_llep, -1) * this_signal_crsl_syst->Integral(extrapol_bin_llep, -1) );
                  }
                }
              } // hybrid method for llep also for signal contamination
            } // else doSimultaneousFit
          } // model with signal contamination
          else{  // MG: case of interest !
            this_signalContamination = (TH1D*) this_signal->Clone();
            this_signalContamination->Scale(0.);
            if(doGenAverage){
              this_signalContamination_syst = (TH1D*) this_signal_syst->Clone("mt2_cont_syst");
              this_signalContamination_syst->Scale(0.);
            }
          }

          //Start loop over MT2 bins
          for( int iBin=1; iBin<this_signal->GetNbinsX()+1; ++iBin ) {
          std::cout << "        MT2 bin=" << iBin << " Region=" << iR->getName() << std::endl;

            bool includeCR=false;
            if(iBin==1) includeCR=true;
            if(iR->nJetsMin()>=7 && iR->nBJetsMin()>1) includeCR=false;

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

              // If datacard exists already on SE, do not create it again
              Long_t id;
              Long_t flags;
              Long_t modtime;
              Long_t size;
              std::string fullPathSE;
              int checkFileSE;
              std::string rmOnSE;
              if( copy2SE ){
                fullPathSE = Form("/pnfs/psi.ch/cms/trivcat/store/user/`whoami`/%s/datacards_%.0f_%.0f/datacard_%s_%s_%.0f_%.0f.txt", pathSE.c_str(), mParent, mLSP, binName.c_str(), sigName.c_str(), mParent, mLSP);
                checkFileSE = (int) gSystem->GetPathInfo(fullPathSE.c_str(), &id, &size, &flags, &modtime);
                std::cout << fullPathSE << "\t" << checkFileSE << "\t" <<size<< std::endl;

                //std::string rmOnSE( Form("env --unset=LD_LIBRARY_PATH gfal-rm gsiftp://t3se01.psi.ch/%s", fullPathSE.c_str()) );
                rmOnSE = Form("gfal-rm gsiftp://t3se01.psi.ch/%s", fullPathSE.c_str()) ;

                if( checkFileSE==0 && (size)==0 ){

                  std::cout << "Removing. File " << fullPathSE << " exists and has zero size " << (size) << ". Removing." << std::endl;
                  system( rmOnSE.c_str() );

                }
                else if ( checkFileSE==0 && (size)>0 ){

                  std::cout << "Skipping. File " << fullPathSE << " exists and has non-zero size  " << (size) << ". Skipping." << std::endl;

                  continue;

                }
              } // end if copy2SE

              // Get template card for this bin
              std::string templateDatacard( Form("%s/datacard_%s.txt", path_templ.c_str(), binName.c_str()) );

              // Create new card for this bin
              std::string newDatacard( Form("%s/datacard_%s_%s_%.0f_%.0f.txt", path_mass.c_str(), binName.c_str(), sigName.c_str(), mParent, mLSP) );
              std::string helpDatacard( Form("%s/datacard_%s_%s_%.0f_%.0f_forSed.txt", path_mass.c_str(), binName.c_str(), sigName.c_str(), mParent, mLSP) );

              std::ifstream thisNewDatacard( newDatacard.c_str() );
              //if( thisNewDatacard.good() ) continue; // if data-card exists do not overwrite

              float sig = this_signal->GetBinContent(iBin);
              float sig_syst = 0;
              float sig_reco = this_signal_reco->GetBinContent(iBin);

              if( !( sig_reco > 0) ) continue;
              float sigErr = this_signal_reco->GetBinError(iBin)/sig_reco;

              sig*=xs_norm; // To eventually rescale xsec.
              //Scaling to lumi (so one doesn't have to reloop to change lumi), for ETH, not for SnT histograms
              //sig *= cfg.lumi(;)

              // Siganl Contamination
              double sigContErr = 0.0;
              double sigCont = 0.0;
              if(doSignalContamination && doSimultaneousFit){
                sigCont = this_signalContamination->IntegralAndError(1, -1, sigContErr);
              }else if(doSignalContamination && !doSimultaneousFit){
                sigCont = this_signalContamination->GetBinContent(iBin);
                sigContErr = this_signalContamination->GetBinError(iBin);
              }
              sigContErr = (sigCont > 0) ? fabs(sigContErr)/sigCont : 0.0;
              //

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
              float totUncorrErrCont = 1.+sqrt(sigContErr*sigContErr+0.05*0.05+0.05*0.05); // MC stat + scales (5%) + JEC (5%)

              //float totUncorrErr = 1.+sqrt(sigErr*sigErr+0.05*0.05+0.1*0.1); // MC stat + scales (5%) + JEC (10%)
              //float totUncorrErrCont = 1.+sqrt(sigContErr*sigContErr+0.05*0.05+0.1*0.1); // MC stat + scales (5%) + JEC (10%)

              if(doSignalContamination && !doSimultaneousFit) sig=sig-sigCont;
              else if(!doSignalContamination) {
                sigCont=0.;
                totUncorrErrCont=0.;
              }
              if(sig<0.) sig=0.;

              if(doGenAverage){
                sig_syst = 1 + fabs((this_signal_syst->GetBinContent(iBin)-this_signalContamination_syst->GetBinContent(iBin))/(sig !=0 ? sig : 1.0)); // cont_syst=0 if no doSignalCont
                if ( (this_signal_syst->GetBinContent(iBin)-this_signalContamination_syst->GetBinContent(iBin))*sig < 0 )
                  sig_syst = 1/sig_syst; // to account for negative variation
                  //std::cout << "debug sig_syst" << sig_syst << std::endl;
              }

              std::string mvCommand( Form("mv %s %s", newDatacard.c_str(), helpDatacard.c_str()) );
              std::string rmCommand( Form("rm -f %s", helpDatacard.c_str()) );

              std::string sedCommand( Form("sed 's/XXX/%.3f/' %s > %s", sig, templateDatacard.c_str(), newDatacard.c_str()) );
              system( sedCommand.c_str() );

              std::string sedCommand_sigCont( Form("sed -i 's/YYY/%.3f/' %s", sigCont, newDatacard.c_str()) );
              if(doSimultaneousFit && includeCR)
                system( sedCommand_sigCont.c_str() );

              std::string sedCommand_uncErr( Form("sed -i 's/UUU/%.3f/' %s", totUncorrErr, newDatacard.c_str()) );
              system( sedCommand_uncErr.c_str() );

              std::string sedCommand_uncErrCR( Form("sed -i 's/VVV/%.3f/' %s", totUncorrErrCont, newDatacard.c_str()) );
              if(doSimultaneousFit && includeCR)
                system( sedCommand_uncErrCR.c_str() );

              std::string sedCommand_isrErr( Form("sed -i 's/III/%.3f/' %s", isrErr, newDatacard.c_str()) );
              std::string sedCommand_bTagHErr( Form("sed -i 's/HHH/%.3f/' %s", bTagErr_heavy, newDatacard.c_str()) );
              std::string sedCommand_bTagLErr( Form("sed -i 's/LLL/%.3f/' %s", bTagErr_light, newDatacard.c_str()) );
              std::string sedCommand_lepEffErr( Form("sed -i 's/EEE/%.3f/' %s", lepEffErr, newDatacard.c_str()) );

              std::string sedCommand_genErr( Form("sed -i 's/SSS/%.3f/' %s", sig_syst, newDatacard.c_str()) );

              if (doGenAverage) system( sedCommand_genErr.c_str() );

              if( includeSignalUnc ){
                system( sedCommand_isrErr.c_str() );
                system( sedCommand_bTagHErr.c_str() );
                system( sedCommand_bTagLErr.c_str() );

                if( addSigLepSF && (( model == "T2tt" || model == "T1tttt" || model == "T2bt" || model == "T2bW" )))
                  system( sedCommand_lepEffErr.c_str() );
              } // end do includeSignalUnc

              if( copy2SE ){
                // Copying on SE
                std::string mkdirOnSE( Form("env --unset=LD_LIBRARY_PATH gfal-mkdir -p  gsiftp://t3se01.psi.ch/pnfs/psi.ch/cms/trivcat/store/user/`whoami`/%s/datacards_%.0f_%.0f", pathSE.c_str(), mParent, mLSP) );
                std::string copyOnSE( Form("xrdcp -v %s root://t3dcachedb.psi.ch:1094///pnfs/psi.ch/cms/trivcat/store/user/`whoami`/%s/datacards_%.0f_%.0f/datacard_%s_%s_%.0f_%.0f.txt", newDatacard.c_str(), pathSE.c_str(), mParent, mLSP, binName.c_str(), sigName.c_str(), mParent, mLSP) );
                system( mkdirOnSE.c_str() );
                system( copyOnSE.c_str() );

                // Attempt copying 3 times (to maximize efficiency)
                for(int c=0; c<3; ++c){
                  checkFileSE = (int) gSystem->GetPathInfo(fullPathSE.c_str(),&id, &size, &flags, &modtime);
                  if( checkFileSE==0 && (size)==0 ){
                    std::cout << "Copy did not work. Trying again: " << c << std::endl;
                    system( rmOnSE.c_str() );
                    system( copyOnSE.c_str() );
                  }
                  else{
                    std::cout << "Copy succeded. Exiting." << std::endl;
                    system( rmCommand.c_str() );
                    break;
                  } //
                } // for copy attempts
              } // copy2SE
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


std::string gammaConvention( float yieldSR, int yieldCR, int position, const std::string& corrName, const std::string& uncorrName, float testAlpha ) {

  std::string use_uncorrName(uncorrName);
  if( uncorrName=="" )
    use_uncorrName = corrName;

  std::stringstream line;
  line << std::fixed;
  line << std::setprecision(3);

  int precision = 3;
  float syst = -1.;
  if( yieldCR==0 && yieldSR==0. ) {
    line << corrName << "  gmN " << yieldCR << "   ";
    syst = testAlpha;
  } else if( yieldCR==0 && yieldSR>0. ) {
    line << use_uncorrName << "  lnN  ";
    syst = 2.;
  } else if( yieldCR>0 && yieldSR==0. ) {
    line << use_uncorrName << "  gmN 0  ";
    syst = testAlpha;
  } else {
    float alpha = yieldSR/((float)yieldCR);
    line << corrName << "  gmN " << yieldCR << "   ";
    syst = alpha;
    // precision = 10;
    precision = 5;
  }
  line << std::setprecision(precision);

  for( int i=0; i<position; ++i )
    line << " - ";

  line << syst;

  for( int i=position+1; i<4; ++i )
    line << " - ";

  std::string line_str = line.str();
  return line_str;

}
