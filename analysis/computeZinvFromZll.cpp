#include <iostream> 
#include <sstream>
#include <fstream>
#include <iomanip>

#include "interface/MT2Config.h"
#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"
#include "interface/MT2DrawTools.h"
#include "interface/MT2EstimateTree.h"
#include "interface/MT2EstimateSyst.h"


#define mt2_cxx
#include "../interface/mt2.h"

#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TLorentzVector.h"


using namespace std;

/*
  
  This script aims in computing the Z invisible estimate (computed from Z->ll decays) for a given year.

*/



//---------------------------------------------------------//
// Choose here the correct extrapolation region set
//the regions are not the same for Moriond2019 compared to Moriond2017, and this has an impact on the regionsSet we use to integrate over Nb
  
bool forMoriond2019 = true; 
bool forMoriond2017 = false;

bool doIntegrationOverNbWithMergedRegions = false; //test with merged Nj regions
bool dontDoIntegration = false; //introduced for debugging purposes only

//---------------------------------------------------------//


MT2Analysis<MT2EstimateSyst>* computePurityOF( MT2Analysis<MT2Estimate>* SF, MT2Analysis<MT2Estimate>* OF, const MT2Config& cfg, bool do_Runcert=0 );

void extrapolToTopoRegion( MT2Analysis<MT2Estimate>* shape_TR, MT2Analysis<MT2Estimate>* shape, bool mergedHighNj, bool isMC=0 );

void buildHybrid( MT2Analysis<MT2Estimate>* shape_hybrid, MT2Analysis<MT2Estimate>* shape_data, MT2Analysis<MT2Estimate>* shape_MCsr, MT2Analysis<MT2Estimate>* shape_MCcr, MT2Analysis<MT2Estimate>* shape_MCcr_forExtremeHT, MT2Analysis<MT2Estimate>* bin_extrapol );

int getFixedExtrapolBin( MT2Region* region, TH1D* histo );




