// a generalization of computeLostLepton to be run three times for each year
// must be run with three config files

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




void buildHybrid( MT2Analysis<MT2Estimate>* hybrid_llepCR, MT2Analysis<MT2Estimate>* hybrid_shape, MT2Analysis<MT2Estimate>* data_forShape, MT2Analysis<MT2Estimate>* MCsr_forShape, MT2Analysis<MT2Estimate>* MCcr_forShape, MT2Analysis<MT2Estimate>* bin_extrapol );


int main( int argc, char* argv[] ) {


  std::cout << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|           Running computeLostLepton_combined       |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc!=4 ) {
    std::cout << "USAGE: ./computeLostLepton [configFileName16] [configFileName17] [configFileName18]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  std::string configFileName16(argv[1]);
  MT2Config cfg16(configFileName16);

  std::string configFileName17(argv[2]);
  MT2Config cfg17(configFileName17);

  std::string configFileName18(argv[3]);
  MT2Config cfg18(configFileName18);

  TH1::AddDirectory(kTRUE);


  std::string llepControlRegionDir1 = cfg16.getEventYieldDir() + "/llepControlRegion";
  std::string llepControlRegionDir2 = cfg17.getEventYieldDir() + "/llepControlRegion";
  std::string llepControlRegionDir3 = cfg18.getEventYieldDir() + "/llepControlRegion";


  ////////////////////////////////////////////////////////////////
  // Get inputs
  ////////////////////////////////////////////////////////////////

  // Data Control Region
  MT2Analysis<MT2Estimate>* llepCR16 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir1 + "/data.root", "llepCR");
  llepCR16->setName("llepCR16");
  MT2Analysis<MT2Estimate>* llepCR17 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir2 + "/data.root", "llepCR");
  llepCR17->setName("llepCR17");
  MT2Analysis<MT2Estimate>* llepCR18 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir3 + "/data.root", "llepCR");
  llepCR18->setName("llepCR18");


  if( llepCR16==0 || llepCR17==0 || llepCR18==0 ) {
    std::cout << "-> Please run llepControlRegion first. I need to get the llepCR yields from there." << std::endl;
    std::cout << "-> Thank you for your cooperation." << std::endl;
    exit(193);
  }

  // build sum from the first
  //MT2Analysis<MT2Estimate>* llepCR = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir1 + "/data.root", "llepCR");
  MT2Analysis<MT2Estimate>* llepCR = new MT2Analysis<MT2Estimate>( *(llepCR16) );
  llepCR->setName("llepCR");
  (*llepCR) += (*(llepCR17));
  (*llepCR) += (*(llepCR18));

