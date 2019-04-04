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


  //we save in the output file
  std::string outFile = cfg.getEventYieldDir() + "/zinvFromZll.root";

  zllData_shape	              ->addToFile(outFile);				  			     
  zllMC_shape	              ->addToFile(outFile);
  zllMC_shape_TR              ->addToFile(outFile);
  zllMC_shape_TR_forTest      ->addToFile(outFile);
  purity_forHybrid            ->setName("purity_forHybrid");
  purity_forHybrid            ->addToFile(outFile);	
  bin_extrapol                ->addToFile(outFile);  
  ZinvZllRatioHybrid          ->addToFile(outFile); 
  ZinvEstimateFromZll_hybrid  ->addToFile(outFile);
  zllData_forHybrid           ->addToFile(outFile);
  zllData_shape_TR            ->addToFile(outFile);
  zllData_shape_TR_forTest    ->addToFile(outFile);
  zinvMC_forShape             ->addToFile(outFile);
  zinvMC_forShape_TR          ->addToFile(outFile);
  zinvMC_forShape_TR_forTest  ->addToFile(outFile);
  zllHybrid_shape_TR          ->addToFile(outFile);
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
	//purity = (contentSF - R_sfof * contentOF)/ contentSF;
	purity = 1.0;
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

  if(region->htMin()==250 && region->htMax()==450 ){
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
    std::cout << "the boundaries are wrong " << std::endl;

  return bin_extrapol;
}





