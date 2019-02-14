#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>

#include "../interface/MT2Analysis.h"
#include "../interface/MT2EstimateZinvGamma.h"
#include "../interface/MT2EstimateTree.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2Sample.h"
#include "../interface/MT2DrawTools.h"
#include "../interface/MT2Config.h"

#define mt2_cxx
#include "../interface/mt2.h"
#include "../interface/Utils.h"
#include "../interface/MT2LeptonSFTool.h"
#include "../interface/MT2BTagSFHelper.h"

#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TLorentzVector.h"

using namespace std;

int Round(float d) {
  return (int)(floor(d + 0.5));
}

bool doZinvEst = true;
bool do_bg = true;




TH1D*  h_muTrk_hi = 0;
TH2D*  h_elTrk = 0;


void computeYieldSnO( const MT2Sample& sample, const MT2Config& cfg,
		      MT2Analysis<MT2EstimateTree>* anaTree,
		      MT2Analysis<MT2EstimateTree>* anaTree_of,
		      MT2BTagSFHelper* bTagSF,
		      bool do_ZinvEst,  bool invertedZcuts = false );
void addVariables(MT2Analysis<MT2EstimateTree>* anaTree);
void roundLikeData( MT2Analysis<MT2EstimateTree>* data );
//float getHLTweight( int lep1_pdgId, int lep2_pdgId, float lep1_pt, float lep2_pt, int variation);