/*
  // MC Control Region
  MT2Analysis<MT2Estimate>* MCcr_ = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir1 + "/mc.root", "llepCR");
  if( MCcr_==0 ) {
    std::cout << "-> Please run llepControlRegion first. I need to get the MC llepCR yields from there." << std::endl;
    std::cout << "-> Thank you for your cooperation." << std::endl;
    exit(193);
  }

  // MC Control Region Integral
  MT2Analysis<MT2Estimate>* MCcr = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir1 + "/mc.root", "llepCR");
  MCcr->setName("llepCR_mc");
  MT2Analysis<MT2Estimate>* MCcr_integral;
  MCcr_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCcr_integral", cfg.regionsSet(), MCcr_ );
  MCcr_integral->setName("llepCR_mc_integral");

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

  // MC Signal Region integral
  MT2Analysis<MT2Estimate>* MCsr_   = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "Top");
  MCsr_->setName("MCsr_");

  MT2Analysis<MT2Estimate>* WJets_ = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr_) += (*(WJets_));

  MT2Analysis<MT2Estimate>* MCsr_integral;
  MCsr_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCsr_integral", cfg.regionsSet(), MCsr_ );


  // MC ratio SR/CR
  MT2Analysis<MT2Estimate>* RatioMC = new MT2Analysis<MT2Estimate>( "llepRatioMC", cfg.regionsSet() );
  (*RatioMC) = ( (*MCsr_integral) / (*MCcr_integral) );


  ////////////////////////////////////////////////////////////////
  // Prepare estimates for Hybrid shape calculation
  ////////////////////////////////////////////////////////////////

  // Hybrid shapes that will be computed
  MT2Analysis<MT2Estimate>* hybrid_shape = new MT2Analysis<MT2Estimate>( "hybrid_shape", cfg.regionsSet() );
  MT2Analysis<MT2Estimate>* hybrid_llepCR = new MT2Analysis<MT2Estimate>( "hybrid_llepCR", cfg.regionsSet() );

  // extrapol bin that will be computed
  MT2Analysis<MT2Estimate>* extrapol_bin = new MT2Analysis<MT2Estimate>( "extrapol_bin", cfg.regionsSet() );

  // MC Signal Region (multiplied by lumi)
  MT2Analysis<MT2Estimate>* MCsr_forShape   = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "Top");
  MCsr_forShape->setName( "MCsr_forShape" );
  MT2Analysis<MT2Estimate>* WJets_forShape = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr_forShape) += (*(WJets_forShape));

  // MC Control Region (multiplied by lumi)
  MT2Analysis<MT2Estimate>*  MCcr_forShape = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir1 + "/mc.root", "llepCR");
  MCcr_forShape->setName( "MCcr_forShape" );

  (*MCsr_forShape) *= cfg.lumi();
  (*MCcr_forShape) *= cfg.lumi();

  // Data control region (multiplied by lumi)
  MT2Analysis<MT2Estimate>* llepCR_data_forShape;
  llepCR_data_forShape  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir1 + "/data.root", "llepCR");



  //Building the hybrid shape
  buildHybrid( hybrid_llepCR, hybrid_shape, llepCR_data_forShape, MCsr_forShape, MCcr_forShape, extrapol_bin );
  // final estimate will be hybrid_llepCR * hybrid_shape

  ////////////////////////////////////////////////////////////////
  // Final estimates and save to output
  ////////////////////////////////////////////////////////////////

  // Lost lepton estimate
  MT2Analysis<MT2Estimate>* llepEstimate = new MT2Analysis<MT2Estimate>( "llepEstimate", cfg.regionsSet() );
  (*llepEstimate) = (*hybrid_llepCR) * (*hybrid_shape);
*/
  std::string outFile = cfg16.getEventYieldDir() + "/llepEstimate";
  outFile += ".root";

  llepCR->writeToFile( outFile, "recreate" );
  //llepCR16->addToFile( outFile );
  //llepCR17->addToFile( outFile );
  //llepCR18->addToFile( outFile );

  //llepEstimate->writeToFile( outFile, "recreate" );
  //hybrid_shape->addToFile( outFile );
  //hybrid_llepCR->addToFile( outFile );
  //extrapol_bin->addToFile( outFile );
  //RatioMC->addToFile( outFile );

  //MCsr->addToFile( outFile );
  //llep_est_integral->addToFile( outFile );
  //llepCR_data_forShape->addToFile( outFile );


  return 0;

}