int main( int argc, char* argv[] ) { 


  std::cout << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|           Running computeZinvFromZll               |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc!=2 ) {
    std::cout << "USAGE: ./computeZinvFromZll [configFileName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

 
  std::string configFileName(argv[1]);
  MT2Config cfg(configFileName);


  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

  float lumi = cfg.lumi();

  std::string zllControlRegionDir = cfg.getEventYieldDir() + "/zllControlRegion"; 

  //we get the necessary data, MCcr and MCsr trees
  MT2Analysis<MT2EstimateTree>* zllData_tree = MT2Analysis<MT2EstimateTree>::readFromFile(zllControlRegionDir + "/data.root", "data");
  MT2Analysis<MT2EstimateTree>* zllMC_tree = MT2Analysis<MT2EstimateTree>::readFromFile(zllControlRegionDir + "/mc.root", "zllCR");
  MT2Analysis<MT2EstimateTree>* zinvMC_tree = MT2Analysis<MT2EstimateTree>::readFromFile(cfg.getEventYieldDir() + "/ZJetsInclusive.root", "ZJets"); 
 

  //get the data files
  MT2Analysis<MT2Estimate>* zllData_forHybrid_notIntegral;
  MT2Analysis<MT2Estimate>* zllData_of_forHybrid_notIntegral;
  zllData_forHybrid_notIntegral = MT2Analysis<MT2Estimate>::readFromFile(zllControlRegionDir + "/data_forZinvEst.root", "data");
  zllData_of_forHybrid_notIntegral = MT2Analysis<MT2Estimate>::readFromFile(zllControlRegionDir + "/data_of_forZinvEst.root", "data_of");
    
  // first component for the computation of the estimate: Number of events in the data di-lepton control region
  MT2Analysis<MT2Estimate>* zllData_of_forHybrid = MT2Estimate::makeIntegralAnalysisFromEstimate( "zllData_of_forHybrid", cfg.regionsSet(), zllData_of_forHybrid_notIntegral );
  MT2Analysis<MT2Estimate>* zllData_forHybrid = MT2Estimate::makeIntegralAnalysisFromEstimate( "zllData_forHybrid", cfg.regionsSet(), zllData_forHybrid_notIntegral );
 
  
  //second component to build the estimate: purity of Z->ll in the control sample (retrieved from SF and OF data samples)
  MT2Analysis<MT2EstimateSyst>* purity_forHybrid = computePurityOF(zllData_forHybrid, zllData_of_forHybrid, cfg, 1); 
  purity_forHybrid->setName("purity_forHybrid");

 

  /////////////////////////////////////////////////////
  //      begin of hybrid shape implementation       //
  /////////////////////////////////////////////////////
  
  //zllHybrid_shape_TR will be the third element to build the estimate

  //region_forExtrapol is the region with integrated Nb: be sure we use the one corresponding to the cfg.regionsSet() (zurich2016 vs Moriond2019)
  std::string region_forExtrapol;
  if(forMoriond2017){
    region_forExtrapol = "zurich2016_forExtrapol";
    cout << endl << "INFO: the computation of the estimate will be performed with the regions set used for Moriond2017" << endl << endl;
  }
  else if(forMoriond2019){
    if(!doIntegrationOverNbWithMergedRegions && !dontDoIntegration){
      region_forExtrapol = "Moriond2019_forExtrapol";
      cout << endl << "INFO: the computation of the estimate will be performed with the regions set used for Moriond2019" << endl << endl;
    }
    else if(!doIntegrationOverNbWithMergedRegions && dontDoIntegration){
      region_forExtrapol = "Moriond2019";
      cout << endl << "INFO: the computation of the estimate will be performed with the NON-INTEGRATED regions set used for Moriond2019" << endl << endl;
    }
    else{
      region_forExtrapol = "Moriond2019_forExtrapol_merged";
      cout << endl << "INFO: the computation of the estimate will be performed with the regions set used for Moriond2019 - merged" << endl << endl;
    }    
  }
  
  TH1::AddDirectory(kTRUE); // stupid ROOT memory allocation needs this 
 
  //we fill the shapes in the integrated Nb regions (moriond2019_forExtrapol) 
   MT2Analysis<MT2EstimateTree>* zllData_shape = MT2EstimateTree::makeAnalysisFromInclusiveTree( "shape" , region_forExtrapol, zllData_tree,  "((fabs(Z_mass-91.19)<=20.) && Z_pt>=200.)");
  cout << "Shape 1/4 filled" << endl;

  //MT2Analysis<MT2Estimate>* zllData_shape_TR = new MT2Analysis<MT2Estimate>("zllData_shape_TR", cfg.regionsSet() );
  //extrapolToTopoRegion( zllData_shape_TR, (MT2Analysis<MT2Estimate>*)zllData_shape, doIntegrationOverNbWithMergedRegions );
  
  //for the MC shapes, we normalize them to luminosity
  MT2Analysis<MT2EstimateTree>* zinvMC_forShape = MT2EstimateTree::makeAnalysisFromInclusiveTree( "zinv_forShape" , region_forExtrapol, zinvMC_tree,  "");
  (*zinvMC_forShape) *= lumi; 
  cout << "Shape 2/4 filled" << endl;

 
  MT2Analysis<MT2EstimateTree>* zllMC_shape = MT2EstimateTree::makeAnalysisFromInclusiveTree( "zllMC_shape" , region_forExtrapol, zllMC_tree,  "(fabs(Z_mass-91.19)<=20.) && Z_pt>200.");
  (*zllMC_shape) *= lumi;
  cout << "Shape 3/4 filled" << endl;
  
  //for extreme ht we use the shape per topological region, so not the integrated one
  MT2Analysis<MT2EstimateTree>* zllMC_shape_forExtremeHT = MT2EstimateTree::makeAnalysisFromInclusiveTree( "zllMC_shape_forExtremeHT" , cfg.regionsSet(), zllMC_tree,  "(fabs(Z_mass-91.19)<=20.) && Z_pt>200.");
  (*zllMC_shape_forExtremeHT) *= lumi;
  cout << "Shape 4/4 filled" << endl;

 
  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this
  
  //declaring the elements entering in the buildHybrid function
  MT2Analysis<MT2Estimate>* bin_extrapol  = new MT2Analysis<MT2Estimate>( "bin_extrapol", cfg.regionsSet() );
  MT2Analysis<MT2Estimate>* zllData_shape_TR = new MT2Analysis<MT2Estimate>("zllData_shape_TR", cfg.regionsSet() ); 
  MT2Analysis<MT2Estimate>* zllMC_shape_TR = new MT2Analysis<MT2Estimate>( "zllMC_shape_TR", cfg.regionsSet() );
  MT2Analysis<MT2Estimate>* zllHybrid_shape_TR = new MT2Analysis<MT2Estimate>( "zllHybrid_shape_TR", cfg.regionsSet() ); //will be the equivalent of k_hybrid in the transfert function of Z invisible estimate
  MT2Analysis<MT2Estimate>* zinvMC_forShape_TR = new MT2Analysis<MT2Estimate>( "zinvMC_forShape_TR", cfg.regionsSet() );
  
  //since the estimates were created in the Nb-integrated regions set (moriond2019_forExtrapol), we have to redistribute the yields in all the topological bins
  //e.g HT250to450_j2to3_b0toInf will fill (identically) HT250to450_j2to3_b0, HT250to450_j2to3_b1, HT250to450_j2to3_b2
  extrapolToTopoRegion( zllData_shape_TR, (MT2Analysis<MT2Estimate>*)zllData_shape, doIntegrationOverNbWithMergedRegions );
  extrapolToTopoRegion( zllMC_shape_TR, (MT2Analysis<MT2Estimate>*)zllMC_shape, doIntegrationOverNbWithMergedRegions, 1 ); //1 means it is mc
  extrapolToTopoRegion( zinvMC_forShape_TR, (MT2Analysis<MT2Estimate>*)zinvMC_forShape, doIntegrationOverNbWithMergedRegions, 1 );

  //objects introduced for debugging purposes only
  MT2Analysis<MT2Estimate>* zllData_shape_TR_forTest = new MT2Analysis<MT2Estimate>("zllData_shape_TR_forTest", cfg.regionsSet() );
  zllData_shape_TR_forTest->setName("zllData_shape_TR_forTest");
  extrapolToTopoRegion( zllData_shape_TR_forTest, (MT2Analysis<MT2Estimate>*)zllData_shape, doIntegrationOverNbWithMergedRegions );

  MT2Analysis<MT2Estimate>* zllMC_shape_TR_forTest = new MT2Analysis<MT2Estimate>("zllMC_shape_TR_forTest", cfg.regionsSet() );
  zllMC_shape_TR_forTest->setName("zllMC_shape_TR_forTest");
  extrapolToTopoRegion( zllMC_shape_TR_forTest, (MT2Analysis<MT2Estimate>*)zllMC_shape, doIntegrationOverNbWithMergedRegions, 1 );

  MT2Analysis<MT2Estimate>* zinvMC_forShape_TR_forTest = new MT2Analysis<MT2Estimate>("zinvMC_forShape_TR_forTest", cfg.regionsSet() );
  zinvMC_forShape_TR_forTest->setName("zinvMC_forShape_TR_forTest");
  extrapolToTopoRegion( zinvMC_forShape_TR_forTest, (MT2Analysis<MT2Estimate>*)zinvMC_forShape, doIntegrationOverNbWithMergedRegions, 1 );
 
  
  //we build the hybrid shape with the above-created elements
  buildHybrid( zllHybrid_shape_TR, zllData_shape_TR, zinvMC_forShape_TR, zllMC_shape_TR, (MT2Analysis<MT2Estimate>*)zllMC_shape_forExtremeHT, bin_extrapol );
  

  //TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this
 

  /////////////////////////////////////////////////////
  //       end of hybrid shape implementation        //
  /////////////////////////////////////////////////////



  //we create the ZinvZllRatio (4th component to build the estimate)
  MT2Analysis<MT2Estimate>* Zinv_forHybrid_notIntegral = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "ZJets");
  (* (MT2Analysis<MT2Estimate>*) Zinv_forHybrid_notIntegral) = (* (MT2Analysis<MT2Estimate>*)Zinv_forHybrid_notIntegral) * lumi;
  MT2Analysis<MT2Estimate>* zllMC_forHybrid_notIntegral = MT2Analysis<MT2Estimate>::readFromFile(zllControlRegionDir + "/mc_forZinvEst.root", "zllCR");
  (*zllMC_forHybrid_notIntegral) *= lumi;

  MT2Analysis<MT2Estimate>* zllMC_forHybrid = MT2Estimate::makeIntegralAnalysisFromEstimate( "zllMC_forHybrid", cfg.regionsSet(), zllMC_forHybrid_notIntegral );
  MT2Analysis<MT2Estimate>* Zinv_forHybrid = MT2Estimate::makeIntegralAnalysisFromEstimate( "Zinv_forHybrid", cfg.regionsSet(), Zinv_forHybrid_notIntegral );

  MT2Analysis<MT2Estimate>* ZinvZllRatioHybrid = new MT2Analysis<MT2Estimate>( "ZinvZllRatioHybrid", cfg.regionsSet() );
  (*ZinvZllRatioHybrid) = ( *Zinv_forHybrid) / (*zllMC_forHybrid);
   
  //TH1::AddDirectory(kTRUE); // stupid ROOT memory allocation needs this

  //we have now all the necessary components to build the estimate

  //intermediate component to compute the final estimate (needed for the datacard)
  MT2Analysis<MT2Estimate>* alpha = new MT2Analysis<MT2Estimate>( "alpha", cfg.regionsSet() );
  (*alpha) =  (*zllHybrid_shape_TR) * (*ZinvZllRatioHybrid);
 
 
  //we write finally down the final estimate of the Z invisible background retrieved from Zll control sample
  MT2Analysis<MT2Estimate>* ZinvEstimateFromZll_hybrid = new MT2Analysis<MT2Estimate>( "ZinvEstimateFromZll_hybrid", cfg.regionsSet() );
  (*ZinvEstimateFromZll_hybrid) = (*zllData_forHybrid) * (*purity_forHybrid) * (*alpha) ;


  
  //needed for further studies, please ignore
  MT2Analysis<MT2Estimate>* kHybrid_alternative = new MT2Analysis<MT2Estimate>( "kHybrid_alternative", cfg.regionsSet() );
  (*kHybrid_alternative) = (*zllData_shape_TR) * (*zinvMC_forShape_TR) / (*zllMC_shape_TR);
  
  std::set<MT2Region> regions = kHybrid_alternative->getRegions();
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {    
    double integral = kHybrid_alternative->get(*iR)->yield->Integral(1, -1);
    kHybrid_alternative->get(*iR)->yield->Scale(1/integral);
  }

  MT2Analysis<MT2Estimate>* ZinvEstimateFromZll_hybrid_fullData = new MT2Analysis<MT2Estimate>( "ZinvEstimateFromZll_hybrid_fullData", cfg.regionsSet() );
  (*ZinvEstimateFromZll_hybrid_fullData) = (*zllData_forHybrid) * (*purity_forHybrid) * (*ZinvZllRatioHybrid) * (*kHybrid_alternative) ;


  MT2Analysis<MT2Estimate>* kHybrid_alternative_MC = new MT2Analysis<MT2Estimate>( "kHybrid_alternative_MC", cfg.regionsSet() );

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {    
    double integral_MCcr = zllMC_shape_TR->get(*iR)->yield->Integral(1, -1);
    double integral_data = zllData_shape_TR->get(*iR)->yield->Integral(1, -1);

    kHybrid_alternative_MC->get(*iR)->yield = (TH1D*) zinvMC_forShape_TR->get(*iR)->yield->Clone();
    kHybrid_alternative_MC->get(*iR)->yield->Scale(integral_data/integral_MCcr);
    
    double integral =  kHybrid_alternative_MC->get(*iR)->yield->Integral(1,-1);
    kHybrid_alternative_MC->get(*iR)->yield->Scale(1/integral);
    
  }

  MT2Analysis<MT2Estimate>* ZinvEstimateFromZll_hybrid_fullMC = new MT2Analysis<MT2Estimate>( "ZinvEstimateFromZll_hybrid_fullMC", cfg.regionsSet() );
  (*ZinvEstimateFromZll_hybrid_fullMC) = (*zllData_forHybrid) * (*purity_forHybrid) * (*ZinvZllRatioHybrid) * (*kHybrid_alternative_MC) ;


  //we save in the output file
  std::string outFile = cfg.getEventYieldDir() + "/zinvFromZll.root";

  zllData_shape	              ->addToFile(outFile);				  			     
  zllMC_shape	              ->addToFile(outFile);
  zllMC_shape_TR              ->addToFile(outFile);
  zllMC_shape_TR_forTest      ->addToFile(outFile);
  purity_forHybrid            ->addToFile(outFile);	
  bin_extrapol                ->addToFile(outFile);  
  ZinvZllRatioHybrid          ->addToFile(outFile); 
  ZinvEstimateFromZll_hybrid  ->addToFile(outFile);
  ZinvEstimateFromZll_hybrid_fullData  ->addToFile(outFile);
  ZinvEstimateFromZll_hybrid_fullMC  ->addToFile(outFile);
  zllData_forHybrid           ->addToFile(outFile);
  zllData_shape_TR            ->addToFile(outFile);
  zllData_shape_TR_forTest    ->addToFile(outFile);
  zinvMC_forShape             ->addToFile(outFile);
  zinvMC_forShape_TR          ->addToFile(outFile);
  zinvMC_forShape_TR_forTest  ->addToFile(outFile);
  zllHybrid_shape_TR          ->addToFile(outFile);
  kHybrid_alternative         ->addToFile(outFile);
  alpha                       ->addToFile(outFile);
  zllData_of_forHybrid        ->addToFile(outFile);
  zllData_forHybrid           ->addToFile(outFile);
  zllMC_shape_forExtremeHT    ->addToFile(outFile);
   
  delete zllData_tree;
  
  return 0;

}