int main(int argc, char* argv[]) {

  std::cout << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|            Running zllControlRegion                |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc<2 ) {
    std::cout << "USAGE: ./zllControlRegion [configFileName] [data/MC]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  std::string configFileName(argv[1]);
  MT2Config cfg(configFileName);

  std::cout << "-> Reading era/year from config: " << cfg.year() << std::endl;
  if(cfg.year()==0) {
    std::cout << "There must be an error: era/year is empty in config" << std::endl;
    exit(120);
  }

  bool onlyData   = false;
  bool onlyMC     = false;
  bool onlySignal = false;
  if( argc > 2 ) {
    std::string dataMC(argv[2]);
    if( dataMC=="data" ) onlyData = true;
    else if( dataMC=="MC" || dataMC=="mc" ) onlyMC = true;
    else if( dataMC=="signal" ) onlySignal = true;
    else {
      std::cout << "-> You passed a second argument that isn't 'data' nor 'MC/mc', so I don't know what to do about it." << std::endl;
    }
  }

  TH1::AddDirectory(kFALSE); //stupid ROOT memory allocation needs this

  std::string outputdir = cfg.getEventYieldDir() + "/zllControlRegion";
  system(Form("mkdir -p %s", outputdir.c_str()));

  std::string regionsSet;// = "13TeV_inclusive";
  regionsSet=cfg.crRegionsSet();
  // regionsSet=cfg.zllRegions();
  // std::string regionsSet = cfg.zllRegions();

  std::cout << "-> Using regions: " << regionsSet << std::endl;


  //previous method to get the scale factors: we don't need this anymore!
  /*
  //Getting the scale factor histogram/////////////////
  //Electrons//

  ////////////////////////////////////////////
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  // WARNING !!!!
  // THE SCALE FACTOR HISTOGRAMS HAVE BEEN UPDATED: COULDNT FIND GfsElectronToVeto ANYMORE
  // USED THE OLD scaleFactor.root FILE
  // TO BE FIXED!!

  std::string filename = "/mnt/t3nfs01/data01/shome/mschoene/lepSF/old2016/scaleFactors_old.root";
  TFile * f_ele = new TFile(filename.c_str() );
  if (!f_ele->IsOpen()) std::cout << " ERROR: Could not find scale factor file " << filename << std::endl;
  //Uncomment for loose Id
  //TH2D* h_id = (TH2D*) f_ele->Get("CutBasedLoose");
  //(TH2D*) f_ele->Get("CutBasedVeto");
  TH2D* h_id = (TH2D*) f_ele->Get("GsfElectronToVeto");
  TH2D* h_iso = (TH2D*) f_ele->Get("MVAVLooseElectronToMini");
  if (!h_id || !h_iso) std::cout << "ERROR: Could not find scale factor histogram"<< std::endl;
  TH2D* h_elSF = (TH2D*) h_id->Clone("h_elSF");
  h_elSF->SetDirectory(0);
  h_elSF->Multiply(h_iso);

  std::string filenameElTrk = "/mnt/t3nfs01/data01/shome/mschoene/lepSF/egammaEffi_SF2D.root";
  TFile * f_eleTrk = new TFile(filenameElTrk.c_str() );
  if (!f_eleTrk->IsOpen()) std::cout << " ERROR: Could not find scale factor file " << filenameElTrk << std::endl;
  h_elTrk = (TH2D*) f_eleTrk->Get("EGamma_SF2D");
  h_elTrk->SetDirectory(0);
  f_eleTrk->Close(); delete f_eleTrk;


  //Muons//
  std::string filenameID = "/mnt/t3nfs01/data01/shome/mschoene/lepSF/2016/TnP_MuonID_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root";
  std::string filenameISO = "/mnt/t3nfs01/data01/shome/mschoene/lepSF/2016/TnP_MuonID_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root";
  std::string filenamedxyz = "/mnt/t3nfs01/data01/shome/mschoene/lepSF/2016/TnP_MuonID_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root";
  TFile * f1 = new TFile(filenameID.c_str() );
  TFile * f2 = new TFile(filenameISO.c_str() );
  TFile * f3 = new TFile(filenamedxyz.c_str() );
  if (!f1->IsOpen()) { std::cout<<" ERROR: Could not find ID scale factor file "<<filenameID<<std::endl; return 0;}
  if (!f2->IsOpen()) { std::cout<<"ERROR: Could not find ISO scale factor file "<<filenameISO<<std::endl; return 0;}
  if (!f3->IsOpen()) { std::cout<<"ERROR: Could not find dxy dz scale factor file "<<filenamedxyz<<std::endl; return 0;}
  TH2D* h_id_mu = (TH2D*) f1->Get("pt_abseta_PLOT_pair_probeMultiplicity_bin0");
  TH2D* h_iso_mu = (TH2D*) f2->Get("pt_abseta_PLOT_pair_probeMultiplicity_bin0_&_PF_pass");
  TH2D* h_dxyz_mu = (TH2D*) f3->Get("pt_abseta_PLOT_pair_probeMultiplicity_bin0_&_PF_pass");
  if (!h_id_mu || !h_iso_mu  || !h_dxyz_mu) { std::cout<<"ERROR: Could not find scale factor histogram"<<std::endl; return 0;}
  TH2D* h_muSF = (TH2D*) h_id_mu->Clone("h_muSF");
  h_muSF->SetDirectory(0);
  h_muSF->Multiply(h_iso_mu);
  h_muSF->Multiply(h_dxyz_mu);



  TH1D* h_trk_mu_hi = 0;

  std::string filenameTrk = "/mnt/t3nfs01/data01/shome/mschoene/lepSF/general_tracks_and_early_general_tracks_corr_ratio.root";
  TFile * fTrk = new TFile(filenameTrk.c_str() );
  if (!fTrk->IsOpen()) { std::cout<<" ERROR: Could not find track ineff scale factor file "<<filenameTrk<<std::endl; return 0;}
  h_trk_mu_hi = (TH1D*) fTrk->Get("mutrksfptg10");
  if (!h_trk_mu_hi) { std::cout<<"ERROR: Could not find trk sf histogram"<<std::endl; return 0;}
  h_muTrk_hi = (TH1D*) h_trk_mu_hi->Clone("h_muTrk_hi");
  h_muTrk_hi->SetDirectory(0);
  // fTrk->Close(); delete fTrk;
  */

  if( onlySignal ){
    onlyData=true;
    onlyMC=true;
  }


   if( cfg.useMC() && !onlyData ) { // run on MC

    std::string samplesFileName = "../samples/samples_" + cfg.mcSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;


    std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 700, 799, cfg.useETHmc()); // DY signal only
    if( fSamples.size()==0 ) {
      std::cout << "There must be an error: samples is empty!" << std::endl;
      exit(1209);
    }



    MT2Analysis<MT2EstimateTree>* mcTree = new MT2Analysis<MT2EstimateTree>( "zllCR", cfg.crRegionsSet() );
    addVariables(mcTree); //Adds some additional variables Zpt,Zmass, raw MT2...

    MT2Analysis<MT2EstimateTree>* mcTree_of = new MT2Analysis<MT2EstimateTree>( "zllCR_of", cfg.crRegionsSet() );
    addVariables(mcTree_of);

    //MT2BTagSFHelper* bTagSF = new MT2BTagSFHelper();

    for( unsigned i=0; i<fSamples.size(); ++i ){
      MT2BTagSFHelper* bTagSF = new MT2BTagSFHelper();
      computeYieldSnO( fSamples[i], cfg, mcTree, mcTree_of, bTagSF, false);
      //delete bTagSF;
      bTagSF = nullptr;
    }

    //delete bTagSF;
    //bTagSF = nullptr;

    mcTree->writeToFile(outputdir+"/mc.root");
    mcTree_of->writeToFile(outputdir+"/mc_of.root");




    if(doZinvEst){
      //MT2BTagSFHelper* bTagSF_forZinvEst = new MT2BTagSFHelper();
      MT2Analysis<MT2EstimateTree>* mcTree_forZinvEst = new MT2Analysis<MT2EstimateTree>( "zllCR", cfg.regionsSet() );
      addVariables(mcTree_forZinvEst); //Adds some additional variables Zpt,Zmass, raw MT2...

      MT2Analysis<MT2EstimateTree>* mcTree_of_forZinvEst = new MT2Analysis<MT2EstimateTree>( "zllCR_of", cfg.regionsSet() );
      addVariables(mcTree_of_forZinvEst);

      for( unsigned i=0; i<fSamples.size(); ++i ){
	MT2BTagSFHelper* bTagSF_forZinvEst = new MT2BTagSFHelper();
	computeYieldSnO( fSamples[i], cfg, mcTree_forZinvEst, mcTree_of_forZinvEst, bTagSF_forZinvEst, true);
	//delete bTagSF_forZinvEst;
	bTagSF_forZinvEst = nullptr;
      }

      //delete bTagSF_forZinvEst;
      //bTagSF_forZinvEst = nullptr;

      mcTree_forZinvEst->writeToFile(outputdir+"/mc_forZinvEst.root");
      mcTree_of_forZinvEst->writeToFile(outputdir+"/mc_of_forZinvEst.root");


    }

    if( cfg.dummyAnalysis() ) {
      roundLikeData(mcTree);
      mcTree->addToFile(outputdir+"/data.root");
      roundLikeData(mcTree_of);
      mcTree_of->addToFile(outputdir+"/data_of.root");
    }


    if(do_bg==true){
       //MC
      MT2Analysis<MT2EstimateTree>* mc_top = new MT2Analysis<MT2EstimateTree>( "Top", cfg.crRegionsSet(),300, "Top" );
      MT2Analysis<MT2EstimateTree>* mc_top_of = new MT2Analysis<MT2EstimateTree>( "Top", cfg.crRegionsSet(),300, "Top" );
      addVariables(mc_top);      addVariables(mc_top_of);

      std::vector<MT2Sample> fSamples_top = MT2Sample::loadSamples(samplesFileName, 300, 499, cfg.useETHmc());
      for( unsigned i=0; i<fSamples_top.size(); ++i ){
	MT2BTagSFHelper* bTagSF_top = new MT2BTagSFHelper();
	computeYieldSnO( fSamples_top[i], cfg, mc_top, mc_top_of, bTagSF_top, false);
	bTagSF_top = nullptr;
      }


	 //MT2Analysis<MT2EstimateTree>* mc_qcd = new MT2Analysis<MT2EstimateTree>( "QCD", cfg.crRegionsSet(),100, "QCD" );
	 //MT2Analysis<MT2EstimateTree>* mc_qcd_of = new MT2Analysis<MT2EstimateTree>( "QCD", cfg.crRegionsSet(),100, "QCD");
	 //addVariables(mc_qcd);      addVariables(mc_qcd_of);
	 //std::vector<MT2Sample> fSamples_qcd = MT2Sample::loadSamples(samplesFileName, 100, 199);
	 //for( unsigned i=0; i<fSamples_qcd.size(); ++i )
	 //computeYieldSnO( fSamples_qcd[i], cfg, mc_qcd, mc_qcd_of, h_elSF, h_muSF, false);

	 //MT2Analysis<MT2EstimateTree>* mc_wjets = new MT2Analysis<MT2EstimateTree>( "WJets", cfg.crRegionsSet(),500, "W+jets"  );
	 //MT2Analysis<MT2EstimateTree>* mc_wjets_of = new MT2Analysis<MT2EstimateTree>( "WJets", cfg.crRegionsSet(),500, "W+jets");
	 //addVariables(mc_wjets);      addVariables(mc_wjets_of);
	 //std::vector<MT2Sample> fSamples_wjets = MT2Sample::loadSamples(samplesFileName, 500, 599);
	 //for( unsigned i=0; i<fSamples_wjets.size(); ++i )
	 //computeYieldSnO( fSamples_wjets[i], cfg, mc_wjets, mc_wjets_of, h_elSF, h_muSF, false);


      MT2Analysis<MT2EstimateTree>* mc_zll   = mcTree;
      mc_zll->setName("DYJets");
      mc_zll->setFullName("DY+jets");

      std::string outFile = outputdir + "/ZllPurityTrees.root";
      mc_zll->writeToFile( outFile );
      mc_top->addToFile( outFile );
      //      mc_qcd->addToFile( outFile );
      //      mc_wjets->addToFile( outFile );

      //For the OPPOSITE FLAVOR EVENTS:
      MT2Analysis<MT2EstimateTree>* mc_zll_of   = mcTree_of;
      mc_zll_of->setName("DYJets");
      mc_zll_of->setFullName("DY+jets");

      std::string outFile_of = outputdir + "/ZllPurityTrees_of.root";
      mc_zll_of->writeToFile( outFile_of );
      mc_top_of->addToFile( outFile_of );
      //     mc_qcd_of->addToFile( outFile_of );
      //     mc_wjets_of->addToFile( outFile_of );

      if(doZinvEst){
	MT2Analysis<MT2EstimateTree>* mc_top_forZinvEst = new MT2Analysis<MT2EstimateTree>( "zllCR", cfg.regionsSet() );
	addVariables(mc_top_forZinvEst); //Adds some additional variables Zpt,Zmass, raw MT2...

	MT2Analysis<MT2EstimateTree>* mc_top_of_forZinvEst = new MT2Analysis<MT2EstimateTree>( "zllCR_of", cfg.regionsSet() );
	addVariables(mc_top_of_forZinvEst);

	for( unsigned i=0; i<fSamples_top.size(); ++i ){
	  MT2BTagSFHelper* bTagSF_top_forZinvEst = new MT2BTagSFHelper();
	  computeYieldSnO( fSamples_top[i], cfg, mc_top_forZinvEst, mc_top_of_forZinvEst, bTagSF_top_forZinvEst, true);
	  bTagSF_top_forZinvEst = nullptr;
	}

	mc_top_forZinvEst->writeToFile(outputdir+"/mc_Top_forZinvEst.root");
	mc_top_of_forZinvEst->writeToFile(outputdir+"/mc_Top_of_forZinvEst.root");
      }

      } //End do background trees

  } //if only MC

  if( !onlyMC ) {

    //DATA
    std::string samplesFile_data = "../samples/samples_" + cfg.dataSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading data from file: " << samplesFile_data << std::endl;
    std::vector<MT2Sample> samples_data = MT2Sample::loadSamples(samplesFile_data, "", -1, 100, cfg.useETHdata());
    //std::vector<MT2Sample> samples_data = MT2Sample::loadSamples(samplesFile_data, "noDuplicates");

    //    std::vector<MT2Sample> samples_data = MT2Sample::loadSamples(samplesFile_data, "Double");

    // std::vector<MT2Sample> samples_data_of = MT2Sample::loadSamples(samplesFile_data, "merged");

    MT2Analysis<MT2EstimateTree>* dataTree = new MT2Analysis<MT2EstimateTree>( "data", cfg.crRegionsSet() );
    MT2Analysis<MT2EstimateTree>* dataTree_of = new MT2Analysis<MT2EstimateTree>( "data_of", cfg.crRegionsSet() );

    //Filler Tree so that I don't have to rewrite the function
    // MT2Analysis<MT2EstimateTree>* dataTree_filler = new MT2Analysis<MT2EstimateTree>( "data_filler", cfg.crRegionsSet() );

    addVariables(dataTree);      addVariables(dataTree_of); // addVariables(dataTree_filler);

    if( samples_data.size()==0 ) {
      std::cout << std::endl;
      std::cout << "-> WARNING!! Didn't find any data in file: " << samplesFile_data << "!" << std::endl;
      std::cout << "-> Exiting." << std::endl;
      std::cout << std::endl;
    } else {
      for( unsigned i=0; i<samples_data.size(); ++i ){
	MT2BTagSFHelper* bTagSF_data = new MT2BTagSFHelper();
	computeYieldSnO( samples_data[i], cfg, dataTree, dataTree_of, bTagSF_data, false);
	bTagSF_data = nullptr;
      }
      //      computeYieldSnO( samples_data[i], cfg, dataTree, dataTree_filler, h_elSF, h_muSF, false);

      //for( unsigned i=0; i<samples_data_of.size(); ++i )
      //	computeYieldSnO( samples_data_of[i], cfg, dataTree_filler, dataTree_of, h_elSF, h_muSF, false);
    }

    dataTree->addToFile(outputdir+"/data.root");
    dataTree_of->writeToFile(outputdir+"/data_of.root");


    //we create here the estimates in the ttbar enriched CR (inverted cuts on Zmass and ZpT) that are needed to compute R(SF/OF) later on
    MT2Analysis<MT2EstimateTree>* dataTree_invertedZcuts = new MT2Analysis<MT2EstimateTree>( "data_invertedZcuts", cfg.regionsSet() );
    MT2Analysis<MT2EstimateTree>* dataTree_of_invertedZcuts = new MT2Analysis<MT2EstimateTree>( "data_of_invertedZcuts", cfg.regionsSet() );
    addVariables(dataTree_invertedZcuts); addVariables(dataTree_of_invertedZcuts);

    for(unsigned i=0; i<samples_data.size(); ++i){
      MT2BTagSFHelper* bTagSF_data_invertedCuts = new MT2BTagSFHelper();
      computeYieldSnO( samples_data[i], cfg, dataTree_invertedZcuts, dataTree_of_invertedZcuts, bTagSF_data_invertedCuts, false, true);
      bTagSF_data_invertedCuts = nullptr;
    }

    dataTree_invertedZcuts->addToFile(outputdir+"/data_invertedZcuts.root");
    dataTree_of_invertedZcuts->addToFile(outputdir+"/data_of_invertedZcuts.root");


    if(doZinvEst){
      MT2Analysis<MT2EstimateTree>* dataTree_forZinvEst = new MT2Analysis<MT2EstimateTree>( "data", cfg.regionsSet() );
      MT2Analysis<MT2EstimateTree>* dataTree_of_forZinvEst = new MT2Analysis<MT2EstimateTree>( "data_of", cfg.regionsSet() );
      addVariables(dataTree_forZinvEst); addVariables(dataTree_of_forZinvEst);

      for( unsigned i=0; i<samples_data.size(); ++i ){
	MT2BTagSFHelper* bTagSF_data_forZinvEst = new MT2BTagSFHelper();
	computeYieldSnO( samples_data[i], cfg, dataTree_forZinvEst, dataTree_of_forZinvEst, bTagSF_data_forZinvEst, true);
	bTagSF_data_forZinvEst = nullptr;
      }

      dataTree_forZinvEst->addToFile(outputdir+"/data_forZinvEst.root");
      dataTree_of_forZinvEst->addToFile(outputdir+"/data_of_forZinvEst.root");
    }



  } // if DATA


  if( onlySignal){
    std::cout << "DOING THE SIGNAL" << std::endl;

    std::string samplesFileName = "../samples/samples_" + cfg.mcSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;

    std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 999, 2000, cfg.useETHmc()); // signal only
    if( fSamples.size()==0 ) {
      std::cout << "There must be an error: samples is empty!" << std::endl;
      exit(1209);
    }

    MT2Analysis<MT2EstimateTree>* signalTree = new MT2Analysis<MT2EstimateTree>( "zllSigCR", cfg.regionsSet() );
    addVariables(signalTree); //Adds some additional variables Zpt,Zmass, raw MT2...

    MT2Analysis<MT2EstimateTree>* signalTree_of = new MT2Analysis<MT2EstimateTree>( "zllSigCR_of", cfg.regionsSet() );
    addVariables(signalTree_of);

    for( unsigned i=0; i<fSamples.size(); ++i ){
      MT2BTagSFHelper* bTagSF_signal = new MT2BTagSFHelper();
      computeYieldSnO( fSamples[i], cfg, signalTree, signalTree_of, bTagSF_signal, true);
      bTagSF_signal = nullptr;
    }

    signalTree->addToFile(outputdir+"/signal_forZinvEst.root");

    signalTree_of->addToFile(outputdir+"/signal_of_forZinvEst.root");


  }//end of only signal


  return 0;

}