void buildHybrid( MT2Analysis<MT2Estimate>* shape_hybrid, MT2Analysis<MT2Estimate>* shape_data, MT2Analysis<MT2Estimate>* shape_MCsr, MT2Analysis<MT2Estimate>* shape_MCcr, MT2Analysis<MT2Estimate>* shape_MCcr_forExtremeHT, MT2Analysis<MT2Estimate>* bin_extrapol ) {
  
  std::cout << endl << "In build hybrid:" << std::endl;

  std::set<MT2Region> regions       = shape_data->getRegions();
  
  int n(1);
  
  bool writeToExtFile = false;

  ofstream writeToFile("file_bin_extrapol.txt");
  if(writeToFile.fail()){
     cerr << "Erreur lors de l'ouverture du fichier" << endl;
     return;
  }
 
 
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {
   
    MT2Region* region = new MT2Region( *iR );
   
    cout << endl << "Region: " << region->getName() << endl;
    
    if(writeToExtFile){
      writeToFile << endl << "Region " << n << endl;
      writeToFile << region->getNiceNames()[0] << " " << region->getNiceNames()[1] << endl;
    ++n;
    }


    TH1D* this_shape_data    = (TH1D*)shape_data   ->get( *iR)->yield;
    TH1D* this_shape_MCsr    = (TH1D*)shape_MCsr   ->get( *iR)->yield;

    //At extreme HT we get the shape from MC after MT2 400 per TR
    TH1D* this_shape_MCcr;
    if( (region->nJetsMin() > 1) && (region->htMin()==1500) ){
      this_shape_MCcr = (TH1D*)shape_MCcr_forExtremeHT->get( *iR)->yield;
    }
    else{
      this_shape_MCcr = (TH1D*)shape_MCcr             ->get( *iR)->yield;     
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

    std::vector< std::string > niceNames = region->getNiceNames();
   
   
    bool getExtrapolBin = 0;
    if(getExtrapolBin){

      if( region->nJetsMax()==1){ 
	bin_extrapol = 2;
      }
      else{

	bin_extrapol = getFixedExtrapolBin( region, this_shape_data );

	if( bin_extrapol == nBins ){ //We take the full shape from data!
	  bin_extrapol = bin_extrapol+1;
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
      for( int iBin=nBins; iBin>= 1; iBin-- ){
      
	if(writeToExtFile){
	  writeToFile << "Maximum number of statistics in MC CR: " <<  this_shape_MCcr->Integral( iBin, -1) << endl;
	}
	
	if( this_shape_MCcr->Integral( iBin, -1) >= 50. ){
	  if( iBin == nBins ){ //We take the full shape from data!
	    bin_extrapol = iBin+1;
	    integral = 1.;//we don't have to do a special normalization in this case
	    errData  = 0.;
	    integralMC = this_shape_MCcr->IntegralAndError( iBin, -1, errMC);
	    cout << "[7] integral: " << integral << endl;
	    cout << "[7] integralMC: " << integralMC << endl;
	  }else{
	    bin_extrapol = iBin;
	    integral = this_shape_data->IntegralAndError( iBin, -1, errData);
	    integralMC = this_shape_MCcr->IntegralAndError( iBin, -1, errMC);
	    cout << "[6] integral: " << integral << endl;
	    cout << "[6] integralMC: " << integralMC << endl;
	  }
	  break;
	}
	else{	
	  bin_extrapol = 1;
	  integralMC = this_shape_MCcr->IntegralAndError( bin_extrapol, -1, errMC);
	  integral   = this_shape_data->IntegralAndError( bin_extrapol, -1, errData);
	  cout << "[5] integral: " << integral << endl;
	  cout << "[5] integralMC: " << integralMC << endl;

	}
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
    
    //cout << "Check: bin_extrapol / bin_extrapol_fixed / nBins :  " << bin_extrapol << " / " << bin_extrapol_fixed << " / " << nBins << endl;
    if(writeToExtFile){
      writeToFile << "Check: bin_extrapol / bin_extrapol_fixed / nBins :  " << bin_extrapol << " / " << bin_extrapol_fixed << " / " << nBins << endl;
    }
   
    //Filling the histo that knows where we extrapolate
    this_binExtrapol->SetBinContent( 1, bin_extrapol );

    double errZinv;
    double integralZinv = this_shape_MCsr->IntegralAndError( bin_extrapol, -1, errZinv);
    double relativeErrZinv = 1.0;
    if(integralZinv>0)
      relativeErrZinv = errZinv/integralZinv;

    double relativeErrData = 1.0;
    if(integral>0)
      relativeErrData = errData/integral;

    double errShapeExt = sqrt(relativeErrData*relativeErrData+relativeErrZinv*relativeErrZinv);

    double relativeErrMC = 1.0;
    if(integralMC>0)
      relativeErrMC = errMC/integralMC;

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

	cout << "[4] MC_sr_cont: " << MCsr_cont << endl;
	cout << "[4] MC_cr_cont: " << MCcr_cont << endl;
	 

      }
      else{

	MCsr_cont = integralZinv;
	MCcr_cont = integralMC;
	MCsr_contErr = errZinv;
	MCcr_contErr = errMC;

	MCsr_forkMT2 = this_shape_MCsr->GetBinContent(iBin);

	cout << "[3] MC_sr_cont: " << MCsr_cont << endl;
	cout << "[3] MC_cr_cont: " << MCcr_cont << endl;

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

	cout << "[2] this_shape_data avant: " << this_shape_data->GetBinContent(iBin) << endl;

	this_shape_data ->SetBinContent(iBin, this_shape_data->GetBinContent(iBin)*ratioMC_cont);
	this_shape_MCcr ->SetBinContent(iBin, this_shape_MCcr->GetBinContent(iBin)*ratioMC_cont);
	this_shape_data ->SetBinError(iBin, this_shape_data->GetBinContent(iBin)*relativeErrorData);
	this_shape_MCcr ->SetBinError(iBin, this_shape_MCcr->GetBinContent(iBin)*relativeErrorMC);

	cout << "[2] this_shape_data apres: " << this_shape_data->GetBinContent(iBin) << endl;

      }else{

	relativeErrorData = sqrt( errShapeExt*errShapeExt + ratioMC_err*ratioMC_err );
        relativeErrorMC = sqrt( errShapeMCExt*errShapeMCExt + ratioMC_err*ratioMC_err );

	cout << "[1] integral: " << integral << endl;
	cout << "[1] ratioMC_cont: " << ratioMC_cont << endl;
	cout << "[1] kMT2_zinv: " << kMT2_zinv << endl;
	cout << "[1] integralZinv: " << integralZinv << endl;

        this_shape_data ->SetBinContent(iBin, integral*ratioMC_cont*kMT2_zinv);
        this_shape_MCcr ->SetBinContent(iBin, integralMC*ratioMC_cont*kMT2_zinv);
        this_shape_data ->SetBinError(iBin, relativeErrorData*this_shape_data->GetBinContent(iBin));
        this_shape_MCcr ->SetBinError(iBin, relativeErrorMC*this_shape_MCcr->GetBinContent(iBin));

	cout << "[1] this_shape_data: " << this_shape_data->GetBinContent(iBin) << endl;

      }
     
      
      
      this_shape_hybrid->SetBinContent(iBin, this_shape_data->GetBinContent(iBin) );    
      this_shape_hybrid->SetBinError  (iBin, this_shape_data->GetBinError(iBin) );
      
      cout << "zllHybrid_shape_TR filled with: " << this_shape_hybrid->GetBinContent(iBin) << endl;
      cout << "and error: " <<  this_shape_hybrid->GetBinError(iBin) << endl;
    }


    //And now it has to be normalized if there is at least one event
    this_shape_MCcr   ->Scale( 1./this_shape_MCcr->Integral());
    cout << "entries this_shape_data: " << this_shape_data->GetEntries() << endl;
    cout << "entries this_shape_integral: " << this_shape_data->Integral() << endl;
    if(this_shape_data->Integral() != 0){
      this_shape_data   ->Scale( 1./this_shape_data->Integral());
      this_shape_hybrid ->Scale( 1./this_shape_hybrid->Integral());
      cout << "normalized by its integral: " << this_shape_hybrid->Integral() << endl;
    }else{
      cout << "Hybrid shape filled with 0, and not normalized to avoid propagation of nan" << endl;
    }
  
    if( nBins == 1) this_shape_hybrid->SetBinError( 1, 0.0 );

  
  }//end loop over final estimate loops

  return;

}