//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                                                                                      //
//                         IMPLEMENTATION OF THE FUNCTIONS                              //
//                                                                                      //
// 
//////////////////////////////////////////////////////////////////////////////////////////






MT2Analysis<MT2EstimateSyst>* computePurityOF( MT2Analysis<MT2Estimate>* SF, MT2Analysis<MT2Estimate>* OF, const MT2Config& cfg, bool do_Runcert ) {

  std::string SFname = SF->getName();
  std::string OFname = OF->getName();

  // temporarily set all names to the output name so that returned MT2Analysis has consistent naming in all regions:
  std::string estimateName = "purity";
  SF->setName( estimateName );
  OF->setName( estimateName );

  std::set<MT2Region> regions = SF->getRegions();

  std::set<MT2EstimateSyst*> newData;

  ofstream writeToFile("purity.txt");

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    MT2Region* region = new MT2Region( *iR );
    std::vector< std::string > niceNames = region->getNiceNames();

    writeToFile << endl << endl << region->getName() << endl;

    MT2Estimate* SFEst = SF->get(*iR);
    MT2Estimate* OFEst = OF->get(*iR);
   
    MT2EstimateSyst* thisNewEstimate;
    thisNewEstimate =  new MT2EstimateSyst(*OFEst);
    for( int ibin=1; ibin<thisNewEstimate->yield->GetNbinsX()+1; ++ibin ) {
      float contentSF = SFEst->yield->GetBinContent(ibin);
      float contentOF = OFEst->yield->GetBinContent(ibin);
      
      writeToFile << "contentSF = " << contentSF << endl;
      writeToFile << "contentOF = " << contentOF << endl;

      //R(SF/OF) ratio
      float R_sfof;
      float R_sfof_err;
      if(cfg.year()==2016){
        R_sfof = 1.12;
        R_sfof_err = 0.0; 
      }
      else if(cfg.year()==2017){
	R_sfof = 1.02;
	R_sfof_err = 0.0; 
      }
      else if(cfg.year()==2018){
	R_sfof = 1.04;
	R_sfof = 0.0;
      }
	
      writeToFile << "R(SF/OF) = " << R_sfof << endl;

      float purity = 0.0;
      if(contentSF>(R_sfof * contentOF)){
	purity = (contentSF - R_sfof * contentOF)/ contentSF;
	writeToFile << "purity = (" << contentSF << " - " << R_sfof << " * " << contentOF << ")/ " << contentSF << endl;
	writeToFile << "purity = " << purity << endl;
	writeToFile << endl;

      }
      
      float purity_err = 1.0;
      if( purity > 0.){
	if(do_Runcert)
	  purity_err = sqrt( sqrt( contentOF )*sqrt( contentOF )*R_sfof*R_sfof + R_sfof_err*R_sfof_err* contentOF * contentOF  )/ contentSF;
	else
	  purity_err = sqrt( contentOF )/ contentSF;
      }else{
	purity = 1.0;
	purity_err = 1.0;	
      }

      if( contentOF == 0 ){
	if( contentSF > 0 )
	  purity_err = 1.8/ contentSF; //uncert = 1.8 in case of 0 events
	else
	  purity_err = 1.0;
      }
       
      thisNewEstimate->yield->SetBinContent( ibin, purity );
     
      thisNewEstimate->yield->SetBinError( ibin, purity_err );
      
      thisNewEstimate->yield_systUp->SetBinContent( ibin, 1+purity_err );
      thisNewEstimate->yield_systDown->SetBinContent( ibin, 1-purity_err );

    }
    newData.insert( thisNewEstimate );
  }
  MT2Analysis<MT2EstimateSyst>* analysis = new MT2Analysis<MT2EstimateSyst>( estimateName, newData );

  // set names back to original:
  SF->setName( SFname );
  OF->setName( OFname );

  return analysis;
}



