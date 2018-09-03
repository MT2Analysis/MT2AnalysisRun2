#include <cmath>
#include <iostream>
#include <map>

#include "TCanvas.h"
#include "TAxis.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TRandom3.h"
#include "TEfficiency.h"
#include "TProfile.h"
#include "TVirtualFitter.h"
#include "TLegend.h"
#include "TLatex.h"


#include "../interface/MT2Config.h"
#include "../interface/MT2Analysis.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2EstimateSyst.h"
#include "../interface/MT2EstimateQCD.h"
#include "../interface/MT2EstimateTree.h"
#include "../interface/MT2DrawTools.h"




bool closureTest = false; // = validation region 100<MT2<200 GeV

bool macroPlots = false; // to make macro QCD estimate for Gio. Use convert2Gio.py after this

float scaleMC = 0.; // simulate stats for the given lumi [0 means MC stats]

float nonQCDunc = 0.20; // extra uncertainty on non-QCD subtraction (relative uncertainty)

bool onlyUseUpToRunG = true; // only use up to run G for rphi, fj, rb
//float lumiRatioGtoH = 27.70/36.46;
float lumiRatioGtoH = 27.261/35.876;
//float lumiRatioGtoH = 27.958/36.815;

//float lumiRatioGtoH = 27.70/36.46;
int lastRunG = 280385;

bool purityFromMC = true; // as discussed with Frank
//This is why you do MC first (also other reasons) to get the MC purity

//float prescales[3] = {7000., 180.0, 60.0}; // 2015
float prescales[3] = {7900., 440.6, 110.2}; // up to run G 27.7 fb-1
//float prescales[3] = {9200., 460.6, 115.2}; // full 2016

void projectFromInclusive( MT2Analysis<MT2Estimate>* analysis, MT2Analysis<MT2EstimateTree>* ana_inclusive, const std::string& selection );
void fillFromTreeAndRatio( MT2Estimate* estimate, MT2Estimate* nCR, MT2EstimateSyst* r_effective, TTree* tree, TF1* f1_ratio, TH1D* h_band, float prescale=1. );
void fillFromTreeAndRatio( MT2Estimate* estimate, MT2Estimate* nCR, MT2EstimateSyst* r_effective, TTree* tree, TF1* f1_ratio, TH1D* h_band, TF1* f1_ratio_up, TH1D* h_band_up, TF1* f1_ratio_down, TH1D* h_band_down , float prescale=1. );
void get_rHat( const MT2Config& cfg, MT2Analysis<MT2Estimate>* rHat, MT2Analysis<MT2EstimateTree>* analysis, MT2Analysis<MT2EstimateTree>* ana_rest=NULL );
void addSystAndNormalize4rHat(MT2Analysis<MT2Estimate>* rHat);
void get_fJets( const MT2Config& cfg, MT2Analysis<MT2Estimate>* fJets, MT2Analysis<MT2EstimateTree>* analysis, MT2Analysis<MT2EstimateTree>* ana_rest=NULL );
void drawSingleFit( const MT2Config& cfg, bool useMC, const std::string& outdir, MT2EstimateQCD* qcd, MT2EstimateQCD* all, TF1* thisFitQCD, TH1D* h_band, float xMin_fit, float xMax_fit );
void computePurity( TH1D* purity, TH1D* nonQCD, TH1D* all , float lumi);
void multiplyHisto( TH1D* histo, TH1D* other );
void scaleHisto( TH1D* histo, float val, float err );
void drawClosure( const std::string& outputdir, MT2Analysis<MT2Estimate>* estimate, MT2Analysis<MT2Estimate>* mcTruth );
void getRelError( MT2EstimateSyst* reff, MT2Estimate* syst, const std::string& outputdir="");
void addErrInQuadrature( TH1D* histo, TH1D* relErr );



