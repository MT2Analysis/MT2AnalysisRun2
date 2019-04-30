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



bool do_dummyMC = false;
bool do_hybrid = true;


void buildHybrid( MT2Analysis<MT2Estimate>* nlepCR, MT2Analysis<MT2Estimate>* shape_hybrid, MT2Analysis<MT2Estimate>* shape_data, MT2Analysis<MT2Estimate>* shape_MCsr, MT2Analysis<MT2Estimate>* shape_MCcr, MT2Analysis<MT2Estimate>* bin_extrapol );


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


  TH1::AddDirectory(kFALSE);


  std::string llepControlRegionDir = cfg.getEventYieldDir() + "/llepControlRegion";


  ////////////////////////////////////////////////////////////////
  // Get inputs and calculate ratio MC
  ////////////////////////////////////////////////////////////////

  // Data Control Region
  MT2Analysis<MT2Estimate>* llepCR;
  if( !do_dummyMC )
    llepCR  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/data.root", "llepCR");
  else{
    llepCR  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
    (*llepCR) *= cfg.lumi();
  }

  if( llepCR==0 ) {
    std::cout << "-> Please run llepControlRegion first. I need to get the llepCR yields from there." << std::endl;
    std::cout << "-> Thank you for your cooperation." << std::endl;
    exit(193);
  }

  // MC Control Region
  MT2Analysis<MT2Estimate>* MCcr_ = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
  if( MCcr_==0 ) {
    std::cout << "-> Please run llepControlRegion first. I need to get the MC llepCR yields from there." << std::endl;
    std::cout << "-> Thank you for your cooperation." << std::endl;
    exit(193);
  }

  // MC Control Region Integral
  MT2Analysis<MT2Estimate>* MCcr = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
  MCcr->setName("llepCR_mc");
  MT2Analysis<MT2Estimate>* MCcr_integral;
  if( do_hybrid ){
    MCcr_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCcr_integral", cfg.regionsSet(), MCcr_ );
    MCcr_integral->setName("llepCR_mc_integral");
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

  // MC Signal Region integral
  MT2Analysis<MT2Estimate>* MCsr_   = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "Top");
  MCsr_->setName("MCsr_");

  MT2Analysis<MT2Estimate>* WJets_ = MT2Analysis<MT2Estimate>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "WJets");
  (*MCsr_) += (*(WJets_));

  MT2Analysis<MT2Estimate>* MCsr_integral;
  if( do_hybrid ){
    MCsr_integral = MT2Estimate::makeIntegralAnalysisFromEstimate( "MCsr_integral", cfg.regionsSet(), MCsr_ );
    // MCsr_integral->setName("llepSR_mc_integral");
  }

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
  MT2Analysis<MT2Estimate>*  MCcr_forShape = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
  MCcr_forShape->setName( "MCcr_forShape" );

  (*MCsr_forShape) *= cfg.lumi();
  (*MCcr_forShape) *= cfg.lumi();

  // Data control region (multiplied by lumi)
  MT2Analysis<MT2Estimate>* llepCR_data_forShape;
  if( !do_dummyMC )
    llepCR_data_forShape  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/data.root", "llepCR");
  else{
    llepCR_data_forShape  = MT2Analysis<MT2Estimate>::readFromFile(llepControlRegionDir + "/mc.root", "llepCR");
    (*llepCR_data_forShape) *= cfg.lumi();
  }


  //Building the hybrid shape
  buildHybrid( hybrid_llepCR, hybrid_shape, llepCR_data_forShape, MCsr_forShape, MCcr_forShape, extrapol_bin );
  // final estimate will be hybrid_llepCR * hybrid_shape

  ////////////////////////////////////////////////////////////////
  // Final estimates and save to output
  ////////////////////////////////////////////////////////////////

  // Lost lepton estimate
  MT2Analysis<MT2Estimate>* llepEstimate = new MT2Analysis<MT2Estimate>( "llepEstimate", cfg.regionsSet() );
  (*llepEstimate) = (*hybrid_llepCR) * (*hybrid_shape);

  std::string outFile = cfg.getEventYieldDir() + "/llepEstimate";
  outFile += ".root";

  llepEstimate->writeToFile( outFile, "recreate" );
  hybrid_shape->addToFile( outFile );
  hybrid_llepCR->addToFile( outFile );
  RatioMC->addToFile( outFile );
  extrapol_bin->addToFile( outFile );
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
    float integral = 0.;
    double integralMC = 0.;
    double errMC = 1.;

    std::vector< std::string > niceNames = region->getNiceNames();
    std::cout << niceNames[0] << " " << niceNames[1] << std::endl;
    for( int iBin=nBins; iBin>= 1; iBin-- ){
      // std::cout << this_data_forShape->Integral( iBin, -1)  << std::endl;
      // if( this_data_forShape->Integral( iBin, -1) >= 10. ){
      if( this_MCcr_forShape->Integral( iBin, -1) >= 50. || nBins==1 ){
        if( iBin == nBins ){ //We take the full shape from data!
          bin_extrapol = iBin+1;
          integral = 1.;    //we don't have to do a special normalization in this case
          integralMC = this_MCcr_forShape->IntegralAndError( iBin, -1, errMC);
        }else{
          bin_extrapol = iBin;
          integral = this_data_forShape->Integral( iBin, -1);
          integralMC = this_MCcr_forShape->IntegralAndError( iBin, -1, errMC);
	      }
	      break;
      }
      else{
        bin_extrapol = 1;
        integral  = this_data_forShape->Integral( bin_extrapol, -1);
        integralMC = this_MCcr_forShape->IntegralAndError( bin_extrapol, -1, errMC);
      }
    } // end loop over mt2 bins

    //Filling the histo that knows where we extrapolate
    this_binExtrapol->SetBinContent( 1, bin_extrapol );

    std::cout << "extrapol bin / total bins= " << bin_extrapol << " / " << nBins << " : " << integral << std::endl;

    // loop again over mt2 bins
    for(int iBin=1; iBin<= nBins; iBin++){
      float MCsr_cont;
      float MCcr_cont;
      float MCsr_contErr;
      float MCcr_contErr;
      if(iBin<bin_extrapol){
        MCsr_cont = this_MCsr_forShape->GetBinContent(iBin);
        MCcr_cont = this_MCcr_forShape->GetBinContent(iBin);
        MCsr_contErr = this_MCsr_forShape->GetBinError(iBin);
        MCcr_contErr = this_MCcr_forShape->GetBinError(iBin);
      }
      else{
        MCsr_cont    = this_MCsr_forShape->GetBinContent(iBin);
        MCsr_contErr = this_MCsr_forShape->GetBinError(iBin);
        MCcr_cont = integralMC;
        MCcr_contErr = errMC;
      }

      float ratioMC_cont = MCsr_cont/MCcr_cont;
      float ratioMC_err  = sqrt( (MCsr_contErr/MCcr_cont)*(MCsr_contErr/MCcr_cont) + (MCsr_cont*MCcr_contErr/(MCcr_cont*MCcr_cont))*(MCsr_cont*MCcr_contErr/(MCcr_cont*MCcr_cont)) );

      if (ratioMC_cont>0) ratioMC_err /= ratioMC_cont;

      float relativeErrorData;
      float relativeErrorMC;

      std::cout << "Bin Content " << iBin << ": " << this_MCcr_forShape->GetBinContent(iBin) << " : " << this_MCsr_forShape->GetBinContent(iBin) << " : " << ratioMC_cont << std::endl;

      if(ratioMC_cont > 3){
        std::cout << ratioMC_cont << std::endl;
        ratioMC_cont = 3.;
      }

      if( iBin<bin_extrapol && (bin_extrapol != nBins) ){

        //relativeErrorData = sqrt( this_data_forShape->GetBinError(iBin)*this_data_forShape->GetBinError(iBin)/(this_data_forShape->GetBinContent(iBin)*this_data_forShape->GetBinContent(iBin)) + ratioMC_err*ratioMC_err );
        relativeErrorMC = sqrt( this_MCcr_forShape->GetBinError(iBin)*this_MCcr_forShape->GetBinError(iBin)/(this_MCcr_forShape->GetBinContent(iBin)*this_MCcr_forShape->GetBinContent(iBin)) + ratioMC_err*ratioMC_err );

        this_hybrid_llepCR->SetBinContent(iBin, this_data_forShape->GetBinContent(iBin));
        this_hybrid_llepCR->SetBinError(iBin, this_data_forShape->GetBinError(iBin));

        this_data_forShape ->SetBinContent(iBin, ratioMC_cont);
        this_data_forShape ->SetBinError(iBin, ratioMC_cont*ratioMC_err);

        //	this_data_forShape ->SetBinContent(iBin, this_data_forShape->GetBinContent(iBin)*ratioMC_cont);
        //	this_data_forShape ->SetBinError(iBin, relativeErrorData*this_data_forShape->GetBinContent(iBin));
        this_MCcr_forShape ->SetBinContent(iBin, this_MCcr_forShape->GetBinContent(iBin)*ratioMC_cont);
        this_MCcr_forShape ->SetBinError(iBin, relativeErrorMC*this_MCcr_forShape->GetBinContent(iBin));

      }else{

        //relativeErrorData = sqrt( 1/sqrt(integral)*1/sqrt(integral) + ratioMC_err*ratioMC_err );
        relativeErrorMC = sqrt( 1/sqrt(integralMC)*1/sqrt(integralMC) + ratioMC_err*ratioMC_err );

        this_hybrid_llepCR ->SetBinContent(iBin, integral);
        this_hybrid_llepCR ->SetBinError(iBin, sqrt(integral));

        this_data_forShape ->SetBinContent(iBin, ratioMC_cont);
        this_data_forShape ->SetBinError(iBin, ratioMC_cont*ratioMC_err);

        //	this_data_forShape ->SetBinContent(iBin, integral*ratioMC_cont);
        //	this_data_forShape ->SetBinError(iBin, relativeErrorData*this_data_forShape->GetBinContent(iBin));

        this_MCcr_forShape ->SetBinContent(iBin, integralMC*ratioMC_cont);
        this_MCcr_forShape ->SetBinError(iBin, relativeErrorMC*this_MCcr_forShape->GetBinContent(iBin));
      }

      std::cout << "Bin Content " << iBin << ": " << this_MCcr_forShape->GetBinContent(iBin) << " : "  <<  this_hybrid_llepCR->GetBinContent(iBin) << " : " << this_data_forShape->GetBinContent(iBin) << " : " << this_MCsr_forShape->GetBinContent(iBin) << " : " << integral << " : "  <<  ratioMC_cont << std::endl;

    } // end mt2 bins

    for(int iBin=1; iBin<= nBins; iBin++){
      this_hybrid_shape->SetBinContent(iBin, this_data_forShape->GetBinContent(iBin) );
      this_hybrid_shape->SetBinError  (iBin, this_data_forShape->GetBinError(iBin) );
    }

    if( nBins == 1) this_hybrid_shape->SetBinError( 1, 0.0 );

  }//end loop over final estimate loops

  return;

}