void extrapolToTopoRegion( MT2Analysis<MT2Estimate>* shape_TR, MT2Analysis<MT2Estimate>* shape, bool mergedHighNj, bool isMC ) {

  std::set<MT2Region> regions       = shape_TR->getRegions();
  std::set<MT2Region> regions_shape = shape->getRegions();
  
  //////////////loop over the SR regions, fill if contained//////////////

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {
 
    MT2Region* regionToMatch = new MT2Region( *iR );

    cout << endl << endl << "Analysis for the NORMAL REGION: " << regionToMatch->getName() << endl;
     
    TH1D* this_shape_TR    = shape_TR   ->get(*iR)->yield;
  
    int nBins = this_shape_TR->GetNbinsX();

    //Loop over the shape regions///////////FILL CORRECTLY THE DATA SHAPE (NON NORMALIZED)
    for( std::set<MT2Region>::iterator iR_shape=regions_shape.begin(); iR_shape!=regions_shape.end(); ++iR_shape ) { 
    
      MT2Region* regionToMatch_shape = new MT2Region( *iR_shape );
     
      cout << "Try with forExtrapol region: " << regionToMatch_shape->getName() << endl;
      

      if( !(regionToMatch->MT2Region::isIncluded(regionToMatch_shape)) && !(regionToMatch->nJetsMin()==2 && (regionToMatch->nJetsMax()==6 || regionToMatch->nJetsMax()==-1 || regionToMatch->nJetsMax()==3) )  ){continue;}  //not contained, doesn't matter
      if( regionToMatch->htMin() != regionToMatch_shape->htMin() ){continue;} //HT has to match both high and low for all regions
      if( regionToMatch->htMax() != regionToMatch_shape->htMax() ){continue;}

      if(!mergedHighNj){
	if(regionToMatch->htMin()!=1500){
	  if(regionToMatch_shape->nJetsMin() < regionToMatch->nJetsMin()) continue; //to avoid monojets to be counted where they should not
	  if((regionToMatch_shape->nJetsMin() > regionToMatch->nJetsMax()) && regionToMatch->nJetsMin()!= 7 && regionToMatch->nJetsMin()!=10) continue; //so that higher Nj regions don't fill lower Nj regions
	  if(regionToMatch->nJetsMin()==2 && regionToMatch->nJetsMax()==6){
	    if(regionToMatch_shape->nJetsMin()==2) continue; //since 2-6j regions require a minimum of 3b, then we exclude the 2j region
	  }
	}
      }
      else{ //slightly different treatment when the high Nj regions are merged
	if(regionToMatch->htMin()!=1500){
	  if((regionToMatch_shape->nJetsMin() < regionToMatch->nJetsMin()) && regionToMatch->nJetsMin()!=10) continue; //to avoid monojets to be counted where they should not
	  if((regionToMatch_shape->nJetsMin() > regionToMatch->nJetsMax()) && regionToMatch->nJetsMin()!= 7 && regionToMatch->nJetsMin()!=10) continue; //so that higher Nj regions don't fill lower Nj regions
	  if(regionToMatch->nJetsMin()==2 && regionToMatch->nJetsMax()==6){
	    if(regionToMatch_shape->nJetsMin()==2) continue; //since 2-6j regions require a minimum of 3b, then we exclude the 2j region
	  }
	}
      }

      //previous selection:
      /*if( regionToMatch->nJetsMin()==2 && (regionToMatch->nJetsMax()==6 || regionToMatch->nJetsMax()==-1) ){
	//not true anymore due to 2 & 3 jets splitting, those have not to be summed no matter what,	if( (regionToMatch->nJetsMin() != regionToMatch_shape->nJetsMin()) && (regionToMatch->nJetsMax() != regionToMatch_shape->nJetsMax() ) ) continue; //lower or upper njets has to match, but not both
	if ( regionToMatch->nBJetsMin()==3 && regionToMatch_shape->nJetsMin()==2 && regionToMatch->htMin()!=1500 ){continue;} //removing the 2j bin for the 3b shape (3b is ~requiring 3j)
	}
      
      if( regionToMatch->nJetsMin()==2 && regionToMatch->nJetsMax()==3 ){
	if( !(regionToMatch_shape->nJetsMin()==2 || regionToMatch_shape->nJetsMin()==3) ){continue;}
      }

      if( regionToMatch->nJetsMin()==2 && regionToMatch->nJetsMax()==3  && regionToMatch->nBJetsMin()==0 ){
       	std::cout << "doing the 2-3 region " <<  regionToMatch->htMin() << "to" << regionToMatch->htMax()<<  " using the shape region " << regionToMatch_shape->nJetsMin() << "to" << regionToMatch_shape->nJetsMax() << "jets " <<  regionToMatch->htMin() << "to" << regionToMatch->htMax() << std::endl;
      }
      */
      

      cout << "not rejected" << endl;
     
      //yields from the forExtrapol regions
      TH1D* this_shape  = shape->get(*iR_shape)->yield;
      int   nBins_shape = this_shape->GetNbinsX();

      //we loop on the bins of main region set
      for(int iBin=1; iBin<= nBins; iBin++){
	cout << "iBin : nBins : nBins_forExtrapol : " << iBin << " : " << nBins << " : " << nBins_shape << endl;

	if(iBin == nBins && nBins_shape > nBins){
	  if( regionToMatch->nJetsMin()==2 && (regionToMatch->nJetsMax()==6 || regionToMatch->nJetsMax()==-1 || regionToMatch->nJetsMax()==3 ) && this_shape_TR->GetBinContent( iBin )!=0 ){
	    	  
	    //If MC
	    if( isMC ){
	      double int_err= 0.;
	      this_shape->IntegralAndError(iBin,-1,int_err);
	      double int_err_previous_Region= 0.;
	      this_shape_TR->IntegralAndError(iBin,-1,int_err_previous_Region);
	      this_shape_TR->SetBinContent( iBin, this_shape->Integral(iBin,-1)+this_shape_TR->Integral( iBin,-1 ) );
	      this_shape_TR->SetBinError( iBin, sqrt( int_err*int_err +int_err_previous_Region *int_err_previous_Region ) );
	      cout << "[1] Filling with... " << this_shape->Integral(iBin,-1)+this_shape_TR->Integral( iBin,-1 ) << endl; 
	    }else{ 
	      cout << "[1] Filling with... " << this_shape->Integral(iBin,-1)+this_shape_TR->Integral( iBin,-1 ) << endl;
	      this_shape_TR->SetBinError( iBin, sqrt( this_shape->Integral(iBin,-1)*this_shape->Integral(iBin,-1)+this_shape_TR->Integral( iBin,-1 )*this_shape_TR->Integral( iBin,-1 )) );
	      this_shape_TR->SetBinContent( iBin, this_shape->Integral(iBin,-1)+this_shape_TR->Integral( iBin,-1 ) );
	    }
	    
	  }else{
	    if( isMC ){
	      double int_err= 0.;
	      this_shape->IntegralAndError(iBin,-1,int_err);
	      this_shape_TR->SetBinContent( iBin, this_shape->Integral(iBin, -1) );
	      this_shape_TR->SetBinError( iBin, int_err );
	      cout << "[2] Filling with... " << this_shape->Integral(iBin, -1) << endl;
	    }else{
	      cout << "[2] Filling with... " << this_shape->Integral(iBin, -1) << endl;
	      this_shape_TR->SetBinContent( iBin, this_shape->Integral(iBin, -1) );
	      this_shape_TR->SetBinError( iBin, sqrt(this_shape->Integral(iBin, -1)) );
	    }
	  }
	}else{ //if it is not the last bin
	  if( regionToMatch->nJetsMin()==2 && (regionToMatch->nJetsMax()==6 || regionToMatch->nJetsMax()==-1 || regionToMatch->nJetsMax()==3 ) && this_shape_TR->GetBinContent( iBin )!=0 ){
	    //if( regionToMatch->nJetsMin()==2 && (regionToMatch->nJetsMax()==6|| regionToMatch->nJetsMax()==-1) && this_shape_TR->GetBinContent( iBin )!=0 ){

	    if( !isMC ){
	      cout << "[3] Filling with... " << this_shape->GetBinContent(iBin)+this_shape_TR->GetBinContent( iBin ) << endl;
	      this_shape_TR->SetBinError( iBin, sqrt(this_shape->GetBinContent(iBin)+this_shape_TR->GetBinContent(iBin)));
	      this_shape_TR->SetBinContent(iBin,this_shape->GetBinContent(iBin)+this_shape_TR->GetBinContent( iBin ) );
	     
	    }else{ 
	     
	      this_shape_TR->SetBinError( iBin, sqrt(this_shape->GetBinError(iBin)*this_shape->GetBinError(iBin)+this_shape_TR->GetBinError(iBin)*this_shape_TR->GetBinError(iBin)));
	      cout << "[3] Filling with... " << this_shape->GetBinContent(iBin)+this_shape_TR->GetBinContent( iBin ) << endl;  
	      this_shape_TR->SetBinContent(iBin,this_shape->GetBinContent(iBin)+this_shape_TR->GetBinContent( iBin ) );
	     
	    }  
	  }else{
	    if( !isMC ){
	      cout << "[4] Filling with... " << this_shape->GetBinContent(iBin)<< endl;
	      this_shape_TR->SetBinContent( iBin, this_shape->GetBinContent(iBin));
	      this_shape_TR->SetBinError( iBin, sqrt(this_shape->GetBinContent(iBin)));
	    }else{
	      this_shape_TR->SetBinContent( iBin, this_shape->GetBinContent(iBin));
	      this_shape_TR->SetBinError( iBin, this_shape->GetBinError(iBin) );
	      cout << "[4] Filling with... " << this_shape->GetBinContent(iBin) << endl;
	    }
	  }

	} 
      }//filled and fine
    }//end loop over shape regions
  }//end loop over TR 

 
  return;

}