int main( int argc, char* argv[] ) {


  std::cout << std::endl << std::endl;
  std::cout << "----------------------------------------------------" << std::endl;
  std::cout << "|                                                  |" << std::endl;
  std::cout << "|                                                  |" << std::endl;
  std::cout << "|          Running computeQCDFromDeltaPhi          |" << std::endl;
  std::cout << "|                                                  |" << std::endl;
  std::cout << "|                                                  |" << std::endl;
  std::cout << "----------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc<2 ) {
    std::cout << "USAGE: ./fitDeltaPhiQCD [configFileName] [data/MC] [closureTest=false]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  MT2DrawTools::setStyle();


  std::string configFileName(argv[1]);
  MT2Config cfg(configFileName);


  bool useMC = true;

  if( argc>2 ) {

    std::string mc_or_data = std::string(argv[2]); 
    if( mc_or_data=="mc" ) mc_or_data="MC";
    if( mc_or_data=="MC" ) useMC = true;
    else useMC=false;

  } 

  if( argc>3 ) {
    std::string closurestr(argv[3]);
    if( closurestr=="closureTest" || closurestr=="true" ) {
      closureTest=true;
      std::cout << "-> Running closure test in validation region" << std::endl;
    }
    else
      closureTest=false;
  }

  TH1D::AddDirectory(kTRUE);



  std::string qcdCRdir = cfg.getEventYieldDir() + "/qcdControlRegion/";
  std::string outputdir = qcdCRdir;
  if( closureTest ) {
    outputdir = outputdir + "/test/";
  }
  else if( macroPlots ) {
    outputdir = outputdir + "/macro/";
  }  
  
  //if (nonQCDunc != 0)
  //  outputdir = outputdir + Form("/nonQCDunc%d/", int(nonQCDunc*100));

  std::string fitsDir = outputdir;
  if( useMC ) fitsDir = fitsDir + "/fitsMC";
  else        fitsDir = fitsDir + "/fitsData";
  system( Form("mkdir -p %s", fitsDir.c_str() ));

  std::string runRange = "";
  if (onlyUseUpToRunG)
    runRange = Form("&&run<=%d",lastRunG);

  MT2Analysis<MT2EstimateTree>* qcdTree_mc   = MT2Analysis<MT2EstimateTree>::readFromFile( qcdCRdir + "/mc.root",   "qcdCRtree" );
  MT2Analysis<MT2EstimateTree>* qcdTree_data = MT2Analysis<MT2EstimateTree>::readFromFile( qcdCRdir + "/data.root", "qcdCRtree" );
  

  std::string regionsSet = cfg.qcdRegionsSet();
  std::string regionsSet_fits = "zurich2016_onlyHT"; //inclusive in nj & nb
  std::string regionsSet_fJets = "zurich2016_onlyHT"; //inclusive in nb
  std::string regionsSet_rHat  = "zurich2016_onlyJets_noB";//inclusive in HT //assume invariance in HT for rB -> calculate from unprescale HT trigger region (i.e. HT>1000)

  std::cout << "-> Making MT2EstimateTrees from inclusive tree...";
  MT2Analysis<MT2EstimateTree>* data_4rHat ;
  MT2Analysis<MT2EstimateTree>* data_4fJets;
  MT2Analysis<MT2EstimateTree>* data_noPS_4fJets;
  MT2Analysis<MT2EstimateTree>* qcdmc_4rHat ;
  MT2Analysis<MT2EstimateTree>* qcdmc_4fJets;
  MT2Analysis<MT2EstimateTree>* rest_4rHat ;
  MT2Analysis<MT2EstimateTree>* rest_4fJets;
  //spike removal with id cuts//
  qcdmc_4rHat  = MT2EstimateTree::makeAnalysisFromInclusiveTree( "qcdmc_4rHat"   , regionsSet_rHat  , qcdTree_mc, "((id>150  && id<200)&& ht>1000 && mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3)" ); // invert deltaPhi (i.e. low deltaphi = not the signal region)
  //original: qcdmc_4rHat  = MT2EstimateTree::makeAnalysisFromInclusiveTree( "qcdmc_4rHat"   , regionsSet_rHat  , qcdTree_mc, "((id>150  && id<200) && mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3)" ); // invert deltaPhi (i.e. low deltaphi = not the signal region)
  ////  qcdmc_4rHat  = MT2EstimateTree::makeAnalysisFromInclusiveTree( "qcdmc_4rHat"   , regionsSet_rHat  , qcdTree_mc, "((id>150&&(id>151||ht<450)&&(id>152||ht<575)&&(id>153||ht<1000)&&(id>154||ht<1500)) && id<200 && mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3)" ); // invert deltaPhi (i.e. low deltaphi = not the signal region)
  ////qcdmc_4rHat  = MT2EstimateTree::makeAnalysisFromInclusiveTree( "qcdmc_4rHat"   , regionsSet_rHat  , qcdTree_mc, "((id>150&&(id>152||ht<450)&&(id>153||ht<575)&&(id>154||ht<1000)&&(id>155||ht<1500)) && id<200 && mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3)" ); // invert deltaPhi (i.e. low deltaphi = not the signal region)
  qcdmc_4fJets = MT2EstimateTree::makeAnalysisFromInclusiveTree( "qcdmc_4fJets"  , regionsSet_fJets , qcdTree_mc, "((id>150 && id<200 && mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3))" ); // invert deltaPhi ; id=152 plays a role for VLHT in 100<mt2<200 
 
  //original: qcdmc_4fJets = MT2EstimateTree::makeAnalysisFromInclusiveTree( "qcdmc_4fJets"  , regionsSet_fJets , qcdTree_mc, "((id>150 && id<200 && mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3))" ); // invert deltaPhi ; id=152 plays a role for VLHT in 100<mt2<200 
  ////  qcdmc_4fJets = MT2EstimateTree::makeAnalysisFromInclusiveTree( "qcdmc_4fJets"  , regionsSet_fJets , qcdTree_mc, "((id>150&&(id>151||ht<450)&&(id>152||ht<575)&&(id>153||ht<1000)&&(id>154||ht<1500)) && id<200 && mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3)" ); // invert deltaPhi ; id=152 plays a role for VLHT in 100<mt2<200
  //// qcdmc_4fJets = MT2EstimateTree::makeAnalysisFromInclusiveTree( "qcdmc_4fJets"  , regionsSet_fJets , qcdTree_mc, "((id>150&&(id>152||ht<450)&&(id>153||ht<575)&&(id>154||ht<1000)&&(id>155||ht<1500)) && id<200 && mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3)" ); // invert deltaPhi ; id=152 plays a role for VLHT in 100<mt2<200

  if ( !useMC ) {//= DATA!!!
    // with PFHT900 trigger only (HT only triggered)
    data_4rHat       = MT2EstimateTree::makeAnalysisFromInclusiveTree( "data_4rHat"      , regionsSet_rHat  , qcdTree_data, "((id&1)==1 && ht>1000. &&  mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3"+runRange+")" ); // invert deltaPhi; ht>1000 unprescaled triggers
    data_4fJets      = MT2EstimateTree::makeAnalysisFromInclusiveTree( "data_4fJets"     , regionsSet_fJets , qcdTree_data, "((id&1)==1 &&              mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3"+runRange+")" ); // invert deltaPhi; HT-only triggers, ps'ed for HT<1000, empty for HT<450
    // without prescaled guys //update to new id stuff ... // it's wrong atm
    data_noPS_4fJets = MT2EstimateTree::makeAnalysisFromInclusiveTree( "data_noPS_4fJets", regionsSet_fJets , qcdTree_data, "(((id==1&&ht>100)||(id==2&&ht>450&&ht<1000)||(id==3&&ht<450))&& mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3"+runRange+")" ); // invert deltaPhi; unprescaled triggers, HT-only for ht>100, HTMHT for450< ht<1000, MET for ht<450 (below ht<1000 we live in turnon, better not to subtract an unknown amount of non-QCD [smaller than 18% for VLHT])
    //NON-QCD MC to be subtracted
    rest_4rHat       = MT2EstimateTree::makeAnalysisFromInclusiveTree( "rest_4rHat"      , regionsSet_rHat  , qcdTree_mc  , "(id>=300 && ht>1000. &&  mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3)" ); // invert deltaPhi
    rest_4fJets      = MT2EstimateTree::makeAnalysisFromInclusiveTree( "rest_4fJets"     , regionsSet_fJets , qcdTree_mc  , "(id>=300 &&              mt2>100. && mt2<200. && nJets>1 && deltaPhiMin<0.3)" ); // invert deltaPhi 
  }

  std::cout << " Done." << std::endl;

  // take r_eff from MC where fit variation results are stored
  std::string mcFile   = outputdir + "/qcdEstimateMC.root";
  MT2Analysis<MT2EstimateSyst>* reffMC;
  if ( !useMC ){
    reffMC = MT2Analysis<MT2EstimateSyst>::readFromFile( mcFile  , "r_effective" );
    reffMC->setName("r_effectiveMC");
  }

  std::cout << "-> Creating the MT2Estimates...";
  MT2Analysis<MT2Estimate>* estimate         = new MT2Analysis<MT2Estimate>    ("qcdEstimate"     , regionsSet);
  MT2Analysis<MT2Estimate>* nCR              = new MT2Analysis<MT2Estimate>    ("nCR"             , regionsSet);
  MT2Analysis<MT2EstimateSyst>* r_effective  = new MT2Analysis<MT2EstimateSyst>("r_effective"     , regionsSet);

  MT2Analysis<MT2Estimate>* est_mcRest       = new MT2Analysis<MT2Estimate>    ("est_mcRest"      , regionsSet);
  MT2Analysis<MT2Estimate>* nCR_mcRest       = new MT2Analysis<MT2Estimate>    ("nCR_mcRest"      , regionsSet);
  MT2Analysis<MT2EstimateSyst>* r_eff_mcRest = new MT2Analysis<MT2EstimateSyst>("r_eff_mcRest"    , regionsSet);

  MT2Analysis<MT2Estimate>* r_systFitVar     = new MT2Analysis<MT2Estimate>("r_systFit"           , regionsSet);

  MT2Analysis<MT2Estimate>* qcdPurity        = new MT2Analysis<MT2Estimate>("qcdPurity"       , regionsSet);
  MT2Analysis<MT2Estimate>* qcdPurityMC;
  if ( !useMC ){
    qcdPurityMC = MT2Analysis<MT2Estimate>::readFromFile( mcFile  , "qcdPurity" );
  }

  MT2Analysis<MT2Estimate>* r_hat_mc         = new MT2Analysis<MT2Estimate>("r_hat_mc"        , regionsSet_rHat);
  MT2Analysis<MT2Estimate>* f_jets_mc        = new MT2Analysis<MT2Estimate>("f_jets_mc"       , regionsSet_fJets);
  MT2Analysis<MT2Estimate>* r_hat_data       = new MT2Analysis<MT2Estimate>("r_hat_data"      , regionsSet_rHat);
  MT2Analysis<MT2Estimate>* f_jets_data      = new MT2Analysis<MT2Estimate>("f_jets_data"     , regionsSet_fJets);
  MT2Analysis<MT2Estimate>* f_jets_data_noPS = new MT2Analysis<MT2Estimate>("f_jets_data_noPS", regionsSet_fJets);
  std::cout << " Done." << std::endl;


  if( closureTest ) {
    //only one bin to account for correlation of uncerts //with more bins you underestimate the variation
    //if you wanna see some nice shapes, increase the number of bins~
    MT2Estimate    ::rebinYields( estimate    , 1, 100., 200. );
    std::cout << "rebinned first one" << std::endl;
    MT2Estimate    ::rebinYields( nCR         , 1, 100., 200. );
    MT2EstimateSyst::rebinYields( r_effective , 1, 100., 200. );

    MT2Estimate    ::rebinYields( est_mcRest  , 1, 100., 200. );
    MT2Estimate    ::rebinYields( nCR_mcRest  , 1, 100., 200. );
    MT2EstimateSyst::rebinYields( r_eff_mcRest, 1, 100., 200. );

    MT2Estimate::rebinYields    ( r_systFitVar, 1, 100., 200. );

    MT2Estimate::rebinYields    ( qcdPurity   , 1, 100., 200. );

    std::set<MT2Region> theseRegions = estimate->getRegions();
    for( std::set<MT2Region>::iterator iR = theseRegions.begin(); iR != theseRegions.end(); ++iR ) {

      std::cout << std::endl << "Bins estimate: " << estimate->get(*iR)->yield->GetNbinsX() << " " << estimate->get(*iR)->yield->GetBinLowEdge(1) << " " << estimate->get(*iR)->yield->GetBinCenter(1) << std::endl;
      std::cout << std::endl << "Bins nCR: " << nCR->get(*iR)->yield->GetNbinsX() << " " << nCR->get(*iR)->yield->GetBinLowEdge(1) << " " << nCR->get(*iR)->yield->GetBinCenter(1) << std::endl;
    }

  }
  else if( macroPlots ) {
    //update CHANGE
    const int nBins_tmp = 6;
    double *bins = new double[nBins_tmp+1]{200., 300., 400., 600., 800., 1000., 1500.};
    int nBins = nBins_tmp;
    
    MT2Estimate    ::rebinYields( estimate    , nBins, bins );
    MT2Estimate    ::rebinYields( nCR         , nBins, bins );
    MT2EstimateSyst::rebinYields( r_effective , nBins, bins );
    MT2Estimate    ::rebinYields( est_mcRest  , nBins, bins );
    MT2Estimate::rebinYields    ( nCR_mcRest  , nBins, bins );
    MT2EstimateSyst::rebinYields( r_eff_mcRest, nBins, bins );
    MT2Estimate::rebinYields    ( r_systFitVar, nBins, bins );
    MT2Estimate::rebinYields    ( qcdPurity   , nBins, bins );

  }

  std::cout << "-> Getting fJets...";
  get_fJets( cfg, f_jets_mc, qcdmc_4fJets ); //MC case: to be filled with QCD MC
  if ( !useMC ) {
    get_fJets( cfg, f_jets_data     , data_4fJets     , rest_4fJets );  //cfg, to be filled, data, nonQCDMC  
    get_fJets( cfg, f_jets_data_noPS, data_noPS_4fJets              ); // don't subtract an unknown amount (trigger turnon) small in any case
  }
  std::cout << " Done." << std::endl;
  std::cout << "-> Getting rHat...";
  get_rHat ( cfg, r_hat_mc, qcdmc_4rHat );
  if ( !useMC ) 
    get_rHat ( cfg, r_hat_data, data_4rHat, rest_4rHat );
  std::cout << " Done." << std::endl;



  std::cout << "-> Making MT2EstimateQCD from inclusive tree...";
  MT2Analysis<MT2EstimateQCD>* est_all;

  // don't apply met>250 for validation region 100<MT2<200 from ht-only prescaled triggers
  std::string metcut = closureTest ? "" : "&& (met>250||ht>1000)";

  // makeAnalysisFromInclusiveTree: 
  //first string: for NCR: = low deltaphi -> fills the tree
  //second string: is for rphi ratio-> ht triggers ONLY -> fills 2 histos

  if( useMC ) {
    // the met>250 for lower HT, to be applied in the estimate selection, not for the fits. Safe for non-QCD below. For data it is directly applied in qcdControlRegion as part of the id.
    //// est_all  = MT2EstimateQCD::makeAnalysisFromInclusiveTree( "est", regionsSet_fits, qcdTree_mc , "1"+metcut, "" ); // includes nonQCD
    //// est_all  = MT2EstimateQCD::makeAnalysisFromInclusiveTree( "est", regionsSet_fits, qcdTree_mc , "(id>150&&(id>151||ht<450)&&(id>152||ht<575)&&(id>153||ht<1000)&&(id>154||ht<1500))"+metcut, "(id>150&&(id>151||ht<450)&&(id>152||ht<575)&&(id>153||ht<1000)&&(id>154||ht<1500))" ); // includes nonQCD
    est_all  = MT2EstimateQCD::makeAnalysisFromInclusiveTree( "est", regionsSet_fits, qcdTree_mc , "(id>150&&(id>152||ht<450)&&(id>153||ht<575)&&(id>154||ht<1000)&&(id>155||ht<1500))"+metcut, "(id>150&&(id>152||ht<450)&&(id>153||ht<575)&&(id>154||ht<1000)&&(id>155||ht<1500))" ); // includes nonQCD
  } else {
    if ( closureTest ) // fill tree with ht-only triggers
      est_all  = MT2EstimateQCD::makeAnalysisFromInclusiveTree( "est", regionsSet_fits, qcdTree_data, "((id&1)==1"+runRange+")", "((id&1)==1"+runRange+")" ); //use HT-only triggers for dphi-ratio --- new way
    else // fill tree with signal triggers
      est_all  = MT2EstimateQCD::makeAnalysisFromInclusiveTree( "est", regionsSet_fits, qcdTree_data, "((id&2)==2)", "((id&1)==1"+runRange+")" ); //use HT-only triggers for dphi-ratio --- new way, bit 2 on for signal triggers, bit 1 on for ht-only triggers
  }

  //apply the met cut for control region (by default done for data id&2==2)
  //// MT2Analysis<MT2EstimateQCD>* mc_rest = MT2EstimateQCD::makeAnalysisFromInclusiveTree( "mc_rest", regionsSet_fits, qcdTree_mc  , "(id>=300 || id<=0) "+metcut, "(id>=300 || id<=0)" );
  MT2Analysis<MT2EstimateQCD>* mc_rest = MT2EstimateQCD::makeAnalysisFromInclusiveTree( "mc_rest", regionsSet_fits, qcdTree_mc  , "id>=300"+metcut, "id>=300" );


  MT2Analysis<MT2EstimateQCD>* est_minus_nonQCD = new MT2Analysis<MT2EstimateQCD>("est_minus_nonQCD"  , regionsSet_fits );
  std::cout << " Done." << std::endl;


  // fits need to be done only once
  std::set<MT2Region> QCDregions = est_all->getRegions();
  std::map<MT2Region, TF1* > fits , fits_up , fits_down ;
  std::map<MT2Region, TH1D*> bands, bands_up, bands_down;

  for( std::set<MT2Region>::iterator iR = QCDregions.begin(); iR!=QCDregions.end(); ++iR ) {

    MT2EstimateQCD* matchedEstimate      = est_all          ->getWithMatch( *iR );
    MT2EstimateQCD* matchedEstimate_rest = mc_rest          ->getWithMatch( *iR );
    MT2EstimateQCD* matchedEstimate_mnQ  = est_minus_nonQCD ->getWithMatch( *iR );

    float lumiScale = 1.;

    // these are numbers from FG, use them till we can calculate from ourselves
    // basically they are am allMC normalization to data in the high dPhi region
    float sfFromSNT[5] = {1.88375, 1.38677, 1.27216, 1.16806, 1.02239};

    if( !useMC || scaleMC!=0. ) {
      lumiScale = useMC ? scaleMC : cfg.lumi();
      if     ( iR->htMin() < 300. ) lumiScale *= sfFromSNT[0]/prescales[0];
      else if( iR->htMin() < 500. ) lumiScale *= sfFromSNT[1]/prescales[1];
      else if( iR->htMin() < 600. ) lumiScale *= sfFromSNT[2]/prescales[2];
      else if( iR->htMin() < 1100.) lumiScale *= sfFromSNT[3];
      else                          lumiScale *= sfFromSNT[4];
      if (onlyUseUpToRunG)
	lumiScale *= lumiRatioGtoH;
    }

    // add extra uncertainty on nonQCD subtraction
    if ( !useMC ){
      for( int iBin=1; iBin<matchedEstimate_rest->hDphi->GetXaxis()->GetNbins()+1; ++iBin ){
	float val = matchedEstimate_rest->hDphi->GetBinContent(iBin);
	float err = matchedEstimate_rest->hDphi->GetBinError(iBin);
	err = sqrt(err*err + nonQCDunc*val*nonQCDunc*val);
	matchedEstimate_rest->hDphi->SetBinError(iBin,err);
	val = matchedEstimate_rest->lDphi->GetBinContent(iBin);
	err = matchedEstimate_rest->lDphi->GetBinError(iBin);
	err = sqrt(err*err + nonQCDunc*val*nonQCDunc*val);
	matchedEstimate_rest->lDphi->SetBinError(iBin,err);
      }
    }
    
    //Subtract non qcd
    matchedEstimate_mnQ->lDphi = (TH1D*) matchedEstimate->lDphi->Clone(matchedEstimate_mnQ->lDphi->GetName());
    if (useMC && scaleMC!=0.)  matchedEstimate_mnQ->lDphi->Scale(lumiScale);
    matchedEstimate_mnQ->lDphi->Add( matchedEstimate_rest->lDphi, -lumiScale );

    matchedEstimate_mnQ->hDphi = (TH1D*) matchedEstimate->hDphi->Clone(matchedEstimate_mnQ->hDphi->GetName());
    if (useMC && scaleMC!=0.)  matchedEstimate_mnQ->hDphi->Scale(lumiScale);
    matchedEstimate_mnQ->hDphi->Add( matchedEstimate_rest->hDphi, -lumiScale );

    if (useMC && scaleMC!=0.)  matchedEstimate_mnQ->sqrtErrors();

    // get rid of negative yields after subtraction
    for( int iBin=1; iBin<matchedEstimate_mnQ->hDphi->GetXaxis()->GetNbins()+1; ++iBin )
      if ( matchedEstimate_mnQ->hDphi->GetBinContent(iBin)<0 )
	matchedEstimate_mnQ->hDphi->SetBinContent(iBin,0);

    float xMin_fit = (iR->htMin()>=1000.) ? 70. : 60.;
    float xMax_fit = 100.;
    //fits.insert(std::pair<MT2Region, TF1*>(*iR, matchedEstimate_mnQ->getFit( "pow", xMin_fit, xMax_fit ) ) );
    //GetFit does ratio and fit
    fits[*iR] = matchedEstimate_mnQ->getFit( "pow", xMin_fit, xMax_fit );
    bands[*iR] = new TH1D(Form("band_%s",iR->getName().c_str()), "", 500, matchedEstimate->lDphi->GetXaxis()->GetXmin(), matchedEstimate->lDphi->GetXaxis()->GetXmax());
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(bands[*iR], 0.68);

    std::cout << iR->htRegion()->getNiceName() << std::endl
              << "par0 : " << fits[*iR]->GetParameter(0) << " +/- " << fits[*iR]->GetParError(0) << std::endl
	      << "par1 : " << fits[*iR]->GetParameter(1) << " +/- " << fits[*iR]->GetParError(1) << std::endl;

    drawSingleFit( cfg, useMC, fitsDir, matchedEstimate_mnQ, matchedEstimate, fits[*iR], bands[*iR], xMin_fit, xMax_fit );

    // initial values for variation fits from central fit, to facilitate convergence
    float par0 = fits[*iR]->GetParameter(0);
    float par1 = fits[*iR]->GetParameter(1);

    //UP variation 
    float xMin_fit_var = xMin_fit + 5.;
    float xMax_fit_var = xMax_fit + 25.;
    fits_up [*iR] = matchedEstimate_mnQ->getFit( "pow", xMin_fit_var, xMax_fit_var, par0, par1 );
    bands_up[*iR] = new TH1D(Form("band_up_%s",iR->getName().c_str()), "", 500, matchedEstimate->lDphi->GetXaxis()->GetXmin(), matchedEstimate->lDphi->GetXaxis()->GetXmax());
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(bands_up[*iR], 0.68);
    drawSingleFit( cfg, useMC, fitsDir+"/up/", matchedEstimate_mnQ, matchedEstimate, fits_up[*iR], bands_up[*iR], xMin_fit_var, xMax_fit_var );

    std::cout << iR->htRegion()->getNiceName() << " - right variation" << std::endl
              << "par0 : " << fits_up[*iR]->GetParameter(0) << " +/- " << fits_up[*iR]->GetParError(0) << std::endl
	      << "par1 : " << fits_up[*iR]->GetParameter(1) << " +/- " << fits_up[*iR]->GetParError(1) << std::endl;

    xMin_fit_var = xMin_fit - 5.;
    xMax_fit_var = xMax_fit;
    fits_down [*iR] = matchedEstimate_mnQ->getFit( "pow", xMin_fit_var, xMax_fit_var, par0, par1 );
    bands_down[*iR] = new TH1D(Form("band_down_%s",iR->getName().c_str()), "", 500, matchedEstimate->lDphi->GetXaxis()->GetXmin(), matchedEstimate->lDphi->GetXaxis()->GetXmax());
    (TVirtualFitter::GetFitter())->GetConfidenceIntervals(bands_down[*iR], 0.68);
    drawSingleFit( cfg, useMC, fitsDir+"/down/", matchedEstimate_mnQ, matchedEstimate, fits_down[*iR], bands_down[*iR], xMin_fit_var, xMax_fit_var );

    std::cout << iR->htRegion()->getNiceName() << " - left variation"  << std::endl
              << "par0 : " << fits_down[*iR]->GetParameter(0) << " +/- " << fits_down[*iR]->GetParError(0) << std::endl
	      << "par1 : " << fits_down[*iR]->GetParameter(1) << " +/- " << fits_down[*iR]->GetParError(1) << std::endl;

    float varR = fits_up[*iR]  ->GetParameter(1)/fits[*iR]->GetParameter(1);
    float varL = fits_down[*iR]->GetParameter(1)/fits[*iR]->GetParameter(1);
    float varMax = TMath::Max( fabs(varR-1.0), fabs(varL-1.0) );

    std::cout << "=> slope variation: " << varR << " (right) / " << varL << "(left)" << std::endl
	      << "   maximal variation: " << varMax << std::endl;

  } // end loop qcd regions


  std::set<MT2Region> regions = estimate->getRegions();

  //std::string lastRegionName = "";

  for( std::set<MT2Region>::iterator iR = regions.begin(); iR!=regions.end(); ++iR ) {

    //std::cout << "region ht " << iR->htMin() << " to " << iR->htMax() << ", nj " << iR->nJetsMin() << " to " << iR->nJetsMax() << ", nb " << iR->nBJetsMin() << " to " << iR->nBJetsMax() << std::endl;

    MT2Estimate     *this_estimate    = estimate    ->get( *iR );
    MT2Estimate     *this_nCR         = nCR         ->get( *iR );
    MT2EstimateSyst *this_r_effective = r_effective ->get( *iR );

    MT2Estimate     *this_est_mcRest   = est_mcRest  ->get( *iR );
    MT2Estimate     *this_nCR_mcRest   = nCR_mcRest  ->get( *iR );
    MT2EstimateSyst *this_r_eff_mcRest = r_eff_mcRest->get( *iR );

    MT2Estimate* this_qcdPurity   = qcdPurity   ->get( *iR );

    MT2Estimate* this_r_hat ;
    MT2Estimate *this_f_jets;

    MT2Region* rhat_j2to3 = new MT2Region(250, -1, 2, 3, 0, -1);
    MT2Region* rhat_j2to6 = new MT2Region(250, -1, 2, 6, 0, -1);
    MT2Region* rhat_j4to6 = new MT2Region(250, -1, 4, 6, 0, -1);
    MT2Region* rhat_j2toInf = new MT2Region(250, -1, 2, -1, 0, -1);
    MT2Region* rhat_j4toInf = new MT2Region(250, -1, 4, -1, 0, -1);
    MT2Region* rhat_j7toInf = new MT2Region(250, -1, 7, -1, 0, -1);

    if ( useMC ) {

      //      this_r_hat  = r_hat_mc ->getWithMatch( *iR );

      if(iR->nJetsMin()==2 && iR->nJetsMax()==3)
	this_r_hat = r_hat_mc->get( *rhat_j2to3 );
      else if(iR->nJetsMin()==2 && iR->nJetsMax()==6)
	this_r_hat = r_hat_mc->get( *rhat_j2to6 );
      else if(iR->nJetsMin()==4 && iR->nJetsMax()==6)
	this_r_hat = r_hat_mc->get( *rhat_j4to6 );
      else if(iR->nJetsMin()==2 && iR->nJetsMax()==-1)
	this_r_hat = r_hat_mc->get( *rhat_j2toInf );
      else if(iR->nJetsMin()==4 && iR->nJetsMax()==-1)
	this_r_hat = r_hat_mc->get( *rhat_j4toInf );
      else if(iR->nJetsMin()==7 && iR->nJetsMax()==-1)
	this_r_hat = r_hat_mc->get( *rhat_j7toInf );
      
      this_f_jets = f_jets_mc->getWithMatch( *iR );

    }
    // else if ( iR->htMin() < 300. ) { // if we are in VLHT take the values for fjet from the monojet trigger w/o bkg subtraction
    //   this_r_hat  = r_hat_data      ->getWithMatch( *regionToMatch );
    //   //this_f_jets = f_jets_data_noPS->getWithMatch( *regionToMatch );      
    //   this_f_jets = f_jets_data->getWithMatch( *regionToMatch );  // ht-only trigger also for vlht in 2016 data
    // }
    else { // otherwise take values from HT-only triggers w/ proper bkg subtractions

      //      this_r_hat  = r_hat_data ->getWithMatch( *iR );

      if(iR->nJetsMin()==2 && iR->nJetsMax()==3)
	this_r_hat = r_hat_data->get( *rhat_j2to3 );
      else if(iR->nJetsMin()==2 && iR->nJetsMax()==6)
	this_r_hat = r_hat_data->get( *rhat_j2to6 );
      else if(iR->nJetsMin()==4 && iR->nJetsMax()==6)
	this_r_hat = r_hat_data->get( *rhat_j4to6 );
      else if(iR->nJetsMin()==2 && iR->nJetsMax()==-1)
	this_r_hat = r_hat_data->get( *rhat_j2toInf );
      else if(iR->nJetsMin()==4 && iR->nJetsMax()==-1)
	this_r_hat = r_hat_data->get( *rhat_j4toInf );
      else if(iR->nJetsMin()==7 && iR->nJetsMax()==-1)
	this_r_hat = r_hat_data->get( *rhat_j7toInf );

      this_f_jets = f_jets_data->getWithMatch( *iR );

    }

    MT2EstimateQCD* matchedEstimate      = est_all          ->getWithMatch( *iR );
    MT2EstimateQCD* matchedEstimate_rest = mc_rest          ->getWithMatch( *iR );

    MT2Region *fit_matchedRegion = est_all->matchRegion(*iR);
    //if( iR->htMin() < 300. )  // take fit from low HT also for the very low HT region
    //  fit_matchedRegion = new MT2Region( 450, 575, 2, -1, 0, -1 );

    float ps = 1.;
    if ( closureTest && !useMC ){
      if     ( iR->htMin() < 300. ) ps = prescales[0]; // prescale
      else if( iR->htMin() < 500. ) ps = prescales[1];
      else if( iR->htMin() < 600. ) ps = prescales[2];
    }
    // fill fill fill, tree(matched), fits, bands
    fillFromTreeAndRatio( this_estimate  , this_nCR       , this_r_effective , matchedEstimate->tree, fits[*fit_matchedRegion], bands[*fit_matchedRegion], fits_up[*fit_matchedRegion], bands_up[*fit_matchedRegion], fits_down[*fit_matchedRegion], bands_down[*fit_matchedRegion]     );
    fillFromTreeAndRatio( this_est_mcRest, this_nCR_mcRest, this_r_eff_mcRest, matchedEstimate_rest->tree, fits[*fit_matchedRegion], bands[*fit_matchedRegion] , ps);


    // validation from prescale triggers -> use only up to runG
    float lumi = useMC ? 1.0 : cfg.lumi();
    if (!useMC && onlyUseUpToRunG && closureTest)
      lumi *= lumiRatioGtoH;

    //P = QCD/ (QCD +nonQCD)
    //Franks way: 
    //computePurity( this_qcdPurity->yield, this_est_mcRest->yield, this_estimate->yield, lumi );//Old but gold: change to this back
    computePurity( this_qcdPurity->yield, this_nCR_mcRest->yield, this_nCR->yield, lumi ); //this computes purity directly on CR, above is more correct, this is franks way

    if (!useMC && purityFromMC){ // use purity from MC as agreed with FG
      MT2Estimate* this_qcdPurityMC = qcdPurityMC   ->get( *iR );
      multiplyHisto( this_estimate->yield, this_qcdPurityMC->yield );
    }
    else
      multiplyHisto( this_estimate->yield, this_qcdPurity->yield );
    

    // add in quadrature bin-by-bin uncertainties from fit variation found in MC
    if ( !useMC ){

      MT2Region *this_region;
      //if( iR->htMin() < 300. )  // take fit from low HT also for the very low HT region
      //    this_region = new MT2Region(450, 575, iR->nJetsMin(), iR->nJetsMax(), iR->nBJetsMin(), iR->nBJetsMax() );
      //else 
      this_region = new MT2Region( *iR );

      MT2EstimateSyst* this_reffMC     = reffMC      ->get( *this_region );
      MT2Estimate* this_systFitVar = r_systFitVar->get( *iR );
      getRelError ( this_reffMC, this_systFitVar, outputdir);
      addErrInQuadrature ( this_estimate->yield, this_systFitVar->yield);
    
    }

    int bin_bJets = this_r_hat ->yield->FindBin(iR->nBJetsMin());
    float thisRhatValue = this_r_hat->yield->GetBinContent( bin_bJets );
    float thisRhatError = this_r_hat->yield->GetBinError  ( bin_bJets );

    int bin_jets = this_f_jets->yield->FindBin(iR->nJetsMin() );
    float thisFjetsValue = this_f_jets->yield->GetBinContent( bin_jets );
    float thisFjetsError = this_f_jets->yield->GetBinError  ( bin_jets );

    if( iR->nBJetsMin()==3 && iR->nJetsMin()==2 ) {
      int bin_jets_2  = this_f_jets->yield->FindBin(4); // also valid for VL cause 4-6 and >7 are only one bin
      thisFjetsValue += this_f_jets->yield->GetBinContent( bin_jets_2 );
      thisFjetsError *= thisFjetsError;
      thisFjetsError += this_f_jets->yield->GetBinError( bin_jets_2 )*this_f_jets->yield->GetBinError( bin_jets_2 );
      thisFjetsError  = sqrt(thisFjetsError);
      // if( iR->htMin()==250 && iR->htMax()==450 ){ //CHANGE TO SYNC with GOLF
      // thisFjetsError  = 0.0;
      // thisFjetsValue = 1.;
      // }
    }

    scaleHisto( this_estimate->yield, thisRhatValue , thisRhatError  );
    scaleHisto( this_estimate->yield, thisFjetsValue, thisFjetsError );

    //delete h_band;

  }  // for regions
      
    
  std::string outfileName = outputdir;
  if( useMC ) outfileName = outfileName + "/qcdEstimateMC.root";
  else        outfileName = outfileName + "/qcdEstimateData.root";

  estimate        ->writeToFile( outfileName, "recreate" );
  nCR             ->writeToFile( outfileName );
  r_effective     ->writeToFile( outfileName );
  r_hat_mc        ->writeToFile( outfileName );
  f_jets_mc       ->writeToFile( outfileName );
  r_hat_data      ->writeToFile( outfileName );
  f_jets_data     ->writeToFile( outfileName );
  f_jets_data_noPS->writeToFile( outfileName );
  if (!useMC && purityFromMC){
    qcdPurity       ->setName("qcdPurityData");
    qcdPurity       ->writeToFile( outfileName );
    qcdPurityMC     ->writeToFile( outfileName );
  }
  else
    qcdPurity       ->writeToFile( outfileName );
  if ( !useMC ) {
    reffMC          ->writeToFile( outfileName );
    r_systFitVar    ->writeToFile( outfileName );
  }




  std::string outputFile_fits = fitsDir;
  if( useMC ) outputFile_fits = outputFile_fits + "/qcdFitsMC.root";
  else        outputFile_fits = outputFile_fits + "/qcdFitsData.root";

  est_all          ->writeToFile( outputFile_fits, "recreate" );
  if( useMC ) {
    MT2Analysis<MT2EstimateQCD>* mc_qcd  = MT2EstimateQCD::makeAnalysisFromInclusiveTree( "mc_qcd" , regionsSet_fits, qcdTree_mc  , "id>=100 && id<200" );
    mc_qcd         ->writeToFile( outputFile_fits );
  }
  mc_rest          ->writeToFile( outputFile_fits );
  est_minus_nonQCD ->writeToFile( outputFile_fits );


  return 0;

}





