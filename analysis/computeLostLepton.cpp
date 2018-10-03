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

#include "TLine.h"
#include "TAxis.h"

using namespace std;


bool use_extrapolation = false;
//bool use_extrapolation = false;
bool do_dummyMC = false;
bool do_hybrid = true;


void buildHybrid( MT2Analysis<MT2Estimate>* nlepCR, MT2Analysis<MT2Estimate>* shape_hybrid, MT2Analysis<MT2Estimate>* shape_data, MT2Analysis<MT2Estimate>* shape_MCsr, MT2Analysis<MT2Estimate>* shape_MCcr, MT2Analysis<MT2Estimate>* bin_extrapol );
void plotLlepEstimates(MT2Analysis<MT2Estimate>* toGetPlotted, TString saveName, std::set<MT2Region> regions, TString directory);


int main( int argc, char* argv[] ) {


  std::cout << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|           Running computeLostLepton                |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc!=2 ) {
    std::cout << "USAGE: ./computeLostLepton [configFileName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  std::string configFileName(argv[1]);
  MT2Config cfg(configFileName);


  TH1::AddDirectory(kTRUE);


  std::string llepControlRegionDir = cfg.getEventYieldDir() + "/llepControlRegion"; 

  

  ////////////////////////////////////////////////////////////////////////////////
  //            Check if computeLostLepton has been previously run              //
  ////////////////////////////////////////////////////////////////////////////////

   MT2Analysis<MT2Estimate>* ControlRegion;
  if( !do_dummyMC )
    ControlRegion  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/data.root", "llepCR");
  else{
    ControlRegion  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
    (*ControlRegion) *= cfg.lumi();
  }
  if(ControlRegion==0) {
    std::cout << "-> Please run llepControlRegion first. I need to get the llepCR yields from there." << std::endl;
    std::cout << "-> Thank you for your cooperation." << std::endl;
    exit(193);
  }
   

  ////////////////////////////////////////////////////////////////////////////////
  //         Necessary ingredients for non-hybrid background estimation         //
  ////////////////////////////////////////////////////////////////////////////////

 
  // 1. First part of the estimation
   MT2Analysis<MT2Estimate>* llepCR;
  if( !do_dummyMC )
    llepCR  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/data.root", "llepCR");
  else{
    llepCR  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
    (*llepCR) *= cfg.lumi();
  }

  MT2Analysis<MT2EstimateSyst>* llep_est;
  if( !use_extrapolation && !do_hybrid )
    llep_est = MT2EstimateSyst::makeAnalysisFromEstimate( "llep_est", cfg.regionsSet(), llepCR );


  // 2. Build the SR/CR ratio

  // MC Control Region
  MT2Analysis<MT2Estimate>* MCcr = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
  if( MCcr==0 ) {
   std::cout << "-> Please run llepControlRegion on mc first. I need to get the MC llepCR yields from there." << std::endl;
   std::cout << "-> Thank you for your cooperation." << std::endl;
   exit(193);
  }

  // MC Signal Region
  MT2Analysis<MT2Estimate>* Top   = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "Top");
  MT2Analysis<MT2Estimate>* WJets = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "WJets");
  if( Top==0 || WJets==0 ) {
    std::cout << "-> Please run regionEventYields on MC first. I need to get the Top and W+jets MC yields from there." << std::endl;
    std::cout << "-> Thank you for your cooperation." << std::endl;
    exit(197);
  }
  MT2Analysis<MT2Estimate>* MCsr = new MT2Analysis<MT2Estimate>( *(Top) );
  (*MCsr) += (*(WJets));
  MCsr->setName("Top + W+jets");

  // MC ratio
  MT2Analysis<MT2Estimate>* RatioMC = new MT2Analysis<MT2Estimate>( "llepRatioMC", cfg.regionsSet() );
  if( !use_extrapolation && !do_hybrid )
    (*RatioMC) = ( (*MCsr) / (*MCcr) );

  // The final lost lepton background estimate is the product between 1. and 2. (implemented later)



 // MC Control Region
  //For me this is completely useless
  // MT2Analysis<MT2Estimate>* MCcr_ = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
  //if( MCcr_==0 ) {
  // std::cout << "-> Please run llepControlRegion first. I need to get the MC llepCR yields from there." << std::endl;
  // std::cout << "-> Thank you for your cooperation." << std::endl;
  // exit(193);
  // }

  // MC Control Region
  // MT2Analysis<MT2Estimate>* MCcr = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
 
  // an estimate with all bins equal to the integral of the CR MC  
  // check if this is needed, otherwise remove
  // MT2Analysis<MT2Estimate>* MCcr_integral;
  //if( use_extrapolation || do_hybrid ){
  // MCcr_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCcr_integral", cfg.regionsSet(), MCcr_ );
  // MCcr_integral->setName("llepCR_mc_integral");
  // }


  ////////////////////////////////////////////////////////////////////////////////
  //         Necessary ingredients for hybrid background estimation             //
  ////////////////////////////////////////////////////////////////////////////////

  // this SR object will be necessary to build the ratio between SR and CR region events
  MT2Analysis<MT2Estimate>* MCsr_forShape   = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "Top");
  MCsr_forShape->setName( "MCsr_forShape" );
  MT2Analysis<MT2Estimate>* WJets_forShape = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr_forShape) += (*(WJets_forShape));
  
  // it is on this object that we will check if there is enough statistics to use data, or switch to MC
  MT2Analysis<MT2Estimate>*  MCcr_forShape = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
  MCcr_forShape->setName( "MCcr_forShape" );
  (*MCsr_forShape) *= cfg.lumi();
  (*MCcr_forShape) *= cfg.lumi();
  
  ////  MT2Analysis<MT2Estimate>* RatioMC_forShape = new MT2Analysis<MT2Estimate>( "llepRatioMC_forShape", cfg.regionsSet() );
  ////  (*RatioMC_forShape) = ( (*MCsr_forShape) / (*MCcr_forShape) );
 
  // this is the bin that will set the limit between data and MC regions in hybrid shape
  MT2Analysis<MT2Estimate>* extrapol_bin = new MT2Analysis<MT2Estimate>( "extrapol_bin", cfg.regionsSet() );

  MT2Analysis<MT2Estimate>* llepCR_data_forShape; // will be renamed later on data_shape
  if(!do_dummyMC)
    llepCR_data_forShape  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/data.root", "llepCR");
  else{
    llepCR_data_forShape  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
    (*llepCR_data_forShape) *= cfg.lumi();
  }
  
  // two very important ingredient since their product give the llep background estimate
  MT2Analysis<MT2Estimate>* hybrid_shape = new MT2Analysis<MT2Estimate>("hybrid_shape", cfg.regionsSet());
  MT2Analysis<MT2Estimate>* hybrid_llepCR = new MT2Analysis<MT2Estimate>("hybrid_llepCR", cfg.regionsSet());

  //Building the hybrid shape from data & MC, and the ratio of the MC
  buildHybrid(hybrid_llepCR, hybrid_shape, llepCR_data_forShape, MCsr_forShape, MCcr_forShape, extrapol_bin);

  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////

  /*MT2Analysis<MT2Estimate>* MCsr_   = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "Top");
  MCsr_->setName("MCsr_");

  MT2Analysis<MT2Estimate>* WJets_ = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr_) += (*(WJets_));*/
  // is all of this really useful?
  // MT2Analysis<MT2Estimate>* MCsr_integral;
  //if( do_hybrid ){ // why in do_hybrid since the integral is here made only with data?
  // MCsr_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCsr_integral", cfg.regionsSet(), MCsr_ );
    //// MCsr_integral->setName("llepSR_mc_integral");
  // }

  // MC ratio SR/CR
  // MT2Analysis<MT2Estimate>* RatioMC = new MT2Analysis<MT2Estimate>( "llepRatioMC", cfg.regionsSet() );
  //if( !use_extrapolation && !do_hybrid )
  // (*RatioMC) = ( (*MCsr) / (*MCcr) );
  // else if( use_extrapolation && !do_hybrid )
  // (*RatioMC) = ( (*MCsr) / (*MCcr_integral) );
  // else if( do_hybrid ){
  // (*RatioMC) = ( (*MCsr_integral) / (*MCcr_integral) );//in the end we will not use this one, since the ration is directly implemented in the buildHybrid function
  // }
 
  // Why do you use a syst estimate here ? ->to get up and down integrals (that differ from upper or lower error estimate in the bin content)
  //MT2Analysis<MT2EstimateSyst>* llep_est_ = MT2EstimateSyst::makeAnalysisFromEstimate( "llep_est_", cfg.regionsSet(), llepCR );

  /*
  //lepton estimation without hybrid treatment:
   MT2Analysis<MT2Estimate>* llepCR;
  if( !do_dummyMC )
    llepCR  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/data.root", "llepCR");
  else{
    llepCR  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
    (*llepCR) *= cfg.lumi();
  }

  MT2Analysis<MT2EstimateSyst>* llep_est;
  if( !use_extrapolation && !do_hybrid )
  llep_est = MT2EstimateSyst::makeAnalysisFromEstimate( "llep_est", cfg.regionsSet(), llepCR );*/

  //I still dont see why the llep background estimation has also been implemented this way
  // MT2Analysis<MT2EstimateSyst>* llep_est_integral;
  //if( use_extrapolation || do_hybrid )
  // llep_est_integral = MT2EstimateSyst::makeIntegralAnalysisFromEstimate( "llep_est_integral", cfg.regionsSet(), llep_est_ );


  ////////////////////////////////////////////////////////////////////////////////
  //                      Lost lepton background estimate                       //
  ////////////////////////////////////////////////////////////////////////////////

  //We finaly compute the Lost Lepton Background Estimate
  MT2Analysis<MT2Estimate>* llepEstimate = new MT2Analysis<MT2Estimate>( "llepEstimate", cfg.regionsSet() );

  if( !use_extrapolation && !do_hybrid)
    (*llepEstimate) = (* (MT2Analysis<MT2Estimate>*)llep_est) * (*RatioMC);
  // else if( use_extrapolation )
  // (*llepEstimate) = (* (MT2Analysis<MT2Estimate>*)llep_est_integral) * (*RatioMC);