int getFixedExtrapolBin( MT2Region* region, TH1D* histo ){

  int bin_extrapol = 1;
  int val_mt2 = 200;

  string regionName = region->getName();
  
  if(regionName=="HT250to450_j2to3_b0") bin_extrapol = 4;
  else if(regionName=="HT250to450_j2to3_b1") bin_extrapol = 4;
  else if(regionName=="HT250to450_j2to3_b2") bin_extrapol = 4;
  else if(regionName=="HT250to450_j4to6_b0") bin_extrapol = 2;
  else if(regionName=="HT250to450_j4to6_b1") bin_extrapol = 2;
  else if(regionName=="HT250to450_j4to6_b2") bin_extrapol = 2;
  else if(regionName=="HT250to450_j7toInf_b0") bin_extrapol = 1;
  else if(regionName=="HT250to450_j7toInf_b1") bin_extrapol = 1;
  else if(regionName=="HT250to450_j7toInf_b2") bin_extrapol = 1;
  else if(regionName=="HT250to450_j2to6_b3toInf") bin_extrapol = 4;
  else if(regionName=="HT250to450_j7toInf_b3toInf") bin_extrapol = 1;

  
  else if(regionName=="HT450to575_j2to3_b0") bin_extrapol = 5;
  else if(regionName=="HT450to575_j2to3_b1") bin_extrapol = 5;
  else if(regionName=="HT450to575_j2to3_b2") bin_extrapol = 5;
  else if(regionName=="HT450to575_j4to6_b0") bin_extrapol = 3;
  else if(regionName=="HT450to575_j4to6_b1") bin_extrapol = 3;
  else if(regionName=="HT450to575_j4to6_b2") bin_extrapol = 3;
  else if(regionName=="HT450to575_j7toInf_b0") bin_extrapol = 1;
  else if(regionName=="HT450to575_j7toInf_b1") bin_extrapol = 1;
  else if(regionName=="HT450to575_j7toInf_b2") bin_extrapol = 1;
  else if(regionName=="HT450to575_j2to6_b3toInf") bin_extrapol = 5;
  else if(regionName=="HT450to575_j7toInf_b3toInf") bin_extrapol = 1;


  else if(regionName=="HT575to1200_j2to3_b0") bin_extrapol = 7;
  else if(regionName=="HT575to1200_j2to3_b1") bin_extrapol = 5;
  else if(regionName=="HT575to1200_j2to3_b2") bin_extrapol = 6;
  else if(regionName=="HT575to1200_j4to6_b0") bin_extrapol = 6;
  else if(regionName=="HT575to1200_j4to6_b1") bin_extrapol = 4;
  else if(regionName=="HT575to1200_j4to6_b2") bin_extrapol = 4;
  else if(regionName=="HT575to1200_j2to6_b3toInf") bin_extrapol = 6;
  else if(regionName=="HT575to1200_j7to9_b0") bin_extrapol = 2;
  else if(regionName=="HT575to1200_j7to9_b1") bin_extrapol = 2;
  else if(regionName=="HT575to1200_j7to9_b2") bin_extrapol = 2;
  else if(regionName=="HT575to1200_j7to9_b3") bin_extrapol = 2;
  else if(regionName=="HT575to1200_j7to9_b4toInf") bin_extrapol = 2;
  else if(regionName=="HT575to1200_j10toInf_b0") bin_extrapol = 2;
  else if(regionName=="HT575to1200_j10toInf_b1") bin_extrapol = 2;
  else if(regionName=="HT575to1200_j10toInf_b2") bin_extrapol = 2;
  else if(regionName=="HT575to1200_j10toInf_b3") bin_extrapol = 3;
  else if(regionName=="HT575to1200_j10toInf_b4toInf") bin_extrapol = 2;


  else if(regionName=="HT1200to1500_j2to3_b0") bin_extrapol = 3;
  else if(regionName=="HT1200to1500_j2to3_b1") bin_extrapol = 3;
  else if(regionName=="HT1200to1500_j2to3_b2") bin_extrapol = 3;
  else if(regionName=="HT1200to1500_j4to6_b0") bin_extrapol = 3;
  else if(regionName=="HT1200to1500_j4to6_b1") bin_extrapol = 3;
  else if(regionName=="HT1200to1500_j4to6_b2") bin_extrapol = 3;
  else if(regionName=="HT1200to1500_j2to6_b3toInf") bin_extrapol = 4;
  else if(regionName=="HT1200to1500_j7to9_b0") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j7to9_b1") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j7to9_b2") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j7to9_b3") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j7to9_b4toInf") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j10toInf_b0") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j10toInf_b1") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j10toInf_b2") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j10toInf_b3") bin_extrapol = 1;
  else if(regionName=="HT1200to1500_j10toInf_b4toInf") bin_extrapol = 2;


  else if(regionName=="HT1500toInf_j2to3_b0") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j2to3_b1") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j2to3_b2") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j4to6_b0") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j4to6_b1") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j4to6_b2") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j2to6_b3toInf") bin_extrapol = 4;
  else if(regionName=="HT1500toInf_j7to9_b0") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j7to9_b1") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j7to9_b2") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j7to9_b3") bin_extrapol = 2;
  else if(regionName=="HT1500toInf_j7to9_b4toInf") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j10toInf_b0") bin_extrapol = 2;
  else if(regionName=="HT1500toInf_j10toInf_b1") bin_extrapol = 2;
  else if(regionName=="HT1500toInf_j10toInf_b2") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j10toInf_b3") bin_extrapol = 3;
  else if(regionName=="HT1500toInf_j10toInf_b4toInf") bin_extrapol = 3;

  else{ cout << "PROBLEM IN THE REGION NAME " << regionName <<  endl; 
}

  /*if(region->htMin()==250 && region->htMax()==450 ){
    if( region->nJetsMin()==2 )
      val_mt2 = 400;
    else
      val_mt2 = 300;
  
  }else if(region->htMin()==450 && region->htMax()==575 ){
    if( (region->nJetsMin()==2 && region->nJetsMax()==3 ) || region->nJetsMin()==4  )
      val_mt2 = 400;
    else if( region->nJetsMax()==6 )
      val_mt2 = 500;
  
  }else if(region->htMin()==575 && region->htMax()==1000 ){
    if( region->nJetsMin()==2 || region->nJetsMin()==4  )
      val_mt2 = 600;
  
  }else if(region->htMin()==1000 && region->htMax()==1500 ){
    if( (region->nJetsMin()==2 && region->nJetsMax()==3 ) || region->nJetsMin()==4  )
      val_mt2 = 400;
    else if( region->nJetsMax()==6 )
      val_mt2 = 600; 
  
  }else if(region->htMin()==1500 ){ // only one data template
    val_mt2 = 400;
 
  }

  bin_extrapol = histo->FindBin( val_mt2 );

  if( histo->GetBinLowEdge(bin_extrapol) != val_mt2 ) 
  std::cout << "the boundaries are wrong " << std::endl;*/

  

  return bin_extrapol;
}