void projectFromInclusive( MT2Analysis<MT2Estimate>* analysis, MT2Analysis<MT2EstimateTree>* ana_inclusive, const std::string& selection ) {

  MT2EstimateTree* treeInclusive = ana_inclusive->get( MT2Region("HT450toInf_j2toInf_b0toInf") );
  if( treeInclusive==0 ) {
    std::cout << "ERROR!! You need to pass an inclusive MT2EstimateTree Analysis to use this function!" << std::endl;
    exit(19191);
  }

  std::set<MT2Region> regions = analysis->getRegions();


  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    MT2Estimate* thisEstimate= analysis->get( *iR );

    std::string fullSelection = iR->getRegionCuts();
    if( selection!="" ) fullSelection = fullSelection + " && " + selection;
    treeInclusive->tree->Project( thisEstimate->yield->GetName(), "mt2", Form("weight*(%s)", fullSelection.c_str()) );

  } // for regions


}



void get_rHat( const MT2Config& cfg, MT2Analysis<MT2Estimate>* rHat, MT2Analysis<MT2EstimateTree>* analysis, MT2Analysis<MT2EstimateTree>* ana_rest ) {

  int nBins = 4;
  Double_t bins[nBins+1];
  bins[0] = 0.;
  bins[1] = 1.;
  bins[2] = 2.;
  bins[3] = 3.;
  bins[4] = 6.;

  std::set<MT2Region> regions  = rHat ->getRegions();

  //CHANGE to use rebinning function
  // initialize yields
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {
    MT2Estimate* thisEst = rHat->get(*iR);
    std::string name(thisEst->yield->GetName());
    delete thisEst->yield;
    thisEst->yield = new TH1D( name.c_str(), "", nBins, bins );
    thisEst->yield->Sumw2();
  } 

  // fill histos for regular non-extended regions, add them to the relevant extra regions (before normalization and addition of syst. err)
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    // do computing expensive stuff only for regular regions
    if ( iR->nJetsMax()==-1 && iR->nJetsMin()<7 ) //because we do them afterwards
      continue;

    MT2Estimate* thisEst = rHat->get(*iR);
    std::string name(thisEst->yield->GetName());

    MT2EstimateTree* thisTree = analysis->get(*iR);
    thisTree->tree->Project( name.c_str(), "nBJets", "weight" );
    MT2DrawTools::addOverflowSingleHisto(thisEst->yield);

    if ( ana_rest!=NULL ) { //don't account for PS because we're in ht>1000 regions= unprescale!
      float lumi = cfg.lumi();
      if (onlyUseUpToRunG)
	lumi *= lumiRatioGtoH;
      TH1D *toSubtract = new TH1D( Form("%s_rest",name.c_str()), "", nBins, bins );
      MT2EstimateTree* anotherTree = ana_rest->get(*iR);
      anotherTree->tree->Project( Form("%s_rest",name.c_str()), "nBJets", "weight" );
      MT2DrawTools::addOverflowSingleHisto(toSubtract);

      std::cout << "rhat region: " << iR->sigRegion()->getNiceName() << std::endl
		<< "amount of non-QCD subtracted: ";
      for ( int iBin=1; iBin<=nBins; iBin++ )
	std::cout << toSubtract->GetBinContent(iBin)*lumi/thisEst->yield->GetBinContent(iBin)*100 << "\%("<<thisEst->yield->GetBinContent(iBin )<< "-" << toSubtract->GetBinContent(iBin)*lumi << ")\t";
      std::cout << std::endl;

      thisEst->yield->Add(toSubtract,-lumi);
    }

    // fill extra regions
    MT2Region* extra1 = new MT2Region( iR->htMin(), iR->htMax(), 4, -1, iR->nBJetsMin(), iR->nBJetsMax() );
    MT2Region* extra2 = new MT2Region( iR->htMin(), iR->htMax(), 2, -1, iR->nBJetsMin(), iR->nBJetsMax() );
    if ( iR->isIncluded(extra1) ) {
      MT2Estimate* extraEst = rHat->get(*extra1);
      extraEst->yield->Add(thisEst->yield);
    }  
    if ( iR->isIncluded(extra2) ) {
      MT2Estimate* extraEst = rHat->get(*extra2);
      extraEst->yield->Add(thisEst->yield);
    }  

  } // for regions
    

  // add syst. errors and normalize
  addSystAndNormalize4rHat(rHat);
    
}


