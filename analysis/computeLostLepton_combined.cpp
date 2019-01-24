// A generalization of computeLostLepton to estimate the lost lepton background when combining three years
// must be run with three config files
// output will be saved in the dir specified in the first config file

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


// FIXME: remove duplicates hybrid_llepCR16 , 17, 18
// FIXME: remove duplicates extrapol_bin16

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


  std::string llepControlRegionDir16 = cfg16.getEventYieldDir() + "/llepControlRegion";
  std::string llepControlRegionDir17 = cfg17.getEventYieldDir() + "/llepControlRegion";
  std::string llepControlRegionDir18 = cfg18.getEventYieldDir() + "/llepControlRegion";


  ////////////////////////////////////////////////////////////////
  // Get inputs
  ////////////////////////////////////////////////////////////////

  // Data Control Region
  MT2Analysis<MT2Estimate>* dataCR16 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir16 + "/data.root", "llepCR");
  dataCR16->setName("dataCR16");
  MT2Analysis<MT2Estimate>* dataCR17 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir17 + "/data.root", "llepCR");
  dataCR17->setName("dataCR17");
  MT2Analysis<MT2Estimate>* dataCR18 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir18 + "/data.root", "llepCR");
  dataCR18->setName("dataCR18");


  if( dataCR16==0 || dataCR17==0 || dataCR18==0 ) {
    std::cout << "-> Please run llepControlRegion first. I need to get the dataCR yields from there." << std::endl;
    std::cout << "-> Thank you for your cooperation." << std::endl;
    exit(193);
  }

  // build sum from the first
  //MT2Analysis<MT2Estimate>* dataCR = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir16 + "/data.root", "dataCR");
  MT2Analysis<MT2Estimate>* dataCR = new MT2Analysis<MT2Estimate>( *(dataCR16) );
  dataCR->setName("dataCR");
  (*dataCR) += (*(dataCR17));
  (*dataCR) += (*(dataCR18));


  // MC Control Region combined and normalized to lumi - needed for MC stat uncertainty
  MT2Analysis<MT2Estimate>* MCcr16 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir16 + "/mc.root", "llepCR");
  MCcr16->setName("MCcr16");
  (*MCcr16) *= cfg16.lumi();

  MT2Analysis<MT2Estimate>* MCcr17 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir17 + "/mc.root", "llepCR");
  MCcr17->setName("MCcr17");
  (*MCcr17) *= cfg17.lumi();

  MT2Analysis<MT2Estimate>* MCcr18 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir18 + "/mc.root", "llepCR");
  MCcr18->setName("MCcr18");
  (*MCcr18) *= cfg18.lumi();

  MT2Analysis<MT2Estimate>* MCcr = new MT2Analysis<MT2Estimate>( *(MCcr16) );
  MCcr->setName("MCcr");
  (*MCcr) += (*(MCcr17));
  (*MCcr) += (*(MCcr18));

  // MC Signal Region - year-by-year and combined
  MT2Analysis<MT2Estimate>* MCsr16 = MT2Analysis<MT2Estimate>::readFromFile(cfg16.getEventYieldDir() + "/analyses.root", "Top");
  MCsr16->setName( "MCsr16" );
  MT2Analysis<MT2Estimate>* WJets16 = MT2Analysis<MT2Estimate>::readFromFile(cfg16.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr16) += (*(WJets16));
  (*MCsr16) *= cfg16.lumi();

  MT2Analysis<MT2Estimate>* MCsr17 = MT2Analysis<MT2Estimate>::readFromFile(cfg17.getEventYieldDir() + "/analyses.root", "Top");
  MCsr17->setName( "MCsr17" );
  MT2Analysis<MT2Estimate>* WJets17 = MT2Analysis<MT2Estimate>::readFromFile(cfg17.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr17) += (*(WJets17));
  (*MCsr17) *= cfg17.lumi();

  MT2Analysis<MT2Estimate>* MCsr18 = MT2Analysis<MT2Estimate>::readFromFile(cfg18.getEventYieldDir() + "/analyses.root", "Top");
  MCsr18->setName( "MCsr18" );
  MT2Analysis<MT2Estimate>* WJets18 = MT2Analysis<MT2Estimate>::readFromFile(cfg18.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr18) += (*(WJets18));
  (*MCsr18) *= cfg18.lumi();

  MT2Analysis<MT2Estimate>* MCsr = new MT2Analysis<MT2Estimate>( *(MCsr16) );
  MCsr->setName("MCsr");
  (*MCsr) += (*(MCsr17));
  (*MCsr) += (*(MCsr18));

  // MC Control Region Integral - combined - needed for RatioMC
  MT2Analysis<MT2Estimate>* MCcr_ = new MT2Analysis<MT2Estimate>( *(MCcr) );
  MCcr_->setName("MCcr_");

  MT2Analysis<MT2Estimate>* MCcr_integral;
  MCcr_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCcr_integral", cfg16.regionsSet(), MCcr_ );
  MCcr_integral->setName("MCcr_integral");

  // MC Signal Region Integral - year by year - needed for RatioMC
  MT2Analysis<MT2Estimate>* MCsr16_ = new MT2Analysis<MT2Estimate>( *(MCsr16) );
  MCsr16_->setName("MCsr16_");
  MT2Analysis<MT2Estimate>* MCsr16_integral;
  MCsr16_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCsr16_integral", cfg16.regionsSet(), MCsr16_ );
  MCsr16_integral->setName("MCsr16_integral");

  MT2Analysis<MT2Estimate>* MCsr17_ = new MT2Analysis<MT2Estimate>( *(MCsr17) );
  MCsr17_->setName("MCsr17_");
  MT2Analysis<MT2Estimate>* MCsr17_integral;
  MCsr17_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCsr17_integral", cfg17.regionsSet(), MCsr17_ );
  MCsr17_integral->setName("MCsr17_integral");

  MT2Analysis<MT2Estimate>* MCsr18_ = new MT2Analysis<MT2Estimate>( *(MCsr18) );
  MCsr18_->setName("MCsr18_");
  MT2Analysis<MT2Estimate>* MCsr18_integral;
  MCsr18_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCsr18_integral", cfg18.regionsSet(), MCsr18_ );
  MCsr18_integral->setName("MCsr18_integral");

  // MC ratio SR/CR - year by year and combined
  MT2Analysis<MT2Estimate>* RatioMC16 = new MT2Analysis<MT2Estimate>( "RatioMC16", cfg16.regionsSet() );
  (*RatioMC16) = ( (*MCsr16_integral) / (*MCcr_integral) );

  MT2Analysis<MT2Estimate>* RatioMC17 = new MT2Analysis<MT2Estimate>( "RatioMC17", cfg17.regionsSet() );
  (*RatioMC17) = ( (*MCsr17_integral) / (*MCcr_integral) );

  MT2Analysis<MT2Estimate>* RatioMC18 = new MT2Analysis<MT2Estimate>( "RatioMC18", cfg18.regionsSet() );
  (*RatioMC18) = ( (*MCsr18_integral) / (*MCcr_integral) );

  MT2Analysis<MT2Estimate>* RatioMC = new MT2Analysis<MT2Estimate>( *(RatioMC16) );
  RatioMC->setName("RatioMC");
  (*RatioMC) += (*(RatioMC17));
  (*RatioMC) += (*(RatioMC18));

  ////////////////////////////////////////////////////////////////
  // Prepare estimates for Hybrid shape calculation
  ////////////////////////////////////////////////////////////////

  // Hybrid shapes that will be computed - hybrid_llepCR should be the same for the three cases
  //                                       hybrid_shape  is going to be different in the numerator (using year-by-year MC)

  MT2Analysis<MT2Estimate>* hybrid_shape16 = new MT2Analysis<MT2Estimate>( "hybrid_shape16", cfg16.regionsSet() );
  MT2Analysis<MT2Estimate>* hybrid_shape17 = new MT2Analysis<MT2Estimate>( "hybrid_shape17", cfg17.regionsSet() );
  MT2Analysis<MT2Estimate>* hybrid_shape18 = new MT2Analysis<MT2Estimate>( "hybrid_shape18", cfg18.regionsSet() );

  // for the moment keep three separate for sanity checks
  MT2Analysis<MT2Estimate>* hybrid_llepCR = new MT2Analysis<MT2Estimate>( "hybrid_llepCR", cfg16.regionsSet() );

  // extrapol bin that will be computed - only one for all three years
  MT2Analysis<MT2Estimate>* extrapol_bin = new MT2Analysis<MT2Estimate>( "extrapol_bin", cfg16.regionsSet() );

  // MC Signal Region (multiplied by lumi) - one for each year
  MT2Analysis<MT2Estimate>* MCsr_forShape16   = MT2Analysis<MT2Estimate>::readFromFile(cfg16.getEventYieldDir() + "/analyses.root", "Top");
  MCsr_forShape16->setName( "MCsr_forShape16" );
  MT2Analysis<MT2Estimate>* WJets_forShape16 = MT2Analysis<MT2Estimate>::readFromFile(cfg16.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr_forShape16) += (*(WJets_forShape16));
  (*MCsr_forShape16) *= cfg16.lumi();

  MT2Analysis<MT2Estimate>* MCsr_forShape17   = MT2Analysis<MT2Estimate>::readFromFile(cfg17.getEventYieldDir() + "/analyses.root", "Top");
  MCsr_forShape17->setName( "MCsr_forShape17" );
  MT2Analysis<MT2Estimate>* WJets_forShape17 = MT2Analysis<MT2Estimate>::readFromFile(cfg17.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr_forShape17) += (*(WJets_forShape17));
  (*MCsr_forShape17) *= cfg17.lumi();

  MT2Analysis<MT2Estimate>* MCsr_forShape18   = MT2Analysis<MT2Estimate>::readFromFile(cfg18.getEventYieldDir() + "/analyses.root", "Top");
  MCsr_forShape18->setName( "MCsr_forShape18" );
  MT2Analysis<MT2Estimate>* WJets_forShape18 = MT2Analysis<MT2Estimate>::readFromFile(cfg18.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr_forShape18) += (*(WJets_forShape18));
  (*MCsr_forShape18) *= cfg18.lumi();

  // MC Control Region (multiplied by lumi) - one for each year and then combined
  MT2Analysis<MT2Estimate>*  MCcr_forShape16 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir16 + "/mc.root", "llepCR");
  MCcr_forShape16->setName( "MCcr_forShape16" );
  (*MCcr_forShape16) *= cfg16.lumi();

  MT2Analysis<MT2Estimate>*  MCcr_forShape17 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir17 + "/mc.root", "llepCR");
  MCcr_forShape17->setName( "MCcr_forShape17" );
  (*MCcr_forShape17) *= cfg17.lumi();

  MT2Analysis<MT2Estimate>*  MCcr_forShape18 = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir18 + "/mc.root", "llepCR");
  MCcr_forShape18->setName( "MCcr_forShape18" );
  (*MCcr_forShape18) *= cfg18.lumi();

  MT2Analysis<MT2Estimate>*  MCcr_forShape = new MT2Analysis<MT2Estimate>( *(MCcr_forShape16) );
  MCcr_forShape->setName("MCcr_forShape");
  (*MCcr_forShape) += (*(MCcr_forShape17));
  (*MCcr_forShape) += (*(MCcr_forShape18));

  // Data control region - one for each year and then combined
  MT2Analysis<MT2Estimate>* llepCR_data_forShape16;
  llepCR_data_forShape16  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir16 + "/data.root", "llepCR");
  llepCR_data_forShape16->setName("llepCR_data_forShape16");

  MT2Analysis<MT2Estimate>* llepCR_data_forShape17;
  llepCR_data_forShape17  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir17 + "/data.root", "llepCR");
  llepCR_data_forShape17->setName("llepCR_data_forShape17");

  MT2Analysis<MT2Estimate>* llepCR_data_forShape18;
  llepCR_data_forShape18  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir18 + "/data.root", "llepCR");
  llepCR_data_forShape18->setName("llepCR_data_forShape18");

  MT2Analysis<MT2Estimate>*  llepCR_data_forShape = new MT2Analysis<MT2Estimate>( *(llepCR_data_forShape16) );
  llepCR_data_forShape->setName("llepCR_data_forShape");
  (*llepCR_data_forShape) += (*(llepCR_data_forShape17));
  (*llepCR_data_forShape) += (*(llepCR_data_forShape18));

  //Building the hybrid shapes
  buildHybrid( hybrid_llepCR, hybrid_shape16, llepCR_data_forShape, MCsr_forShape16, MCcr_forShape, extrapol_bin );
  buildHybrid( hybrid_llepCR, hybrid_shape17, llepCR_data_forShape, MCsr_forShape17, MCcr_forShape, extrapol_bin );
  buildHybrid( hybrid_llepCR, hybrid_shape18, llepCR_data_forShape, MCsr_forShape18, MCcr_forShape, extrapol_bin );
  // final estimate will be hybrid_llepCR * hybrid_shape

  ////////////////////////////////////////////////////////////////
  // Final estimates and save to output
  ////////////////////////////////////////////////////////////////

  // Lost lepton estimates , year-by-year and combined
  MT2Analysis<MT2Estimate>* llepEstimate16 = new MT2Analysis<MT2Estimate>( "llepEstimate16", cfg16.regionsSet() );
  (*llepEstimate16) = (*hybrid_llepCR) * (*hybrid_shape16);

  MT2Analysis<MT2Estimate>* llepEstimate17 = new MT2Analysis<MT2Estimate>( "llepEstimate17", cfg17.regionsSet() );
  (*llepEstimate17) = (*hybrid_llepCR) * (*hybrid_shape17);

  MT2Analysis<MT2Estimate>* llepEstimate18 = new MT2Analysis<MT2Estimate>( "llepEstimate18", cfg18.regionsSet() );
  (*llepEstimate18) = (*hybrid_llepCR) * (*hybrid_shape18);

  MT2Analysis<MT2Estimate>* llepEstimate = new MT2Analysis<MT2Estimate>(*(llepEstimate16) );
  llepEstimate->setName("llepEstimate");
  (*llepEstimate) += (*llepEstimate17);
  (*llepEstimate) += (*llepEstimate18);


  // save everything in the first directory ...
  std::string outFile = cfg16.getEventYieldDir() + "/llepEstimateCombined";
  outFile += ".root";


  llepEstimate->writeToFile( outFile, "recreate" );
  llepEstimate16->addToFile( outFile );
  llepEstimate17->addToFile( outFile );
  llepEstimate18->addToFile( outFile );

  hybrid_shape16->addToFile( outFile );
  hybrid_shape17->addToFile( outFile );
  hybrid_shape18->addToFile( outFile );

  hybrid_llepCR->addToFile( outFile );
  extrapol_bin->addToFile( outFile );

  MCcr->addToFile(outFile);
  MCcr16->addToFile(outFile);
  MCcr17->addToFile(outFile);
  MCcr18->addToFile(outFile);

  MCsr->addToFile(outFile);
  MCsr16->addToFile(outFile);
  MCsr17->addToFile(outFile);
  MCsr18->addToFile(outFile);

  RatioMC->addToFile(outFile);
  RatioMC16->addToFile(outFile);
  RatioMC17->addToFile(outFile);
  RatioMC18->addToFile(outFile);

  dataCR->addToFile( outFile );
  dataCR16->addToFile( outFile );
  dataCR17->addToFile( outFile );
  dataCR18->addToFile( outFile );


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