void buildHybrid( MT2Analysis<MT2Estimate>* hybrid_llepCR, MT2Analysis<MT2Estimate>* hybrid_shape, MT2Analysis<MT2Estimate>* data_forShape, MT2Analysis<MT2Estimate>* MCsr_forShape, MT2Analysis<MT2Estimate>* MCcr_forShape, MT2Analysis<MT2Estimate>* bin_extrapol ) {

  std::set<MT2Region> regions       = data_forShape->getRegions();

  // loop over topo regions
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {
    MT2Region* region = new MT2Region( *iR );

    TH1D* this_data_forShape    = (TH1D*)data_forShape   ->get( *iR)->yield;
    TH1D* this_MCsr_forShape    = (TH1D*)MCsr_forShape   ->get( *iR)->yield;
    TH1D* this_MCcr_forShape    = (TH1D*)MCcr_forShape   ->get( *iR)->yield;
    TH1D* this_hybrid_shape     = (TH1D*)hybrid_shape    ->get( *iR)->yield;
    TH1D* this_hybrid_llepCR    = (TH1D*)hybrid_llepCR   ->get( *iR)->yield;
    //    TH1D* this_hybrid_shape  = (TH1D*)hybrid_shape ->get( *iR)->yield->Clone("hybrid_helpH");
    TH1D* this_binExtrapol   = (TH1D*)bin_extrapol ->get( *iR)->yield;

    int nBins = this_data_forShape->GetNbinsX();

    //for each topo region will have a bin number indicating where we extrapolate
    int bin_extrapol = 1;
    float integralData = 0.;
    double integralMC = 0.;

    std::vector< std::string > niceNames = region->getNiceNames();
    std::cout << niceNames[0] << " " << niceNames[1] << std::endl;
    for( int iBin=nBins; iBin>= 1; iBin-- ){
      // std::cout << this_data_forShape->Integral( iBin, -1)  << std::endl;
      // if( this_data_forShape->Integral( iBin, -1) >= 10. ){
      if( this_MCcr_forShape->Integral( iBin, -1) >= 50. || nBins==1 ){
        if( iBin == nBins ){ //We take the full shape from data!
          bin_extrapol = iBin+1;
          integralData = 1.;    //we don't have to do a special normalization in this case
          integralMC = this_MCcr_forShape->Integral( iBin, -1);
        }else{
          bin_extrapol = iBin;
          integralData = this_data_forShape->Integral( iBin, -1);
          integralMC = this_MCcr_forShape->Integral( iBin, -1);
	      }
	      break;
      }
      else{
        bin_extrapol = 1;
        integralData  = this_data_forShape->Integral( bin_extrapol, -1);
        integralMC    = this_MCcr_forShape->Integral( bin_extrapol, -1);
      }
    } // end loop over mt2 bins

    //Filling the histo that knows where we extrapolate
    this_binExtrapol->SetBinContent( 1, bin_extrapol );

    std::cout << "extrapol bin / total bins= " << bin_extrapol << " / " << nBins << " : " << integralData << std::endl;

    // loop again over mt2 bins
    for(int iBin=1; iBin<= nBins; iBin++){

      float MCsr_cont;
      float MCcr_cont;

      if(iBin<bin_extrapol){
        MCsr_cont = this_MCsr_forShape->GetBinContent(iBin);
        MCcr_cont = this_MCcr_forShape->GetBinContent(iBin);
      }
      else{
        MCsr_cont    = this_MCsr_forShape->GetBinContent(iBin);
        MCcr_cont    = integralMC;
      }

      float ratioMC_cont = MCsr_cont/MCcr_cont;

      //std::cout << "Bin Content " << iBin << ": " << this_MCcr_forShape->GetBinContent(iBin) << " : " << this_MCsr_forShape->GetBinContent(iBin) << " : " << ratioMC_cont << std::endl;

      // this looks bad !
      if(ratioMC_cont > 3){
        std::cout << ratioMC_cont << std::endl;
        ratioMC_cont = 3.;
      }

      if( iBin<bin_extrapol && (bin_extrapol != nBins) ){

        this_hybrid_llepCR->SetBinContent(iBin, this_data_forShape->GetBinContent(iBin));
        this_hybrid_shape ->SetBinContent(iBin, ratioMC_cont);
        //this_MCcr_forShape ->SetBinContent(iBin, this_MCcr_forShape->GetBinContent(iBin)*ratioMC_cont);

      }else{

        this_hybrid_llepCR ->SetBinContent(iBin, integralData);
        this_hybrid_shape ->SetBinContent(iBin, ratioMC_cont);
        //this_MCcr_forShape ->SetBinContent(iBin, integralMC*ratioMC_cont);
      }

      //std::cout << "Bin Content " << iBin << ": " << this_MCcr_forShape->GetBinContent(iBin) << " : "  <<  this_hybrid_llepCR->GetBinContent(iBin) << " : " << this_data_forShape->GetBinContent(iBin) << " : " << this_MCsr_forShape->GetBinContent(iBin) << " : " << integral << " : "  <<  ratioMC_cont << std::endl;

    } // end mt2 bins


  }//end loop over topo regions

  return;

}