void addSystAndNormalize4rHat(MT2Analysis<MT2Estimate>* rHat){

  std::vector<float> uncert;
  uncert.push_back( 0.08 ); // from bruno
  uncert.push_back( 0.20 ); 
  uncert.push_back( 0.35 ); 
  uncert.push_back( 0.70 ); 

  std::set<MT2Region> regions  = rHat ->getRegions();

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {
    
    MT2Estimate* thisEst = rHat->get(*iR);

    int nBins = thisEst->yield->GetNbinsX();
    
    for( int iBin=1; iBin<nBins+1; ++iBin ) {

      // add error in quadrature to estimate
      float val_est    = thisEst->yield->GetBinContent(iBin);
      float error_est  = thisEst->yield->GetBinError(iBin);
      float errorRel_est = error_est/val_est;

      // add protection against negative values (if oversubtraction). Does it happen?
      if (val_est<0) thisEst->yield->SetBinContent(iBin, 0);

      float errorRel_tot = sqrt( errorRel_est*errorRel_est + uncert[iBin-1]*uncert[iBin-1] );
      thisEst->yield->SetBinError( iBin, errorRel_tot*val_est );

    }

    thisEst->yield->Scale( 1./thisEst->yield->Integral(1, nBins+1) );

  }

}

void get_fJets( const MT2Config& cfg, MT2Analysis<MT2Estimate>* fJets, MT2Analysis<MT2EstimateTree>* analysis, MT2Analysis<MT2EstimateTree>* ana_rest ) {

  std::vector<float> uncert;
  uncert.push_back( 0.25 ); // from bruno
  uncert.push_back( 0.07 ); 
  uncert.push_back( 0.20 ); 

  std::set<MT2Region> regions = fJets->getRegions();

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    MT2Estimate* thisEst = fJets->get(*iR);

    std::string name(thisEst->yield->GetName());
    
    int nBins = iR->htMin()<300 ? 2 : 3;
    Double_t bins[nBins+1];
    if (iR->htMin()<300 ){
      bins[0] = 2.;
      bins[1] = 4.;
      bins[2] = 11.;
    }else{
      bins[0] = 2.;
      bins[1] = 4.;
      bins[2] = 7.;
      bins[3] = 11.;
    }
    
    delete thisEst->yield;
    thisEst->yield = new TH1D( name.c_str(), "", nBins, bins );
    thisEst->yield->Sumw2();

    MT2EstimateTree* thisTree = analysis->get(*iR);
    thisTree->tree->Project( name.c_str(), "nJets", "weight" );

    MT2DrawTools::addOverflowSingleHisto(thisEst->yield);

    
    if ( ana_rest!=NULL ) {//if it is data
      TH1D *toSubtract = new TH1D( Form("%s_rest",name.c_str()), "", nBins, bins );
      //another = nonQCD MC
      MT2EstimateTree* anotherTree = ana_rest->get(*iR);
      anotherTree->tree->Project( Form("%s_rest",name.c_str()), "nJets", "weight" );
      MT2DrawTools::addOverflowSingleHisto(toSubtract);
      
      float lumi = cfg.lumi();
      if (onlyUseUpToRunG)
	lumi *= lumiRatioGtoH;
      float ps = 1.; 
      if      (iR->htMin() < 400.) ps = prescales[0];
      else if (iR->htMin() < 500.) ps = prescales[1];
      else if (iR->htMin() < 600.) ps = prescales[2];
      
      std::cout << "f_j region: " << iR->htRegion()->getNiceName() << std::endl
		<< "amount of non-QCD subtracted: ";
      for ( int iBin=1; iBin<=nBins; iBin++ )
	std::cout << toSubtract->GetBinContent(iBin)*lumi/ps/thisEst->yield->GetBinContent(iBin)*100 << "\%("<<thisEst->yield->GetBinContent(iBin)<<"-"<<toSubtract->GetBinContent(iBin)*lumi/ps<<")\t";
      std::cout << std::endl;

      //if ( iR->htMin() > 400. )  // HT dataset not filled for VLHT, leave it empty
      thisEst->yield->Add(toSubtract,-lumi/ps);
    }

    for( int iBin=1; iBin<nBins+1; ++iBin ) {

      // add error in quadrature to estimate
      float val_est    = thisEst->yield->GetBinContent(iBin);
      float error_est  = thisEst->yield->GetBinError(iBin);
      float errorRel_est = error_est/val_est;

      // add protection against negative values (if oversubtraction), possible (?) for VLHT f_7 where expected a value ~0
      if (val_est<0) thisEst->yield->SetBinContent(iBin, 0);

      //float inflateUncert = iR->htMin() < 400. ? 2.0 : 1.0; // inflate by a factor two in the VLHT
      float inflateUncert = 1.0; // don't inflate by a factor two in the VLHT for 2016

      //add uncert = from variation test
      float errorRel_tot = sqrt( errorRel_est*errorRel_est + inflateUncert*inflateUncert*uncert[iBin-1]*uncert[iBin-1] );
      thisEst->yield->SetBinError( iBin, errorRel_tot*val_est );

    }
    
    //scale to 1 (want fractions)
    thisEst->yield->Scale( 1./thisEst->yield->Integral(1, nBins+1) );

  } // for regions

}