void roundLikeData( MT2Analysis<MT2EstimateTree>* data ) {

  std::set<MT2Region> regions = data->getRegions();
  for( std::set<MT2Region>::iterator iR = regions.begin(); iR!=regions.end(); ++iR ) {
    TH1D* thisYield = data->get(*iR)->yield;
    for( int iBin=1; iBin<thisYield->GetNbinsX()+1; ++iBin ) {
      float yield = thisYield->GetBinContent(iBin);
      int yield_rounded = Round(yield);
      thisYield->SetBinContent(iBin, yield_rounded  );
      thisYield->SetBinError(iBin, 0. );
    } // for bins
  } // for regions

}



void addVariables(MT2Analysis<MT2EstimateTree>* anaTree){

  MT2EstimateTree::addVar( anaTree, "ID" );

  MT2EstimateTree::addVar( anaTree, "Z_pt" );
  MT2EstimateTree::addVar( anaTree, "Z_phi" );
  MT2EstimateTree::addVar( anaTree, "Z_eta" );
  MT2EstimateTree::addVar( anaTree, "Z_mass" );
  MT2EstimateTree::addVar( anaTree, "Z_lepId" );
  MT2EstimateTree::addVar( anaTree, "nLep" );

  MT2EstimateTree::addVar( anaTree, "lep_tightId0" );
  MT2EstimateTree::addVar( anaTree, "lep_tightId1" );

  MT2EstimateTree::addVar( anaTree, "lep_pdgId0");
  MT2EstimateTree::addVar( anaTree, "lep_pdgId1");
  MT2EstimateTree::addVar( anaTree, "lep_pt0");
  MT2EstimateTree::addVar( anaTree, "lep_pt1");
  MT2EstimateTree::addVar( anaTree, "lep_eta0");
  MT2EstimateTree::addVar( anaTree, "lep_eta1");
  MT2EstimateTree::addVar( anaTree, "lep_phi0");
  MT2EstimateTree::addVar( anaTree, "lep_phi1");
  MT2EstimateTree::addVar( anaTree, "raw_mt2"); // = mt2 with the two leptons
  MT2EstimateTree::addVar( anaTree, "raw_met"); // = met with the two leptons

  MT2EstimateTree::addVar( anaTree, "weight_lep0");
  MT2EstimateTree::addVar( anaTree, "weight_lep1");
  MT2EstimateTree::addVar( anaTree, "weight_lep_err");

  MT2EstimateTree::addVar( anaTree, "HLT_weight");

  MT2EstimateTree::addVar( anaTree, "nJetHF30" );
  MT2EstimateTree::addVar( anaTree, "jet1_pt" );

  MT2EstimateTree::addVar( anaTree, "zll_met_pt" );
  MT2EstimateTree::addVar( anaTree, "zll_met_phi" );
  MT2EstimateTree::addVar( anaTree, "zll_mht_pt" );
  MT2EstimateTree::addVar( anaTree, "zll_mht_phi" );

}