//  else if( do_hybrid )
//    (*llepEstimate) = (* (MT2Analysis<MT2Estimate>*)llep_est_integral) * (*RatioMC) * (*hybrid_shape);
  else if( do_hybrid )
    (*llepEstimate) = (*hybrid_llepCR) * (*hybrid_shape);

  //this is not needed neither
  // MT2Analysis<MT2Estimate>* alpha= new MT2Analysis<MT2Estimate>( "alpha", cfg.regionsSet() );
//  //if( do_hybrid )
//    //(*alpha) =  (*RatioMC) * (*hybrid_shape);
  //if( do_hybrid )
  // (*alpha) =  (*hybrid_shape);

 

  ////////////////////////////////////////////////////////////////////////////////
  //                      save in llepEstimate.root                             //
  ////////////////////////////////////////////////////////////////////////////////

  std::string outFile = cfg.getEventYieldDir() + "/llepEstimate";
  outFile += ".root";

  llepEstimate->writeToFile( outFile, "recreate" );
  RatioMC->addToFile( outFile );
  MCsr->addToFile( outFile );
  if( !use_extrapolation && !do_hybrid ){
    llep_est->addToFile( outFile );
    MCcr->addToFile( outFile );
  }
  else if( use_extrapolation ){
    //llep_est_integral->addToFile( outFile );
    // MCcr_integral->addToFile( outFile );
  }else if( do_hybrid ){
    //llep_est_integral->addToFile( outFile );
    hybrid_shape->addToFile( outFile );
    hybrid_llepCR->addToFile( outFile );
    llepCR_data_forShape->setName("data_shape");
    llepCR_data_forShape->addToFile( outFile );
    MCsr_forShape->addToFile( outFile );
    MCcr_forShape->addToFile( outFile );
    extrapol_bin->addToFile( outFile );
    // alpha->addToFile( outFile );
  }
  

  ////////////////////////////////////////////////////////////////////////////////
  //                      Plot of the estimates yields                          //
  ////////////////////////////////////////////////////////////////////////////////
  
  std::set<MT2Region> regions = llepCR_data_forShape->getRegions();
 
  plotLlepEstimates(llepEstimate, "llepEstimate", regions,  cfg.getEventYieldDir());
  plotLlepEstimates(RatioMC, "RatioMC", regions,  cfg.getEventYieldDir());
  plotLlepEstimates(MCsr, "MCsr", regions,  cfg.getEventYieldDir());
  plotLlepEstimates(hybrid_llepCR, "hybrid_llepCR", regions,  cfg.getEventYieldDir());
  plotLlepEstimates(hybrid_shape, "hybrid_shape", regions,  cfg.getEventYieldDir());
  plotLlepEstimates(llepCR_data_forShape, "llepCR_data_forShape", regions,  cfg.getEventYieldDir());
  plotLlepEstimates(MCcr_forShape, "MCcr_forShape", regions,  cfg.getEventYieldDir());
  plotLlepEstimates(MCsr_forShape, "MCsr_forShape", regions,  cfg.getEventYieldDir());
  plotLlepEstimates(extrapol_bin, "extrapol_bin", regions,  cfg.getEventYieldDir());
  

  return 0;

}