void fillFromTreeAndRatio( MT2Estimate* estimate, MT2Estimate* nCR, MT2EstimateSyst* r_effective, TTree* tree, TF1* f1_ratio, TH1D* h_band, float prescale) {
  fillFromTreeAndRatio( estimate, nCR, r_effective, tree, f1_ratio, h_band , f1_ratio, h_band, f1_ratio, h_band, prescale);
}

void fillFromTreeAndRatio( MT2Estimate* estimate, MT2Estimate* nCR, MT2EstimateSyst* r_effective, TTree* tree, TF1* f1_ratio, TH1D* h_band, TF1* f1_ratio_up, TH1D* h_band_up, TF1* f1_ratio_down, TH1D* h_band_down, float prescale) {


  int nBins;
  double* bins;
  estimate->getYieldBins(nBins, bins);

  TProfile* hp_r = new TProfile( "r", "", nBins, bins );
  TProfile* hp_rErr = new TProfile( "rErr", "", nBins, bins );
  TProfile* hp_rUp   = new TProfile( "rUp"  , "", nBins, bins );
  TProfile* hp_rDown = new TProfile( "rDown", "", nBins, bins );

  float weight;
  tree->SetBranchAddress( "weight", &weight );
  float mt2;
  tree->SetBranchAddress( "mt2", &mt2 );

  int nentries = tree->GetEntries();

  // this tree has only lowDeltaPhi events
  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    tree->GetEntry(iEntry);

    float r = f1_ratio->Eval( mt2 );

    float ps_weight = weight/prescale;

    nCR     ->yield->Fill( mt2, ps_weight   );
    estimate->yield->Fill( mt2, ps_weight*r );

    hp_r->Fill( mt2, r, ps_weight ); //Profile histo -> averaged over N_CR
    hp_rErr->Fill( mt2, h_band->GetBinError(h_band->FindBin(mt2)), ps_weight );

    hp_rUp  ->Fill( mt2, f1_ratio_up  ->Eval(mt2), ps_weight);
    hp_rDown->Fill( mt2, f1_ratio_down->Eval(mt2), ps_weight);
  } // for entries


  for( int iBin=1; iBin<nBins+1; ++iBin ) {

    float r          = hp_r   ->GetBinContent(iBin);
    float error_mean = hp_r   ->GetBinError  (iBin);
    float error_fit  = hp_rErr->GetBinContent(iBin);
    float r_up    = hp_rUp->GetBinContent(iBin);
    float r_down  = hp_rDown->GetBinContent(iBin);

    // if zero events in control region, take r from lower mt2 edge
    if ( nCR->yield->GetBinContent(iBin)==0 ){

      float mt2  = nCR->yield->GetXaxis()->GetBinLowEdge(iBin);
      r          = f1_ratio->Eval( mt2 );
      error_fit  = h_band->GetBinError(h_band->FindBin(mt2));
      r_up    = f1_ratio_up  ->Eval(mt2);
      r_down  = f1_ratio_down->Eval(mt2);
      error_mean = 0.0;
    }
    // fill r_effective //average r
    r_effective->yield->SetBinContent( iBin, r );

    // add fit error in quadrature to R and fill r_effective error
    float error_r    = sqrt(error_fit*error_fit + error_mean*error_mean);
    r_effective->yield->SetBinError(iBin, error_r);

    // // asymmetric error band. Envelope of the three fit bands
    r_effective->yield_systUp  ->SetBinContent(iBin, r_up );
    r_effective->yield_systDown->SetBinContent(iBin, r_down );
    
    // add R error in quadrature to estimate
    float errorRel_r = r>0 ? error_r/r : 1.0;
    float val_est    = estimate->yield->GetBinContent(iBin);
    float error_est  = estimate->yield->GetBinError(iBin);
    float errorRel_est = val_est>0 ? error_est/val_est : 1.;


    float errorRel_tot = sqrt( errorRel_est*errorRel_est + errorRel_r*errorRel_r );
    estimate->yield->SetBinError( iBin, errorRel_tot*val_est );

  }

  delete hp_r;
  delete hp_rUp;
  delete hp_rDown;
  delete hp_rErr;

}