void buildHybrid( MT2Analysis<MT2Estimate>* shape_hybrid, MT2Analysis<MT2Estimate>* shape_data, MT2Analysis<MT2Estimate>* shape_MCsr, MT2Analysis<MT2Estimate>* shape_MCcr, MT2Analysis<MT2Estimate>* shape_MCcr_forExtremeHT, MT2Analysis<MT2Estimate>* bin_extrapol ) {
  
  bool getBinByHand = true; //this flag is only introduced for debugging purpose. Always turn it to false for normal routine

  std::set<MT2Region> regions       = shape_data->getRegions();
    
  bool writeToExtFile = true;
  ofstream writeToFile("file_hybridShape.txt");
  if(writeToFile.fail()){
     cerr << "Erreur lors de l'ouverture du fichier" << endl;
     return;
  }
  else{
    writeToFile << "Notation: " << endl;
    writeToFile << "[1] last bin + sufficient MC_cr statistics" << endl;
    writeToFile << "[2] not last bin + sufficient MC_cr statistics" << endl;
    writeToFile << "[3] not enough MC_cr statistics" << endl;
    writeToFile << "[above] bin above extrapolation bin" << endl;
    writeToFile << "[below] bin below extrapolation bin" << endl;
    writeToFile << "-----------------------------------------------" << endl << endl << endl;
  }
 
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {
   
    MT2Region* region = new MT2Region( *iR );
       
    if(writeToExtFile){ 
      writeToFile << endl << "--------------------------------------------------" << endl;
      writeToFile << endl << region->getName() << endl;
    }

    TH1D* this_shape_data = (TH1D*)shape_data ->get( *iR)->yield;
    TH1D* this_shape_MCsr = (TH1D*)shape_MCsr ->get( *iR)->yield;

    //At extreme HT we get the shape from MC after MT2 400 per TR
    TH1D* this_shape_MCcr;
    if( (region->nJetsMin() > 1) && (region->htMin()==1500) ){
      this_shape_MCcr = (TH1D*)shape_MCcr_forExtremeHT ->get( *iR)->yield;
    }
    else{
      this_shape_MCcr = (TH1D*)shape_MCcr ->get( *iR)->yield;     
    }

    TH1D* this_shape_hybrid  = (TH1D*)shape_hybrid ->get( *iR)->yield;
    TH1D* this_binExtrapol   = (TH1D*)bin_extrapol ->get( *iR)->yield;

    int nBins = this_shape_data->GetNbinsX();
    //for each topo region will have a bin number indicating where we extrapolate
    int bin_extrapol = 1;
    int bin_extrapol_fixed = 1;
    double integral = 0.;
    double errData = 0.;
    double integralMC = 0.;
    double errMC = 0.;
 
   
    bool getExtrapolBin = 0;

    if(getExtrapolBin){

      if(region->nJetsMax()==1){ 
	bin_extrapol = 2;
      }
      else{

	bin_extrapol = getFixedExtrapolBin( region, this_shape_data );

	if( bin_extrapol == nBins ){ //We take the full shape from data!
	  bin_extrapol = bin_extrapol; //bin_extrapol+1; removed the +1, as it is already taken into account in te getFixedExtrapolBin function
	  integral = 1.;//we don't have to do a special normalization in this case
	  errData  = 0.;
	  integralMC = this_shape_MCcr->IntegralAndError( bin_extrapol, -1, errMC);
	}else{
	  bin_extrapol = bin_extrapol;
	  integral = this_shape_data->IntegralAndError( bin_extrapol, -1, errData);
	  integralMC = this_shape_MCcr->IntegralAndError( bin_extrapol, -1, errMC);
	}
      }
    }else{
      //if(region->getName() != "HT575to1200_j4to6_b0" && region->getName() != "HT575to1200_j4to6_b1" && region->getName() != "HT575to1200_j4to6_b2"){
      if(!getBinByHand){
	cout << region->getName() << ": OK" << endl;
	if(writeToExtFile){
	  writeToFile << "Maximum number of events in MC CR: " <<  this_shape_MCcr->Integral(1, -1) << endl;
	}
	for( int iBin=nBins; iBin>= 1; iBin-- ){
	  if( this_shape_MCcr->Integral( iBin, -1) >= 50. ){
	    if( iBin == nBins ){ //We take the full shape from data!
	      bin_extrapol = iBin+1;
	      integral = 1.;//we don't have to do a special normalization in this case
	      errData  = 0.;
	      integralMC = this_shape_MCcr->IntegralAndError( iBin, -1, errMC);
	      if(writeToExtFile){
		writeToFile << "[1] integral: " << integral << endl;
		writeToFile << "[1] integralMC: " << integralMC << endl;
	      }
	    }else{
	      bin_extrapol = iBin;
	      integral = this_shape_data->IntegralAndError( iBin, -1, errData);
	      integralMC = this_shape_MCcr->IntegralAndError( iBin, -1, errMC);
	      if(writeToExtFile){
		writeToFile << "[2] integral: " << integral << endl;
		writeToFile << "[2] integralMC: " << integralMC << endl;
	      }
	    }
	    break;
	  }
	  else{	
	    bin_extrapol = 1;
	    integralMC = this_shape_MCcr->IntegralAndError( bin_extrapol, -1, errMC);
	    integral   = this_shape_data->IntegralAndError( bin_extrapol, -1, errData);
	    if(writeToExtFile){
	      writeToFile << "[3] integral: " << integral << endl;
	      writeToFile << "[3] integralMC: " << integralMC << endl;
	    }
	  }
	}
      }
      else{//we get bin extrapol by hand
	cout << region->getName() << ": special treatment" << endl;
	bin_extrapol = 1;
	integralMC = this_shape_MCcr->IntegralAndError( bin_extrapol, -1, errMC);
	integral   = this_shape_data->IntegralAndError( bin_extrapol, -1, errData);

      }
    }//got the extrapol bin myself


    //as a check now, compute bin_extrapol fixed to compare it with other method
    if( region->nJetsMax()==1){ 
	bin_extrapol_fixed = 2;
    }
    else{
	bin_extrapol_fixed = getFixedExtrapolBin( region, this_shape_data );

	if( bin_extrapol_fixed == nBins ){ 
	  bin_extrapol_fixed = bin_extrapol_fixed+1;
	}else{
	  bin_extrapol_fixed = bin_extrapol_fixed;
	}
    }
    
    if(writeToExtFile){
      //writeToFile << "Check: bin_extrapol / bin_extrapol_fixed / nBins :  " << bin_extrapol << " / " << bin_extrapol_fixed << " / " << nBins << endl;
      writeToFile << "bin_extrapol / nBins :  " << bin_extrapol << " / " << nBins << endl << endl;
    }
   
    //Filling the histo that knows where we extrapolate
    this_binExtrapol->SetBinContent( 1, bin_extrapol );

    double errZinv;
    double integralZinv = this_shape_MCsr->IntegralAndError( bin_extrapol, -1, errZinv);
    double relativeErrZinv = 1.0;
    if(integralZinv>0){
      relativeErrZinv = errZinv/integralZinv;
    }

    double relativeErrData = 1.0;
    if(integral>0){
      relativeErrData = errData/integral;
    }

    double errShapeExt = sqrt(relativeErrData*relativeErrData+relativeErrZinv*relativeErrZinv);

    double relativeErrMC = 1.0;
    if(integralMC>0){
      relativeErrMC = errMC/integralMC;
    }

    double errShapeMCExt = sqrt(relativeErrMC*relativeErrMC+relativeErrZinv*relativeErrZinv);
  
    for(int iBin=1; iBin<= nBins; iBin++){
      double MCsr_cont;
      double MCcr_cont;
      double MCsr_contErr;
      double MCcr_contErr;
      double MCsr_forkMT2;

      if(iBin < bin_extrapol){

	MCsr_cont = this_shape_MCsr->GetBinContent(iBin);
	MCcr_cont = this_shape_MCcr->GetBinContent(iBin);
	MCsr_contErr = this_shape_MCsr->GetBinError(iBin);
	MCcr_contErr = this_shape_MCcr->GetBinError(iBin);

	MCsr_forkMT2 = MCsr_cont;

	if(writeToExtFile){
	  writeToFile << "Bin " << iBin << ":" << endl;
	  writeToFile << "[below] MC_sr_cont: " << MCsr_cont << endl;
	  writeToFile << "[below] MC_cr_cont: " << MCcr_cont << endl;
	}	 

      }
      else{

	MCsr_cont = integralZinv;
	MCcr_cont = integralMC;
	MCsr_contErr = errZinv;
	MCcr_contErr = errMC;

	MCsr_forkMT2 = this_shape_MCsr->GetBinContent(iBin);

	if(writeToExtFile){
	  writeToFile << "Bin " << iBin << ":" << endl;
	  writeToFile << "[above] MC_sr_cont: " << MCsr_cont << endl;
	  writeToFile << "[above] MC_cr_cont: " << MCcr_cont << endl;
	}

      }

      double ratioMC_cont = MCsr_cont/MCcr_cont;
      double ratioMC_err  = sqrt( (MCsr_contErr/MCcr_cont)*(MCsr_contErr/MCcr_cont) + (MCsr_cont*MCcr_contErr/(MCcr_cont*MCcr_cont))*(MCsr_cont*MCcr_contErr/(MCcr_cont*MCcr_cont)) );

      

      if (ratioMC_cont>0)
        ratioMC_err /= ratioMC_cont;

      float relativeErrorData;
      float relativeErrorMC;

      double kMT2_zinv = MCsr_forkMT2/integralZinv;

      if( iBin<bin_extrapol ){
	
	relativeErrorData = sqrt( (this_shape_data->GetBinError(iBin)*this_shape_data->GetBinError(iBin))/(this_shape_data->GetBinContent(iBin)*this_shape_data->GetBinContent(iBin)) +  ratioMC_err*ratioMC_err );
	relativeErrorMC = sqrt( (this_shape_MCcr->GetBinError(iBin)*this_shape_MCcr->GetBinError(iBin))/(this_shape_MCcr->GetBinContent(iBin)*this_shape_MCcr->GetBinContent(iBin)) +  ratioMC_err*ratioMC_err );

	if(writeToExtFile){
	  writeToFile << "[below] this_shape_data avant: " << this_shape_data->GetBinContent(iBin) << endl;
	  writeToFile << "Then this_shape_data computed as this_shape_data->GetBinContent(iBin)*(MCsr_cont/MCcr_cont)" << endl;
	}

	this_shape_data ->SetBinContent(iBin, this_shape_data->GetBinContent(iBin)*ratioMC_cont);
	this_shape_MCcr ->SetBinContent(iBin, this_shape_MCcr->GetBinContent(iBin)*ratioMC_cont);
	this_shape_data ->SetBinError(iBin, this_shape_data->GetBinContent(iBin)*relativeErrorData);
	this_shape_MCcr ->SetBinError(iBin, this_shape_MCcr->GetBinContent(iBin)*relativeErrorMC);
	
	if(writeToExtFile){
	  writeToFile << "[below] this_shape_data apres: " << this_shape_data->GetBinContent(iBin) << endl;
	}

      }else{

	relativeErrorData = sqrt( errShapeExt*errShapeExt + ratioMC_err*ratioMC_err );
        relativeErrorMC = sqrt( errShapeMCExt*errShapeMCExt + ratioMC_err*ratioMC_err );

	if(writeToExtFile){
	  writeToFile << "[above] integral: " << integral << endl;
	  writeToFile << "[above] ratioMC_cont: " << ratioMC_cont << endl;
	  writeToFile << "[above] kMT2_zinv: " << kMT2_zinv << endl;
	  writeToFile << "[above] integralZinv: " << integralZinv << endl;
	}

	if(writeToExtFile){
	  writeToFile << "[above] this_shape_data avant: " << this_shape_data->GetBinContent(iBin) << endl;
	  writeToFile << "Then this_shape_data computed as this_shape_data<-integral*ratioMC_cont*kMT2_zinv" << endl;
	}

        this_shape_data ->SetBinContent(iBin, integral*ratioMC_cont*kMT2_zinv);
        this_shape_MCcr ->SetBinContent(iBin, integralMC*ratioMC_cont*kMT2_zinv);
        this_shape_data ->SetBinError(iBin, relativeErrorData*this_shape_data->GetBinContent(iBin));
        this_shape_MCcr ->SetBinError(iBin, relativeErrorMC*this_shape_MCcr->GetBinContent(iBin));

	if(writeToExtFile){
	  writeToFile << "[above] this_shape_data apres: " << this_shape_data->GetBinContent(iBin) << endl;
	}

      }      
      
      this_shape_hybrid->SetBinContent(iBin, this_shape_data->GetBinContent(iBin) );    
      this_shape_hybrid->SetBinError  (iBin, this_shape_data->GetBinError(iBin) );
      
      if(writeToExtFile){
	writeToFile << "zllHybrid_shape_TR filled with (this_shape_data): " << this_shape_hybrid->GetBinContent(iBin) << endl;
	writeToFile << "and error: " <<  this_shape_hybrid->GetBinError(iBin) << endl << endl;
      }
    }

    //And now it has to be normalized if there is at least one event
    this_shape_MCcr ->Scale( 1./this_shape_MCcr->Integral());
    
    if(this_shape_data->Integral() != 0){
      this_shape_data   ->Scale( 1./this_shape_data->Integral());
      this_shape_hybrid ->Scale( 1./this_shape_hybrid->Integral());
      if(writeToExtFile){
	for(int iBin=1; iBin<= nBins; iBin++){
	  writeToFile << "In bin " << iBin << ", normalized hybrid_shape is: " << this_shape_hybrid->GetBinContent(iBin) << endl;
	}
      }
    }else{
      if(writeToExtFile){
	writeToFile << "Hybrid shape filled with 0, and not normalized to avoid propagation of nan" << endl;
      }
    }
  
    if( nBins == 1) this_shape_hybrid->SetBinError( 1, 0.0 );

  
  }//end loop over final estimate loops

  return;

}