//Loop over same and oppsite flavor just once
void computeYieldSnO( const MT2Sample& sample, const MT2Config& cfg,
		      MT2Analysis<MT2EstimateTree>* anaTree,
		      MT2Analysis<MT2EstimateTree>* anaTree_of,
		      MT2BTagSFHelper* bTagSF,
		      bool do_ZinvEst, bool invertedZcuts = false ) {

  std::cout << std::endl << std::endl << "Check: " ;
  if(invertedZcuts) std::cout << "invertedZcuts = true";
  else std::cout << "invertedZcuts = false";
  std::cout << std::endl << std::endl;

  //initialization of scale factor tools
  //MT2BTagSFHelper* bTagSF =  new MT2BTagSFHelper();
  MT2LeptonSFTool leptonSF;
  bool electronHist = leptonSF.setElHist("zll"); //checks if all the electron sf files can be loaded
  bool muonHist = leptonSF.setMuHist(); //checks if all the muon sf files can be loaded
  bool diLepTrigEffHist = leptonSF.setDiLepTriggerHist(cfg.year());

  std::string regionsSet = cfg.crRegionsSet();
  if( do_ZinvEst || invertedZcuts ) regionsSet = cfg.regionsSet();

  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;

  bool isData = (sample.id >= -1 && sample.id < 100 );
  std::cout << " sample.id=" << sample.id << " isData=" << isData << std::endl;

  // determine if it is an ETH kind of ntuple or not
  bool isETH = (isData and cfg.useETHdata()) || (!isData and cfg.useETHmc());

  // Tree initialization
  TString treeName = isETH ? "Events" : "mt2";
  TTree* tree = (TTree*)file->Get(treeName);

  MT2Tree myTree(tree, isETH);
  //  myTree.loadGenStuff = false;
  //myTree.Init(tree, isETH);

 // Sum of weights
  double nGen=-9999; double nGenWeighted=-9999;
  if(!isData and isETH){
    nGen = getNgen(sample.file, "genEventCount");
    nGenWeighted = getNgen(sample.file, "genEventSumw");
  }


  int nentries = tree->GetEntries();
  //for( int iEntry=0; iEntry<30000; ++iEntry ) {
  for( int iEntry=0; iEntry<nentries; ++iEntry ) {
    if( iEntry % 5000 == 0 ){
      std::cout << "   Entry: " << iEntry << " / " << nentries << std::endl;
    }

    myTree.GetEntry(iEntry);

    // fix for Lep pdg id
    int lep0_pdgId_to_use = -1;
    int lep1_pdgId_to_use = -1;

    if(isETH) {
      lep0_pdgId_to_use = myTree.lep_pdgId[0];
      lep1_pdgId_to_use = myTree.lep_pdgId[1];
    } else {
      lep0_pdgId_to_use = myTree.lep_pdgId_INT[0];
      lep1_pdgId_to_use = myTree.lep_pdgId_INT[1];
    }

    // if( myTree.isData && !myTree.isGolden ) continue;

    //we apply the filters
    // filters should be the same bw ETH and SnT
    if(isData) {
      if(!myTree.passFilters(cfg.year())) continue;
    } //else {
    //  if(!myTree.passFiltersMC(cfg.year())) continue;
    //}

    // apply good vertex cut once for all
    if (isETH) {
      if(myTree.PV_npvs <= 0) continue;
    } else {
      if(myTree.nVert <= 0) continue;
    }

    //FIXME: uncomment RA2 filter line and line after
    //if( myTree.nJet200MuFrac50DphiMet > 0 ) continue; // new RA2 filter
    //if( myTree.met_miniaodPt/myTree.met_caloPt > 5.0 ) continue;

    //crazy events! To be piped into a separate txt file
    if(myTree.jet_pt[0] > 13000){
      std::cout << "Rejecting weird event at run:lumi:evt = " << myTree.run << ":" << myTree.luminosityBlock << ":" << myTree.event << std::endl;
      continue;
    }
    //check if is there is a nan
    if( isnan(myTree.ht) || isnan(myTree.met_pt) ||  isinf(myTree.ht) || isinf(myTree.met_pt)  ){
      std::cout << "Rejecting nan/inf event at run:lumi:evt = " << myTree.run << ":" << myTree.luminosityBlock << ":" << myTree.event << std::endl;
      continue;
    }

    // monojet id
    if ( myTree.nJet30==1 && !myTree.passMonoJetId(0) ) continue;

    // apply HEM veto
    if (!myTree.passHEMFailVeto(cfg.year(), isETH, isData)) continue;

    // Kinematic selections common to both SF and OF
    int nLep_to_be_used = isETH ? myTree.nLep : myTree.nlep;
    if(!( nLep_to_be_used==2 )) continue;
    if(myTree.lep_pt[0]<100) continue;
    if(myTree.lep_pt[1]<35) continue; //updated value (before <30) due to new trigger efficiency

    if( cfg.analysisType() == "mt2"){
      if( regionsSet!="13TeV_noCut" )
        if( !myTree.passSelection("zll", cfg.year(), isETH) ) continue;
    }

    if((lep0_pdgId_to_use*lep1_pdgId_to_use)>0 )   continue;

    int njets  = myTree.nJet30;
    int nbjets = myTree.nBJet20;
    float ht   = myTree.zll_ht;
    //float met  = myTree.zll_met_pt;
    float mt2  = (njets>1) ? myTree.zll_mt2 : myTree.zll_ht;
    float minMTBmet = -999;// myTree.minMTBMet;
    int ID = myTree.evt_id;

    TLorentzVector *LVec = new TLorentzVector[3];
    for(int i=0; i< 2; i++){
      LVec[i].SetPtEtaPhiM(myTree.lep_pt[i], myTree.lep_eta[i],myTree.lep_phi[i], myTree.lep_mass[i]);
    }
    TLorentzVector Zvec = LVec[0] + LVec[1]; //leptons invariant mass

    Double_t weight(1.);

    //weights weights weights!
    if(isData){
      weight = 1.;
    }
    else{
      if (isETH) weight =  myTree.evt_xsec * myTree.evt_kfactor * myTree.evt_filter * 1000/nGen;
      else {
        weight = myTree.evt_scale1fb / (myTree.evt_xsec * myTree.evt_kfactor * myTree.evt_filter) * myTree.weight_lepsf * myTree.weight_btagsf;
        weight *= myTree.getXSecCorrWeight(sample.id, cfg.year());
        if (cfg.year()==2017) weight *= myTree.weight_L1prefire; // FIXME apply also to 2016 when weights are avaialble
        if ((sample.id==301 || sample.id==302 || sample.id==303) && cfg.year()==2016) weight *= myTree.weight_isr / myTree.getAverageISRWeight(sample.id,cfg.year(),0); // nominal
      }
    }

    if(ID >999)
      weight = 1000.*myTree.evt_xsec / nentries;


    //lepton scale factors
    //we apply the same scale factor on same and opposite CR
    if(!isData and isETH){
      //we apply the same weight for both leptons
      for(int i(0); i<2; ++i){
	if(abs(myTree.lep_pdgId[i])<12){ //electrons (lep_pdgID = +- 11)
	  if(electronHist){
	    lepSF elSFandError = leptonSF.getElSF(myTree.lep_pt[i], myTree.lep_eta[i]);
	    float elSF = elSFandError.sf;
	    weight *= elSF;
	  }
	}else if(abs(myTree.lep_pdgId[i])>12){//muons (lep_pdgID = +- 13)
	  if(muonHist){
	    lepSF muSFandError = leptonSF.getMuSF(myTree.lep_pt[i], myTree.lep_eta[i]);
	    float muSF = muSFandError.sf;
	    weight *= muSF;
	  }
	}
      }
    }
    // trigger scale factors
    float HLT_weight=1.;
    if (!isData && diLepTrigEffHist) {
      lepSF diLepTrigEffSF = leptonSF.getDiLepTriggerSF(myTree.lep_pt[0],lep0_pdgId_to_use,myTree.lep_pt[1],lep0_pdgId_to_use);
      HLT_weight = diLepTrigEffSF.sf;
      weight *= HLT_weight;
    }
    //float HLT_weight = getHLTweight( lep0_pdgId_to_use, lep1_pdgId_to_use, myTree.lep_pt[0], myTree.lep_pt[1], 0 );
    // variation -1 and +1 are for the weight up and down


    //b-tagging scale factor
    if(!isData and isETH){

      // declaration of the b-tagged weight
      float weight_btagsf = 1.;
      //declaration of the b-tagged weight uncertainty for heavy flavor (b or c)
      float weight_btagsf_heavy_UP = 1.;
      float weight_btagsf_heavy_DN = 1.;
      //declaration of the b-tagged weight uncertainty for light flavor
      float weight_btagsf_light_UP = 1.;
      float weight_btagsf_light_DN = 1.;

      bool isFastSim = false;

      bTagSF->get_weight_btag(myTree.nJet, myTree.jet_pt, myTree.jet_eta, myTree.jet_mcFlavour, myTree.jet_btagCSV, weight_btagsf, weight_btagsf_heavy_UP, weight_btagsf_heavy_DN, weight_btagsf_light_UP, weight_btagsf_light_DN , isFastSim);

      //cout << "nJet: " << myTree.nJet << " bTagSF: " << weight_btagsf << endl;

      weight *= weight_btagsf;
    }

    int nJetHF30_ = 0;
    int nJet_to_use = (isETH) ? myTree.nJet : myTree.njet;
    for(int j=0; j<nJet_to_use; ++j){
      if( myTree.jet_pt[j] < 30. || fabs(myTree.jet_eta[j]) < 3.0 ) continue;
      else ++nJetHF30_;
    }


    bool isSF = false;
    bool isOF = false;

    //SAME OR OPPOSITE FLAVOR selections
    if(  (lep0_pdgId_to_use == -lep1_pdgId_to_use) ) isSF = true;
    if( !(lep0_pdgId_to_use == -lep1_pdgId_to_use) ) isOF = true;

    if(isSF){ //////////SAME FLAVOR//////////////////////////////////////////
      //apply the triggers
      if(isData && isETH && !myTree.passTriggerSelection("zllSF", cfg.year()))continue;

      if(do_ZinvEst){
	      //SF part
	      if( fabs(myTree.zll_mass-91.19)>=20 ) continue;
	      if( myTree.zll_pt <= 200. ) continue;
      }
      if(invertedZcuts){
	if(fabs(myTree.zll_mass-91.19)<20) continue;
	if(myTree.zll_pt > 200.) continue;
	if(myTree.zll_mass < 50) continue;
      }
      /////////////////////////
      //FIXME: what to do with SF weights?
      // if( abs(myTree.lep_pdgId[0])==11 && myTree.lep_tightId[0]< 0.5 ) continue;
      //if( abs(myTree.lep_pdgId[1])==11 && myTree.lep_tightId[1]< 0.5 ) continue;


      MT2EstimateTree* thisTree;
      if( regionsSet=="zurich" || regionsSet=="zurichPlus" || regionsSet=="zurich2016" || regionsSet=="Moriond2019" ){ //
	      if( ht<450 || njets<7 || nbjets<1 ) {//Fill it the normal way

	        thisTree = anaTree->get( ht, njets, nbjets, minMTBmet, mt2 );

	        if( thisTree==0 ) continue;

	        thisTree->assignVar("ID", ID );

	        thisTree->assignVar("Z_pt", myTree.zll_pt );
	        thisTree->assignVar("Z_phi", Zvec.Phi() );
	        thisTree->assignVar("Z_eta", Zvec.Eta() );
		thisTree->assignVar("Z_mass", myTree.zll_mass );
		thisTree->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );
		thisTree->assignVar("nLep", nLep_to_be_used );
		thisTree->assignVar("lep_pdgId0", lep0_pdgId_to_use );
		thisTree->assignVar("lep_pdgId1", lep1_pdgId_to_use );
		thisTree->assignVar("lep_pt0", myTree.lep_pt[0] );
		thisTree->assignVar("lep_pt1", myTree.lep_pt[1] );
		thisTree->assignVar("lep_eta0", myTree.lep_eta[0] );
		thisTree->assignVar("lep_eta1", myTree.lep_eta[1] );
		thisTree->assignVar("lep_phi0", myTree.lep_phi[0] );
		thisTree->assignVar("lep_phi1", myTree.lep_phi[1] );
		thisTree->assignVar("raw_mt2", myTree.mt2 );
		thisTree->assignVar("raw_met", myTree.met_pt );
		//thisTree->assignVar("weight_lep0", weight_lep0);
		//thisTree->assignVar("weight_lep1", weight_lep1);
		//thisTree->assignVar("weight_lep_err", weight_lep_err);
		thisTree->assignVar("HLT_weight", HLT_weight );
		thisTree->assignVar("nJetHF30", nJetHF30_ );
		thisTree->assignVar("jet1_pt", myTree.jet1_pt );
		// thisTree->assignVar("lep_tightId0", myTree.lep_tightId[0] );
		//thisTree->assignVar("lep_tightId1", myTree.lep_tightId[1] );

		thisTree->assignVar("zll_met_pt", myTree.zll_met_pt );
		thisTree->assignVar("zll_met_phi", myTree.zll_met_phi );
		thisTree->assignVar("zll_mht_pt", myTree.zll_mht_pt );
		thisTree->assignVar("zll_mht_phi", myTree.zll_mht_phi );

		thisTree->fillTree_zll(myTree, weight );
		thisTree->yield->Fill( mt2, weight );


	      }else{

	        thisTree = anaTree->get( ht, njets, 1, minMTBmet, mt2 );
	        if( thisTree==0 ) continue;

	        thisTree->assignVar("ID", ID );

		thisTree->assignVar("Z_pt", myTree.zll_pt );
		thisTree->assignVar("Z_phi", Zvec.Phi() );
		thisTree->assignVar("Z_eta", Zvec.Eta() );
		thisTree->assignVar("Z_mass", myTree.zll_mass );
		thisTree->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

		thisTree->assignVar("nLep", nLep_to_be_used );
		thisTree->assignVar("lep_pdgId0", lep0_pdgId_to_use);
		thisTree->assignVar("lep_pdgId1",lep1_pdgId_to_use );
		thisTree->assignVar("lep_pt0", myTree.lep_pt[0] );
		thisTree->assignVar("lep_pt1", myTree.lep_pt[1] );
		thisTree->assignVar("lep_eta0", myTree.lep_eta[0] );
		thisTree->assignVar("lep_eta1", myTree.lep_eta[1] );
		thisTree->assignVar("lep_phi0", myTree.lep_phi[0] );
		thisTree->assignVar("lep_phi1", myTree.lep_phi[1] );
		thisTree->assignVar("raw_mt2", myTree.mt2 );
		thisTree->assignVar("raw_met", myTree.met_pt );

		// thisTree->assignVar("weight_lep0", weight_lep0);
		//thisTree->assignVar("weight_lep1", weight_lep1);
		//thisTree->assignVar("weight_lep_err", weight_lep_err);
		thisTree->assignVar("HLT_weight", HLT_weight );

		thisTree->assignVar("nJetHF30", nJetHF30_ );
		thisTree->assignVar("jet1_pt", myTree.jet1_pt );
		//thisTree->assignVar("lep_tightId0", myTree.lep_tightId[0] );
		//thisTree->assignVar("lep_tightId1", myTree.lep_tightId[1] );

		thisTree->assignVar("zll_met_pt", myTree.zll_met_pt );
		thisTree->assignVar("zll_met_phi", myTree.zll_met_phi );
		thisTree->assignVar("zll_mht_pt", myTree.zll_mht_pt );
		thisTree->assignVar("zll_mht_phi", myTree.zll_mht_phi );

		thisTree->fillTree_zll(myTree, weight );
		thisTree->yield->Fill( mt2, weight );

		thisTree = anaTree->get( ht, njets, 2, minMTBmet, mt2 );
		if( thisTree==0 ) continue;

		thisTree->assignVar("Z_pt", myTree.zll_pt );
		thisTree->assignVar("Z_phi", Zvec.Phi() );
		thisTree->assignVar("Z_eta", Zvec.Eta() );
		thisTree->assignVar("Z_mass", myTree.zll_mass );
		thisTree->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

		thisTree->assignVar("nLep", nLep_to_be_used );
		thisTree->assignVar("lep_pdgId0", lep0_pdgId_to_use );
		thisTree->assignVar("lep_pdgId1", lep1_pdgId_to_use );
		thisTree->assignVar("lep_pt0", myTree.lep_pt[0] );
		thisTree->assignVar("lep_pt1", myTree.lep_pt[1] );
		thisTree->assignVar("lep_eta0", myTree.lep_eta[0] );
		thisTree->assignVar("lep_eta1", myTree.lep_eta[1] );
		thisTree->assignVar("lep_phi0", myTree.lep_phi[0] );
		thisTree->assignVar("lep_phi1", myTree.lep_phi[1] );
		thisTree->assignVar("raw_mt2", myTree.mt2 );
		thisTree->assignVar("raw_met", myTree.met_pt );

		//thisTree->assignVar("weight_lep0", weight_lep0);
		//thisTree->assignVar("weight_lep1", weight_lep1);
		//thisTree->assignVar("weight_lep_err", weight_lep_err);
		thisTree->assignVar("HLT_weight", HLT_weight );

		thisTree->assignVar("nJetHF30", nJetHF30_ );
		thisTree->assignVar("jet1_pt", myTree.jet1_pt );
		// thisTree->assignVar("lep_tightId0", myTree.lep_tightId[0] );
		//thisTree->assignVar("lep_tightId1", myTree.lep_tightId[1] );

		thisTree->assignVar("zll_met_pt", myTree.zll_met_pt );
		thisTree->assignVar("zll_met_phi", myTree.zll_met_phi );
		thisTree->assignVar("zll_mht_pt", myTree.zll_mht_pt );
		thisTree->assignVar("zll_mht_phi", myTree.zll_mht_phi );

		thisTree->fillTree_zll(myTree, weight );
		thisTree->yield->Fill( mt2, weight );

		thisTree = anaTree->get( ht, njets, 3, minMTBmet, mt2 );
		if( thisTree==0 ) continue;

		thisTree->assignVar("Z_pt", myTree.zll_pt );
		thisTree->assignVar("Z_phi", Zvec.Phi() );
		thisTree->assignVar("Z_eta", Zvec.Eta() );
		thisTree->assignVar("Z_mass", myTree.zll_mass );
		thisTree->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

		thisTree->assignVar("nLep", nLep_to_be_used );
		thisTree->assignVar("lep_pdgId0", lep0_pdgId_to_use);
		thisTree->assignVar("lep_pdgId1", lep1_pdgId_to_use );
		thisTree->assignVar("lep_pt0", myTree.lep_pt[0] );
		thisTree->assignVar("lep_pt1", myTree.lep_pt[1] );
		thisTree->assignVar("lep_eta0", myTree.lep_eta[0] );
		thisTree->assignVar("lep_eta1", myTree.lep_eta[1] );
		thisTree->assignVar("lep_phi0", myTree.lep_phi[0] );
		thisTree->assignVar("lep_phi1", myTree.lep_phi[1] );
		thisTree->assignVar("raw_mt2", myTree.mt2 );
		thisTree->assignVar("raw_met", myTree.met_pt );

		//thisTree->assignVar("weight_lep0", weight_lep0);
		//thisTree->assignVar("weight_lep1", weight_lep1);
		//thisTree->assignVar("weight_lep_err", weight_lep_err);
		thisTree->assignVar("HLT_weight", HLT_weight );

		thisTree->assignVar("nJetHF30", nJetHF30_ );
		thisTree->assignVar("jet1_pt", myTree.jet1_pt );
		//thisTree->assignVar("lep_tightId0", myTree.lep_tightId[0] );
		//thisTree->assignVar("lep_tightId1", myTree.lep_tightId[1] );

		thisTree->assignVar("zll_met_pt", myTree.zll_met_pt );
		thisTree->assignVar("zll_met_phi", myTree.zll_met_phi );
		thisTree->assignVar("zll_mht_pt", myTree.zll_mht_pt );
		thisTree->assignVar("zll_mht_phi", myTree.zll_mht_phi );

		thisTree->fillTree_zll(myTree, weight );
		thisTree->yield->Fill( mt2, weight );

	      } // end kinematic condition
      } else { // region condition

        thisTree = anaTree->get( ht, njets, nbjets, minMTBmet, mt2 );
        if( thisTree==0 ) continue;

        thisTree->assignVar("ID", ID );

       //      thisTree->assignVar("Z_pt", z.Perp() );
        thisTree->assignVar("Z_pt", myTree.zll_pt );
        thisTree->assignVar("Z_phi", Zvec.Phi() );
        thisTree->assignVar("Z_eta", Zvec.Eta() );
        //      thisTree->assignVar("Z_mass", Zvec.M() );
        thisTree->assignVar("Z_mass", myTree.zll_mass );
        thisTree->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

        thisTree->assignVar("nLep", nLep_to_be_used );
        thisTree->assignVar("lep_pdgId0", lep0_pdgId_to_use );
        thisTree->assignVar("lep_pdgId1", lep1_pdgId_to_use );
        thisTree->assignVar("lep_pt0", myTree.lep_pt[0] );
        thisTree->assignVar("lep_pt1", myTree.lep_pt[1] );
        thisTree->assignVar("lep_eta0", myTree.lep_eta[0] );
        thisTree->assignVar("lep_eta1", myTree.lep_eta[1] );
        thisTree->assignVar("lep_phi0", myTree.lep_phi[0] );
        thisTree->assignVar("lep_phi1", myTree.lep_phi[1] );
        thisTree->assignVar("raw_mt2", myTree.mt2 );
        thisTree->assignVar("raw_met", myTree.met_pt );

        //thisTree->assignVar("weight_lep0", weight_lep0);
        //thisTree->assignVar("weight_lep1", weight_lep1);
        //thisTree->assignVar("weight_lep_err", weight_lep_err);
        thisTree->assignVar("HLT_weight", HLT_weight );

        thisTree->assignVar("nJetHF30", nJetHF30_ );
        thisTree->assignVar("jet1_pt", myTree.jet1_pt );
	// thisTree->assignVar("lep_tightId0", myTree.lep_tightId[0] );
        //thisTree->assignVar("lep_tightId1", myTree.lep_tightId[1] );

	thisTree->assignVar("zll_met_pt", myTree.zll_met_pt );
        thisTree->assignVar("zll_met_phi", myTree.zll_met_phi );
	thisTree->assignVar("zll_mht_pt", myTree.zll_mht_pt );
        thisTree->assignVar("zll_mht_phi", myTree.zll_mht_phi );

        thisTree->fillTree_zll(myTree, weight );
        thisTree->yield->Fill( mt2, weight );

      }
    } else if(isOF){ //////////Opposite FLAVOR//////////////////////////////////////////
      //we apply the trigger
      if(isData && !myTree.passTriggerSelection("zllOF", cfg.year())) continue;
      if(do_ZinvEst){
	if( fabs(myTree.zll_mass-91.19)>=20. ) continue;
	if( myTree.zll_pt <= 200. ) continue;
      }
      if(invertedZcuts){
	if(fabs(myTree.zll_mass-91.19)<20) continue;
	if(myTree.zll_pt > 200.) continue;
	if(myTree.zll_mass < 50) continue;
      }

      // FIXME: what to do with lep_tightID?
      //      if( abs(myTree.lep_pdgId[0])==11 && myTree.lep_tightId[0]< 0.5 ) continue;
      //if( abs(myTree.lep_pdgId[1])==11 && myTree.lep_tightId[1]< 0.5 ) continue;

      //      if( !myTree.isData){
      //	      weight *= myTree.weight_btagsf * myTree.weight_lepsf * HLT_weight;
      //}

      MT2EstimateTree* thisTree_of;
      if( regionsSet=="zurich" || regionsSet=="zurichPlus" || regionsSet=="zurich2016" || regionsSet=="Moriond19" ){ //
	      if( ht<450 || njets<7 || nbjets<1 ) {//Fill it the normal way

	        thisTree_of = anaTree_of->get( ht, njets, nbjets, minMTBmet, mt2 );
	        if( thisTree_of==0 ) continue;

	        thisTree_of->assignVar("ID", sample.id );

	        thisTree_of->assignVar("Z_pt", myTree.zll_pt );
	        thisTree_of->assignVar("Z_phi", Zvec.Phi() );
	        thisTree_of->assignVar("Z_eta", Zvec.Eta() );
	        thisTree_of->assignVar("Z_mass", myTree.zll_mass );
	        thisTree_of->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

	        thisTree_of->assignVar("nLep", nLep_to_be_used );
	        thisTree_of->assignVar("lep_pdgId0", lep0_pdgId_to_use );
	        thisTree_of->assignVar("lep_pdgId1", lep1_pdgId_to_use );
	        thisTree_of->assignVar("lep_pt0", myTree.lep_pt[0] );
	        thisTree_of->assignVar("lep_pt1", myTree.lep_pt[1] );
	        thisTree_of->assignVar("lep_eta0", myTree.lep_eta[0] );
	        thisTree_of->assignVar("lep_eta1", myTree.lep_eta[1] );
	        thisTree_of->assignVar("lep_phi0", myTree.lep_phi[0] );
	        thisTree_of->assignVar("lep_phi1", myTree.lep_phi[1] );
	        thisTree_of->assignVar("raw_mt2", myTree.mt2 );
	        thisTree_of->assignVar("raw_met", myTree.met_pt );

		// thisTree_of->assignVar("weight_lep0", weight_lep0);
	        //thisTree_of->assignVar("weight_lep1", weight_lep1);
	        //thisTree_of->assignVar("weight_lep_err", weight_lep_err);

	        thisTree_of->assignVar( "nJetHF30", nJetHF30_ );
	        thisTree_of->assignVar( "jet1_pt", myTree.jet1_pt );

	        //thisTree_of->assignVar("lep_tightId0", myTree.lep_tightId[0] );
	        //thisTree_of->assignVar("lep_tightId1", myTree.lep_tightId[1] );

		thisTree_of->assignVar("zll_met_pt", myTree.zll_met_pt );
		thisTree_of->assignVar("zll_met_phi", myTree.zll_met_phi );
		thisTree_of->assignVar("zll_mht_pt", myTree.zll_mht_pt );
		thisTree_of->assignVar("zll_mht_phi", myTree.zll_mht_phi );

	        thisTree_of->fillTree_zll(myTree, weight );
	        thisTree_of->yield->Fill( mt2, weight );

	      }else{

	        thisTree_of = anaTree_of->get( ht, njets, 1, minMTBmet, mt2 );
	        if( thisTree_of==0 ) continue;

	        thisTree_of->assignVar("ID", sample.id );

	        thisTree_of->assignVar("Z_pt", myTree.zll_pt );
	        thisTree_of->assignVar("Z_phi", Zvec.Phi() );
	        thisTree_of->assignVar("Z_eta", Zvec.Eta() );
	        thisTree_of->assignVar("Z_mass", myTree.zll_mass );
	        thisTree_of->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

	        thisTree_of->assignVar("nLep", nLep_to_be_used );
	        thisTree_of->assignVar("lep_pdgId0", lep0_pdgId_to_use );
	        thisTree_of->assignVar("lep_pdgId1", lep1_pdgId_to_use );
	        thisTree_of->assignVar("lep_pt0", myTree.lep_pt[0] );
	        thisTree_of->assignVar("lep_pt1", myTree.lep_pt[1] );
	        thisTree_of->assignVar("lep_eta0", myTree.lep_eta[0] );
	        thisTree_of->assignVar("lep_eta1", myTree.lep_eta[1] );
	        thisTree_of->assignVar("lep_phi0", myTree.lep_phi[0] );
	        thisTree_of->assignVar("lep_phi1", myTree.lep_phi[1] );
	        thisTree_of->assignVar("raw_mt2", myTree.mt2 );
	        thisTree_of->assignVar("raw_met", myTree.met_pt );

		// thisTree_of->assignVar("weight_lep0", weight_lep0);
	        //thisTree_of->assignVar("weight_lep1", weight_lep1);
	        //thisTree_of->assignVar("weight_lep_err", weight_lep_err);

	        thisTree_of->assignVar( "nJetHF30", nJetHF30_ );
	        thisTree_of->assignVar( "jet1_pt", myTree.jet1_pt );

		// thisTree_of->assignVar("lep_tightId0", myTree.lep_tightId[0] );
	        //thisTree_of->assignVar("lep_tightId1", myTree.lep_tightId[1] );

		thisTree_of->assignVar("zll_met_pt", myTree.zll_met_pt );
		thisTree_of->assignVar("zll_met_phi", myTree.zll_met_phi );
		thisTree_of->assignVar("zll_mht_pt", myTree.zll_mht_pt );
		thisTree_of->assignVar("zll_mht_phi", myTree.zll_mht_phi );

	        thisTree_of->fillTree_zll(myTree, weight );
	        thisTree_of->yield->Fill( mt2, weight );

	        thisTree_of = anaTree_of->get( ht, njets, 2, minMTBmet, mt2 );
	        if( thisTree_of==0 ) continue;
	        thisTree_of->assignVar("ID", sample.id );

	        thisTree_of->assignVar("Z_pt", myTree.zll_pt );
	        thisTree_of->assignVar("Z_phi", Zvec.Phi() );
	        thisTree_of->assignVar("Z_eta", Zvec.Eta() );
	        thisTree_of->assignVar("Z_mass", myTree.zll_mass );
	        thisTree_of->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

	        thisTree_of->assignVar("nLep", nLep_to_be_used );
	        thisTree_of->assignVar("lep_pdgId0", lep0_pdgId_to_use );
	        thisTree_of->assignVar("lep_pdgId1", lep1_pdgId_to_use);
	        thisTree_of->assignVar("lep_pt0", myTree.lep_pt[0] );
	        thisTree_of->assignVar("lep_pt1", myTree.lep_pt[1] );
	        thisTree_of->assignVar("lep_eta0", myTree.lep_eta[0] );
	        thisTree_of->assignVar("lep_eta1", myTree.lep_eta[1] );
	        thisTree_of->assignVar("lep_phi0", myTree.lep_phi[0] );
	        thisTree_of->assignVar("lep_phi1", myTree.lep_phi[1] );
	        thisTree_of->assignVar("raw_mt2", myTree.mt2 );
	        thisTree_of->assignVar("raw_met", myTree.met_pt );

		// thisTree_of->assignVar("weight_lep0", weight_lep0);
	        //thisTree_of->assignVar("weight_lep1", weight_lep1);
	        //thisTree_of->assignVar("weight_lep_err", weight_lep_err);

	        thisTree_of->assignVar( "nJetHF30", nJetHF30_ );
	        thisTree_of->assignVar( "jet1_pt", myTree.jet1_pt );

		//  thisTree_of->assignVar("lep_tightId0", myTree.lep_tightId[0] );
	        //thisTree_of->assignVar("lep_tightId1", myTree.lep_tightId[1] );

		thisTree_of->assignVar("zll_met_pt", myTree.zll_met_pt );
		thisTree_of->assignVar("zll_met_phi", myTree.zll_met_phi );
		thisTree_of->assignVar("zll_mht_pt", myTree.zll_mht_pt );
		thisTree_of->assignVar("zll_mht_phi", myTree.zll_mht_phi );

	        thisTree_of->fillTree_zll(myTree, weight );
	        thisTree_of->yield->Fill( mt2, weight );

	        thisTree_of = anaTree_of->get( ht, njets, 3, minMTBmet, mt2 );
	        if( thisTree_of==0 ) continue;

	        thisTree_of->assignVar("ID", sample.id );

	        thisTree_of->assignVar("Z_pt", myTree.zll_pt );
	        thisTree_of->assignVar("Z_phi", Zvec.Phi() );
	        thisTree_of->assignVar("Z_eta", Zvec.Eta() );
	        thisTree_of->assignVar("Z_mass", myTree.zll_mass );
	        thisTree_of->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

	        thisTree_of->assignVar("nLep", nLep_to_be_used );
	        thisTree_of->assignVar("lep_pdgId0", lep0_pdgId_to_use );
	        thisTree_of->assignVar("lep_pdgId1", lep1_pdgId_to_use );
	        thisTree_of->assignVar("lep_pt0", myTree.lep_pt[0] );
	        thisTree_of->assignVar("lep_pt1", myTree.lep_pt[1] );
	        thisTree_of->assignVar("lep_eta0", myTree.lep_eta[0] );
	        thisTree_of->assignVar("lep_eta1", myTree.lep_eta[1] );
	        thisTree_of->assignVar("lep_phi0", myTree.lep_phi[0] );
	        thisTree_of->assignVar("lep_phi1", myTree.lep_phi[1] );
	        thisTree_of->assignVar("raw_mt2", myTree.mt2 );
	        thisTree_of->assignVar("raw_met", myTree.met_pt );

		thisTree_of->assignVar("zll_met_pt", myTree.zll_met_pt );
		// thisTree_of->assignVar("weight_lep0", weight_lep0);
	        //thisTree_of->assignVar("weight_lep1", weight_lep1);
	        //thisTree_of->assignVar("weight_lep_err", weight_lep_err);

	        thisTree_of->assignVar( "nJetHF30", nJetHF30_ );
	        thisTree_of->assignVar( "jet1_pt", myTree.jet1_pt );

	        //thisTree_of->assignVar("lep_tightId0", myTree.lep_tightId[0] );
	        //thisTree_of->assignVar("lep_tightId1", myTree.lep_tightId[1] );

		thisTree_of->assignVar("zll_met_phi", myTree.zll_met_phi );
		thisTree_of->assignVar("zll_mht_pt", myTree.zll_mht_pt );
		thisTree_of->assignVar("zll_mht_phi", myTree.zll_mht_phi );

	        thisTree_of->fillTree_zll(myTree, weight );
	        thisTree_of->yield->Fill( mt2, weight );

	      }
      } else {

	      thisTree_of = anaTree_of->get( ht, njets, nbjets, minMTBmet, mt2 );
	      if( thisTree_of==0 ) continue;

	      thisTree_of->assignVar("ID", sample.id );

	      thisTree_of->assignVar("Z_pt", myTree.zll_pt );
	      thisTree_of->assignVar("Z_phi", Zvec.Phi() );
	      thisTree_of->assignVar("Z_eta", Zvec.Eta() );
	      thisTree_of->assignVar("Z_mass", myTree.zll_mass );
	      thisTree_of->assignVar("Z_lepId", abs(lep0_pdgId_to_use) );

	      thisTree_of->assignVar("nLep", nLep_to_be_used );
	      thisTree_of->assignVar("lep_pdgId0", lep0_pdgId_to_use );
	      thisTree_of->assignVar("lep_pdgId1", lep1_pdgId_to_use);
	      thisTree_of->assignVar("lep_pt0", myTree.lep_pt[0] );
	      thisTree_of->assignVar("lep_pt1", myTree.lep_pt[1] );
	      thisTree_of->assignVar("lep_eta0", myTree.lep_eta[0] );
	      thisTree_of->assignVar("lep_eta1", myTree.lep_eta[1] );
	      thisTree_of->assignVar("lep_phi0", myTree.lep_phi[0] );
	      thisTree_of->assignVar("lep_phi1", myTree.lep_phi[1] );
	      thisTree_of->assignVar("raw_mt2", myTree.mt2 );
	      thisTree_of->assignVar("raw_met", myTree.met_pt );

	      //thisTree_of->assignVar("weight_lep0", weight_lep0);
	      //thisTree_of->assignVar("weight_lep1", weight_lep1);
	      //thisTree_of->assignVar("weight_lep_err", weight_lep_err);

	      thisTree_of->assignVar( "nJetHF30", nJetHF30_ );
	      thisTree_of->assignVar( "jet1_pt", myTree.jet1_pt );

	      // thisTree_of->assignVar("lep_tightId0", myTree.lep_tightId[0] );
	      //thisTree_of->assignVar("lep_tightId1", myTree.lep_tightId[1] );

	      thisTree_of->assignVar("zll_met_pt", myTree.zll_met_pt );
	      thisTree_of->assignVar("zll_met_phi", myTree.zll_met_phi );
	      thisTree_of->assignVar("zll_mht_pt", myTree.zll_mht_pt );
	      thisTree_of->assignVar("zll_mht_phi", myTree.zll_mht_phi );

	      thisTree_of->fillTree_zll(myTree, weight );
	      thisTree_of->yield->Fill( mt2, weight );

      }
    } else continue;

  } // for entries
  // delete bTagSF;
  //bTagSF = 0;

  //bTagSF.~MT2BTagSFHelper();
  anaTree->finalize();
  anaTree_of->finalize();

  delete tree;




  file->Close();
  delete file;



}



/* OUTDATED
float getHLTweight( int lep1_pdgId, int lep2_pdgId, float lep1_pt, float lep2_pt, int variation ){

  float SF = 1.0;

  //di Muon
  if( abs(lep1_pdgId)==13 && abs(lep2_pdgId)==13 )
    SF = ( variation==0) ? 0.97 : (( variation == -1 ) ? 0.94 : 1.00  );
  //di Electron
  else if( abs(lep1_pdgId)==11 && abs(lep2_pdgId)==11)
    if (lep1_pt < 180. && lep2_pt < 35. )
      SF = ( variation==0) ? 0.91 : (( variation == -1 ) ? 0.84 : 1.00 );
    else
      SF = ( variation==0) ? 1.00 : (( variation == -1 ) ? 0.97 : 1.00 );
  else
    if (lep1_pt < 180.)
      SF = ( variation==0 ) ? 0.92 : (( variation == -1 ) ? 0.89 : 0.95 );
    else
      SF = ( variation==0 ) ? 0.97 : (( variation == -1 ) ? 0.92 : 1.00 );

  return SF;

}
*/