void drawSingleFit( const MT2Config& cfg, bool useMC, const std::string& outdir, MT2EstimateQCD* qcd, MT2EstimateQCD* all, TF1* thisFitQCD, TH1D* h_band, float xMin_fit, float xMax_fit ) {

  system( Form("mkdir -p %s", outdir.c_str() ));

  TH1D* thisRatioAll = all->getRatio();
  TH1D* thisRatioQCD = qcd->getRatio();

  TCanvas* c1 = new TCanvas( "c2", "", 600, 600 );
  c1->cd();
  c1->SetLogx();
  c1->SetLogy();

  
  //float xMin = thisRatioAll->GetXaxis()->GetXmin();
  float xMin = 50;
  //float xMax = useMC ? thisRatioAll->GetXaxis()->GetXmax() : 200;  // we are blind to MT2>200 in data
  //float xMax = useMC ? thisRatioAll->GetXaxis()->GetXmax() : 450;
  //float xMax = useMC ? 800 : 450;
  float xMax = useMC ? 1500 : 450;

  float yMax    = thisRatioAll->GetMaximum()*20.;
  float yMin    = 0.01; //thisRatioQCD->GetMinimum()/2.;

  TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, yMin, yMax );
  h2_axes->SetXTitle( "M_{T2} [GeV]" );
  h2_axes->SetYTitle( "" ); //"r_{#phi}");
  h2_axes->GetXaxis()->SetNoExponent();
  h2_axes->GetXaxis()->SetMoreLogLabels();
  h2_axes->GetYaxis()->SetNoExponent();
  h2_axes->Draw();

  TLatex *ytitle = new TLatex(0.02,0.92,"r_{#phi}");
  ytitle->SetNDC();
  ytitle->SetTextFont(42);
  ytitle->SetTextSize(0.065);
  ytitle->Draw();

  std::vector<std::string> regionNiceNames = qcd->region->getNiceNames();

  TPaveText* regionName = new TPaveText( 0.4, 0.81, 0.9, 0.9, "brNDC" );
  regionName->SetTextAlign( 11 );
  regionName->SetTextSize( 0.035 );
  regionName->SetFillColor( 0 );
  regionName->AddText( regionNiceNames[0].c_str() );
  regionName->AddText( regionNiceNames[1].c_str() );
  //regionName->Draw("same");

  TLegend* legend = new TLegend( (useMC ? 0.33 : 0.4), 0.7, 0.8, 0.92, regionNiceNames[0].c_str() );
  //TLegend* legend = new TLegend( 0.4, 0.65, 0.8, 0.82 );
  legend->SetFillColor(0);
  legend->SetTextSize(0.035);
  if( useMC ) {
    legend->AddEntry( thisRatioAll, "Simulation (all)", "PL" );
    legend->AddEntry( thisRatioQCD, "Simulation (multijet only)", "PL" );
  } else {
    legend->AddEntry( thisRatioAll, "Data", "PL" );
    legend->AddEntry( thisRatioQCD, "Data after subtraction", "PL" );
  }
  TF1 *fitWithBand = (TF1*)thisFitQCD->Clone();
  fitWithBand->SetLineColor(46); 
  fitWithBand->SetLineWidth(2); 
  fitWithBand->SetFillColor(17); 
  fitWithBand->SetFillStyle(3001);
  legend->AddEntry( fitWithBand, "Fit", "FL" );
  legend->Draw("same");

  TLine* lineLeft = new TLine( xMin_fit, yMin, xMin_fit, yMax );
  lineLeft->SetLineStyle(2);
  lineLeft->Draw("same");

  TLine* lineRight = new TLine( xMax_fit, yMin, xMax_fit, yMax );
  lineRight->SetLineStyle(2);
  lineRight->Draw("same");


  h_band->SetMarkerSize(0);
  h_band->SetFillColor(17); 
  h_band->SetFillStyle(3001);
  h_band->Draw("C E3 same");

  thisFitQCD->SetLineColor(46); 
  thisFitQCD->SetLineWidth(2); 
  thisFitQCD->Draw("L same");

  thisRatioAll->SetMarkerStyle(20);
  thisRatioAll->SetMarkerSize(1.3);
  thisRatioAll->SetLineColor(kBlack);
  thisRatioAll->Draw("P same");

  thisRatioQCD->SetMarkerStyle(24);
  thisRatioQCD->SetMarkerSize(1.3);
  thisRatioQCD->SetLineColor(kBlack);
  thisRatioQCD->Draw("P same");

  gPad->RedrawAxis();

  float ps = 1.; 
  if      (qcd->region->htMin() < 400.) ps = prescales[0];
  else if (qcd->region->htMin() < 500.) ps = prescales[1];
  else if (qcd->region->htMin() < 600.) ps = prescales[2];

  TPaveText* labelTop;
  //if( useMC && scaleMC==0.) labelTop = MT2DrawTools::getLabelTop("(13 TeV)"); // non-preliminary
  if( useMC && scaleMC==0.) labelTop = MT2DrawTools::getLabelTopSimulation();
  else if( useMC )          labelTop = MT2DrawTools::getLabelTopSimulation(scaleMC);
  else        labelTop = MT2DrawTools::getLabelTop(cfg.lumi()*(onlyUseUpToRunG ? lumiRatioGtoH : 1.0)/ps);
  labelTop->Draw("same");

  TPaveText *chi2;
  chi2 = new TPaveText(0.22,0.23, 0.5,0.18,"brNDC");
  chi2->SetFillColor(10);   chi2->SetBorderSize(0);
  chi2->AddText(Form("#chi^{2}/ndf = %.1f/%d: %.1f%%", thisFitQCD->GetChisquare(), thisFitQCD->GetNDF(), thisFitQCD->GetProb()*100));
  chi2->Draw();

  TPaveText* labelCMS;

  if (useMC)
    labelCMS = MT2DrawTools::getLabelCMS("CMS Simulation");
    //labelCMS = MT2DrawTools::getLabelCMS("CMS Supplementary (Simulation)");
  else
    labelCMS = MT2DrawTools::getLabelCMS("CMS Preliminary");
    //labelCMS = MT2DrawTools::getLabelCMS();
    //labelCMS = MT2DrawTools::getLabelCMS("CMS Unpublished");
    //labelCMS = MT2DrawTools::getLabelCMS("CMS Supplementary");
  labelCMS->Draw("same");

  // TPaveText *arxiv;
  // if (useMC)
  //   arxiv = new TPaveText(0.22,0.23, 0.5,0.18,"brNDC");
  // else
  //   arxiv = new TPaveText(0.28,0.23, 0.55,0.18,"brNDC");
  // arxiv->SetFillColor(10);   arxiv->SetBorderSize(0);
  // arxiv->SetTextSize(0.035); arxiv->SetTextFont(42);
  // arxiv->AddText("arXiv:1603.04053");
  // arxiv->Draw();

  //  // TLatex *arxiv = new TLatex(); arxiv->SetNDC();
  //  //  arxiv->SetTextSize(0.035); arxiv->SetTextFont(42);
  //  //  if (useMC)
  //  //    arxiv->DrawLatex(0.68,0.88,"arXiv:1603.04053");
  //  //  else
  //  //    arxiv->DrawLatex(0.68,0.83,"arXiv:1603.04053");


  // // TPaveText* labelTop;
  // // if( useMC && scaleMC==0.) labelTop = MT2DrawTools::getLabelTopSimulation();
  // // else if( useMC )          labelTop = MT2DrawTools::getLabelTopSimulation(scaleMC);
  // // else        labelTop = MT2DrawTools::getLabelTop("CMS Preliminary, 2.2 fb^{-1} at #sqrt{s} = 13 TeV");
  // // labelTop->Draw("same");

  c1->SaveAs( Form("%s/ratio_%s%s.pdf", outdir.c_str(), qcd->region->getName().c_str(), (useMC && scaleMC!=0) ? Form("_%.1ffb",scaleMC) : "" ));
  c1->SaveAs( Form("%s/ratio_%s%s.eps", outdir.c_str(), qcd->region->getName().c_str(), (useMC && scaleMC!=0) ? Form("_%.1ffb",scaleMC) : "" ));
  

  delete c1;
  delete h2_axes;

}