void buildHybrid( MT2Analysis<MT2Estimate>* nlepCR, MT2Analysis<MT2Estimate>* shape_hybrid, MT2Analysis<MT2Estimate>* shape_data, MT2Analysis<MT2Estimate>* shape_MCsr, MT2Analysis<MT2Estimate>* shape_MCcr, MT2Analysis<MT2Estimate>* bin_extrapol ) {
  // **Terminology**: - hybrid shape: we merge all the last bins together until there are at least 50 events in total. We estimate the number of yields from MC sample on that part ; from data sample for the rest of the distribution 
  //                  - bin_extrapol: bin above which the tail of the distribution has a minimum of 50 events. 
  //                  - first part of the hybrid shape =  bins below bin_extrapol -> data part
  //                  - second part =  bins over bin_extrapol -> MC part


  //we are going to loop over all the topological regions
  std::set<MT2Region> regions = shape_data->getRegions();

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {
    MT2Region* region = new MT2Region( *iR );

    //we create a yield histogram for each variable of interest
    TH1D* this_shape_data    = (TH1D*)shape_data   ->get( *iR)->yield;
    TH1D* this_shape_MCsr    = (TH1D*)shape_MCsr   ->get( *iR)->yield;
    TH1D* this_shape_MCcr    = (TH1D*)shape_MCcr   ->get( *iR)->yield;
    TH1D* this_shape_hybrid  = (TH1D*)shape_hybrid ->get( *iR)->yield;
    TH1D* this_nlepCR        = (TH1D*)nlepCR       ->get( *iR)->yield;
    //TH1D* this_shape_hybrid  = (TH1D*)shape_hybrid ->get( *iR)->yield->Clone("hybrid_helpH");
    TH1D* this_binExtrapol   = (TH1D*)bin_extrapol ->get( *iR)->yield;

    //the maximum number of bins is the one of data
    int nBins = this_shape_data->GetNbinsX();
    
    //for each topo region will have a bin number indicating where we extrapolate
    int bin_extrapol = 1;
    float integral = 0.;
    double integralMC = 0.;
    double errMC = 1.;

    std::vector< std::string > niceNames = region->getNiceNames();
    std::cout << niceNames[0] << " " << niceNames[1] << std::endl;

    //start hybrid implementation here

    //we begin with the highest bin
    for( int iBin=nBins; iBin>= 1; iBin-- ){
      // std::cout << this_shape_data->Integral( iBin, -1)  << std::endl;
      // if( this_shape_data->Integral( iBin, -1) >= 10. ){

      if( this_shape_MCcr->Integral( iBin, -1) >= 50. || nBins==1 ){
	if( iBin == nBins ){ //We take the full shape from data!
	  bin_extrapol = iBin+1;
	  integral = 1.;    //we don't have to do a special normalization in this case
	  integralMC = this_shape_MCcr->IntegralAndError( iBin, -1, errMC);
	}else{
	  bin_extrapol = iBin;
	  integral = this_shape_data->Integral( iBin, -1); //this is like we have merged all the upper bins together
	  integralMC = this_shape_MCcr->IntegralAndError( iBin, -1, errMC);//this quantity corresponds to the number of events in the MC distribution -> one used in the hybrid shape in the >bin_extrapol part
	}
	break;//once we have found bin_extrapol, we stop the loop over lower bins
      }
      else{//if there is not enough statistics
	bin_extrapol = 1; //we havent found the turning point yet
	integral  = this_shape_data->Integral( bin_extrapol, -1); // this is needed in case there are less than 50 events in the whole distribution
	integralMC = this_shape_MCcr->IntegralAndError( bin_extrapol, -1, errMC);
      }
    }

    //at this point we have found bin_extrapol and the number of events for the MC part

    //Filling the histo that knows where we extrapolate
    this_binExtrapol->SetBinContent( 1, bin_extrapol );
    
    std::cout <<  "extrapol bin / total bins= " << bin_extrapol << " / " << nBins << " : " << integral << std::endl;

    for(int iBin=1; iBin<= nBins; iBin++){
      //variables needed to build the ration MCsr/MCcr
      float MCsr_cont;
      float MCcr_cont;
      float MCsr_contErr;
      float MCcr_contErr;
      if(iBin<bin_extrapol){ // first part of the hybrid shape (data part)
	MCsr_cont = this_shape_MCsr->GetBinContent(iBin);
	MCsr_contErr = this_shape_MCsr->GetBinError(iBin);
	MCcr_cont = this_shape_MCcr->GetBinContent(iBin); 
	MCcr_contErr = this_shape_MCcr->GetBinError(iBin);
      }
      else{ //MC part
	MCsr_cont    = this_shape_MCsr->GetBinContent(iBin);
	MCsr_contErr = this_shape_MCsr->GetBinError(iBin);
	MCcr_cont = integralMC;
	MCcr_contErr = errMC;
      }
      
      // we build the ratio and its error
      float ratioMC_cont = MCsr_cont/MCcr_cont;
      float ratioMC_err  = sqrt( (MCsr_contErr/MCcr_cont)*(MCsr_contErr/MCcr_cont) + (MCsr_cont*MCcr_contErr/(MCcr_cont*MCcr_cont))*(MCsr_cont*MCcr_contErr/(MCcr_cont*MCcr_cont)) );

      if (ratioMC_cont>0)
	ratioMC_err /= ratioMC_cont;

      float relativeErrorData;
      float relativeErrorMC;
      
      std::cout << "Bin Content " << iBin << ": " << this_shape_MCcr->GetBinContent(iBin) << " : " << this_shape_MCsr->GetBinContent(iBin) << " : " << ratioMC_cont << std::endl;

      if(ratioMC_cont > 3){
	std::cout << ratioMC_cont << std::endl;
	ratioMC_cont = 3.;
      }
      
      if( iBin<bin_extrapol && (bin_extrapol != nBins) ){ //data part

	relativeErrorData = sqrt( this_shape_data->GetBinError(iBin)*this_shape_data->GetBinError(iBin)/(this_shape_data->GetBinContent(iBin)*this_shape_data->GetBinContent(iBin)) + ratioMC_err*ratioMC_err );
	relativeErrorMC = sqrt( this_shape_MCcr->GetBinError(iBin)*this_shape_MCcr->GetBinError(iBin)/(this_shape_MCcr->GetBinContent(iBin)*this_shape_MCcr->GetBinContent(iBin)) + ratioMC_err*ratioMC_err );

	//we set the bin content of data in the CR (first ingredient to get llep estimate)
	this_nlepCR->SetBinContent(iBin, this_shape_data->GetBinContent(iBin));
	this_nlepCR->SetBinError(iBin, this_shape_data->GetBinError(iBin));
	
	//we set the bin content of the sr/cr ratio in shape_data
	this_shape_data ->SetBinContent(iBin, ratioMC_cont);
	this_shape_data ->SetBinError(iBin, ratioMC_cont*ratioMC_err);

	////	this_shape_data ->SetBinContent(iBin, this_shape_data->GetBinContent(iBin)*ratioMC_cont);
	////	this_shape_data ->SetBinError(iBin, relativeErrorData*this_shape_data->GetBinContent(iBin));

	//shape_MCcr already multiplied by ratio
	this_shape_MCcr ->SetBinContent(iBin, this_shape_MCcr->GetBinContent(iBin)*ratioMC_cont);
	this_shape_MCcr ->SetBinError(iBin, relativeErrorMC*this_shape_MCcr->GetBinContent(iBin));
     
      }else{ //MC part

	relativeErrorData = sqrt( 1/sqrt(integral)*1/sqrt(integral) + ratioMC_err*ratioMC_err );
	relativeErrorMC = sqrt( 1/sqrt(integralMC)*1/sqrt(integralMC) + ratioMC_err*ratioMC_err );

	this_nlepCR ->SetBinContent(iBin, integral);
	this_nlepCR ->SetBinError(iBin, sqrt(integral));

	this_shape_data ->SetBinContent(iBin, ratioMC_cont);
	this_shape_data ->SetBinError(iBin, ratioMC_cont*ratioMC_err);

//	this_shape_data ->SetBinContent(iBin, integral*ratioMC_cont);
//	this_shape_data ->SetBinError(iBin, relativeErrorData*this_shape_data->GetBinContent(iBin));

	this_shape_MCcr ->SetBinContent(iBin, integralMC*ratioMC_cont);
	this_shape_MCcr ->SetBinError(iBin, relativeErrorMC*this_shape_MCcr->GetBinContent(iBin));
      }

      std::cout << "Bin Content " << iBin << ": " << this_shape_MCcr->GetBinContent(iBin) << " : "  <<  this_nlepCR->GetBinContent(iBin) << " : " << this_shape_data->GetBinContent(iBin) << " : " << this_shape_MCsr->GetBinContent(iBin) << " : " << integral << " : "  <<  ratioMC_cont << std::endl;

    }

    // Now we do have the number of events in the CR with the hybrid shape. This is the first ingredient to ge llepEstimate

//    //And now it has to be normalized
//    this_shape_MCcr  ->Scale( 1./this_shape_MCcr->Integral());
//    this_shape_data->Scale( 1./this_shape_data->Integral());
//
//     //Normalized
//    this_shape_MCcr->Scale(this_shape_data->Integral(bin_extrapol,-1)/this_shape_MCcr->Integral(bin_extrapol,-1) );

    for(int iBin=1; iBin<= nBins; iBin++){

      this_shape_hybrid->SetBinContent(iBin, this_shape_data->GetBinContent(iBin) );
      this_shape_hybrid->SetBinError  (iBin, this_shape_data->GetBinError(iBin) );

    }
//      if( ( bin_extrapol==nBins+1 ) || ( iBin<bin_extrapol && (bin_extrapol != nBins) ) ){
//	this_shape_hybrid->SetBinContent(iBin, this_shape_data->GetBinContent(iBin) );
//	this_shape_hybrid->SetBinError  (iBin, this_shape_data->GetBinError(iBin) );
//      }else{
////	this_shape_hybrid->SetBinContent(iBin, this_shape_MCcr->GetBinContent(iBin) );
////	this_shape_hybrid->SetBinError  (iBin, (1./sqrt(integral))*this_shape_MCcr->GetBinContent(iBin) );
//      }
//    }
    if( nBins == 1) this_shape_hybrid->SetBinError( 1, 0.0 );

//    if( this_shape_hybrid->Integral() != 0 ){
//      this_shape_hybrid->Scale( 1./ this_shape_hybrid->Integral() );
//    }

//    this_shape_hybrid_->Reset();
//    for(int iBin = 1; iBin<=nBins; ++iBin){
//      this_shape_hybrid_->SetBinContent(iBin,this_shape_hybrid->GetBinContent(iBin));
//      this_shape_hybrid_->SetBinError  (iBin,this_shape_hybrid->GetBinError(iBin));
//    }
  
  }//end loop over final estimate loops

  return;

}