void computePurity( TH1D* purity, TH1D* nonQCD, TH1D* all ,float lumi ) {

  for( int iBin=1; iBin<nonQCD->GetXaxis()->GetNbins()+1; ++iBin ) {

    float nonQCD_val = lumi * nonQCD->GetBinContent(iBin);
    float nonQCD_err = lumi * nonQCD->GetBinError  (iBin);
    if( nonQCD_err>nonQCD_val ) nonQCD_err = nonQCD_val;

    float allCR_val  = all->GetBinContent(iBin);
    float allCR_err  = all->GetBinError  (iBin);
    if( allCR_err>allCR_val ) allCR_err = allCR_val;
    //float allCR_err  = sqrt( allCR_val );

    float qcdPurity = (allCR_val>0.) ? (allCR_val-nonQCD_val)/allCR_val : 0.;
    if( qcdPurity<0. ) qcdPurity = 0.;
    
    float qcdPurityErr = (qcdPurity>0.) ? sqrt( nonQCD_err*nonQCD_err/(allCR_val*allCR_val) + allCR_err*allCR_err*nonQCD_val*nonQCD_val/(allCR_val*allCR_val*allCR_val*allCR_val) ) : 0.;

    // as agreed with FG
    if ( qcdPurity<0.001 ) {
      qcdPurity    = iBin>1 ? purity->GetBinContent( iBin-1 ) // take value from left neighbour
	                    : 1.0;  // first bin should not be empty (or extremely rarely) in MC based purity
      qcdPurityErr = iBin>1 ? purity->GetBinError( iBin-1 ) 
	                    : 1.0;  
    }

    purity->SetBinContent( iBin, qcdPurity    );
    purity->SetBinError  ( iBin, qcdPurityErr );

  }

}



void multiplyHisto( TH1D* histo, TH1D* other ) {

  for( int iBin=1; iBin<histo->GetXaxis()->GetNbins()+1; ++iBin ) {

    // val2==0 means zero purity, i.e, nCR=0, then don't modifiy original histo
    // 1. it won't change estimate that was zero in any case
    // 2. we keep the r_eff from the lower mt2 edge
    float val2     = other->GetBinContent(iBin);
    if ( val2==0 )   continue;

    float val1     = histo->GetBinContent(iBin);
    float val1_err = histo->GetBinError  (iBin);
    float val1_errRel = val1_err/val1;

    float val2_err = other->GetBinError  (iBin);
    float val2_errRel = val2_err/val2;

    float errRel = sqrt( val1_errRel*val1_errRel + val2_errRel*val2_errRel );

    histo->SetBinContent( iBin, val1*val2 );
    histo->SetBinError  ( iBin, errRel*val1*val2 );

  }

}



void scaleHisto( TH1D* histo, float val, float err ) {

  float relErr = (val>0.) ? err/val : 0.;

  for( int iBin=1; iBin<histo->GetXaxis()->GetNbins()+1; ++iBin ) {

    float thisVal = histo->GetBinContent(iBin);
    float newVal = thisVal*val;
    histo->SetBinContent( iBin, newVal );

    float thisError = histo->GetBinError(iBin);
    float thisRelErr = (thisVal>0.) ? thisError/thisVal : 0.;

    float newRelErr = sqrt( thisRelErr*thisRelErr + relErr*relErr );
    float newErr = newRelErr*newVal;

    histo->SetBinError( iBin, newErr );

  }


}

void getRelError( MT2EstimateSyst* reff, MT2Estimate* syst, const std::string& outputdir ) {
 
  for( int iBin=1; iBin<reff->yield->GetXaxis()->GetNbins()+1; ++iBin ) {
   
    float c = reff->yield         ->GetBinContent(iBin);
    float u = reff->yield_systUp  ->GetBinContent(iBin);
    float d = reff->yield_systDown->GetBinContent(iBin);

    float error = TMath::Max( fabs(c-u)/c, fabs(c-d)/c);
    
    syst->yield->SetBinContent(iBin, error);
    
  }
  
  if ( outputdir!=0 ) {
    TH1D *systHisto = (TH1D*)syst->yield->Clone();
    systHisto->Scale(100);
    systHisto->SetXTitle("M_{T2}");
    systHisto->SetYTitle("fit variation syst. [%]");
    float max = systHisto->GetMaximum()*1.25;
    systHisto->GetYaxis()->SetRangeUser(0,max);
    systHisto->SetLineWidth(2);

    TCanvas* can = new TCanvas( "can", "", 600, 600 );
  
    systHisto->Draw();

    std::vector<std::string> regionNiceNames = syst->region->getNiceNames();
    TPaveText* regionName = new TPaveText( 0.2, 0.81, 0.5, 0.9, "brNDC" );
    regionName->SetTextAlign( 11 );
    regionName->SetTextSize( 0.035 );
    regionName->SetFillColor( 0 );
    regionName->AddText( regionNiceNames[0].c_str() );
    regionName->AddText( regionNiceNames[1].c_str() );
    regionName->Draw("same");
    
    system( Form("mkdir -p %s%s", outputdir.c_str(),"/systVar/" ));

    TString name = outputdir + "/systVar/systFitVar_" + syst->region->getName();
    can->SaveAs(name+".pdf");
    can->SaveAs(name+".eps");
    can->SaveAs(name+".png");

    delete can;
  }
  
}


void addErrInQuadrature( TH1D* histo, TH1D* relErr ) {

  for( int iBin=1; iBin<histo->GetXaxis()->GetNbins()+1; ++iBin ) {
    
    float val       = histo ->GetBinContent(iBin);
    
    if (val !=0 ) {
      float errRel1   = histo ->GetBinError  (iBin)/val;
      float errRel2   = relErr->GetBinContent(iBin); 
      float totErrRel = sqrt(errRel1*errRel1 + errRel2*errRel2);
      
      histo->SetBinError( iBin, totErrRel*val );
    }
  }
}