void plotLlepEstimates(MT2Analysis<MT2Estimate>* toGetPlotted, TString saveName, std::set<MT2Region> regions, TString directory){

  TH1D* hist;
  
  //we fill the histogram with the llepEstimates of all the regions
  for(std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR){
    hist = toGetPlotted->get(*iR)->yield;
  }
  
  TCanvas *c1 = new TCanvas();
  c1->SetLogy(); //y log scale
  c1->SetGrid();
  hist->SetTitle(saveName);
  hist->SetLineColor(4);
  hist->SetLineWidth(2);
  hist->Draw();
 

  TAxis *Xaxis = hist->GetXaxis();
  TAxis *Yaxis = hist->GetYaxis();
  Xaxis->SetTitle("bins");
  Xaxis->SetTitleSize(0.045);
  Xaxis->SetLabelSize(0.045);
  Xaxis->SetTitleOffset(1.1);
  Yaxis->SetTitleSize(0.045);
  Yaxis->SetLabelSize(0.045);
  Yaxis->SetTitleOffset(1.26);

  gStyle->SetOptStat(0);

  c1->SaveAs(directory + "/plotsLlepEstimates/" + saveName + ".pdf");
  c1->SaveAs(directory + "/plotsLlepEstimates/" + saveName + ".png");

}
