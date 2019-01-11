  
// Script to perform the estimates of the lostlepton control region

#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>

#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TGraphAsymmErrors.h"
#include "TLorentzVector.h"

#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2EstimateTree.h"
#include "interface/MT2EstimateSigSyst.h"
#include "interface/MT2EstimateAllSigSyst.h"
#include "interface/MT2DrawTools.h"
#include "interface/MT2Config.h"

#include "interface/Utils.h"
#include "interface/MT2LeptonSFTool.h"
#include "interface/MT2BTagSFHelper.h"

#include "TRandom3.h"

using namespace std;

#define mt2_cxx
#include "interface/mt2.h"

int roundD(float d) {
  return (int)(floor(d + 0.5));
}

void computeYield( const MT2Sample& sample, const MT2Config& cfg, MT2Analysis<MT2EstimateTree>* anaTree, MT2BTagSFHelper* bTagSF );
void roundLikeData( MT2Analysis<MT2EstimateTree>* data );

float DeltaR(float eta1, float eta2, float phi1, float phi2);
float DeltaPhi(float phi1, float phi2);

template <class T>
MT2Analysis<T>* computeSigYield( const MT2Sample& sample, const MT2Config& cfg );



int main( int argc, char* argv[] ) {
 

  std::cout << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|           Running llepControlRegion                |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc < 2 ) {
    std::cout << "USAGE: ./llepControlRegion [configFileName] [data/mc]" << std::endl;
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

  bool onlyData = false;
  bool onlyMC   = false;
  bool onlySignal = false;
  if( argc > 2 ) {
    std::string dataMC(argv[2]);
    if( dataMC=="data" ) onlyData = true;
    else if( dataMC=="MC" || dataMC=="mc" ) onlyMC = true;
    else if( dataMC=="signal" ) onlySignal = true;
    else {
      std::cout << "-> You passed a second argument that isn't 'data', nor 'MC', nor 'signal', so I don't know what to do about it." << std::endl;
    }
  }
  std::string outputdir = cfg.getEventYieldDir() + "/llepControlRegion";
  system(Form("mkdir -p %s", outputdir.c_str()));

  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

  std::string regionsSet = cfg.regionsSet();
  std::cout << "Using region set: " << regionsSet << std::endl;




  // ********************
  // Do analysis on MC bkg
  // ********************
  if( cfg.useMC() && !onlyData && !onlySignal ) { // use MC BG estimates

    std::string samplesFileName = "../samples/samples_" + cfg.mcSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;
    std::cout << "useETHMC()" << cfg.useETHmc() << std::endl;
    std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 300, 599, cfg.useETHmc()); // only top (tt, t, ttW, ttZ) and W+jets
    if( fSamples.size()==0 ) {
      std::cout << "There must be an error: samples is empty!" << std::endl;
      exit(1209);
    }

   
    MT2Analysis<MT2EstimateTree>* mcCR = new MT2Analysis<MT2EstimateTree> ( "llepCR", regionsSet ); // name given here is the name of the parent directory in the output file
    for( unsigned i=0; i < fSamples.size(); ++i ){
      MT2BTagSFHelper* bTagSF = new MT2BTagSFHelper();
      computeYield( fSamples[i], cfg, mcCR, bTagSF );
      bTagSF = nullptr;
    }
    mcCR->writeToFile( outputdir + "/mc.root" );
    if( cfg.dummyAnalysis() ) {
      // emulate data:
      roundLikeData(mcCR);
      mcCR->writeToFile( outputdir + "/data.root" );
    }
  }
  // ********************
  // Do analysis on signal samples, if any
  // ********************
  std::vector< MT2Analysis< MT2EstimateAllSigSyst >* > signals;

  if( cfg.sigSamples()!="" && cfg.additionalStuff()!="noSignals" && !onlyData ) { // Take signals from a different sample file, compulsory, otherwise no signals analysis

    std::string samplesFileName = "../samples/samples_" + cfg.sigSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading signal samples from file: " << samplesFileName << std::endl;

    std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 1000, -1, cfg.useETHmc()); // only signal (id>=1000)


    if( fSamples.size()==0 ) {

      std::cout << "No signal samples found, skipping." << std::endl;

    } else {

      for( unsigned i=0; i<fSamples.size(); ++i )
        //signals.push_back( computeSigYield<MT2EstimateAllSigSyst>( fSamples[i], cfg ) );

      if( signals.size()>0 ){

	signals[0]->writeToFile( outputdir + "/mc.root" );
	for( unsigned i=1; i<signals.size(); ++i )
	  signals[i]->writeToFile( outputdir + "/mc.root" );
      }

    } // if samples != 0

  } // if mc samples

  // ********************
  // Do analysis on data
  // ********************
  if( !(cfg.dummyAnalysis()) && cfg.dataSamples()!="" && !onlyMC && !onlySignal) {

    std::string samplesFile_data = "../samples/samples_" + cfg.dataSamples() + ".dat";

    std::cout << std::endl << std::endl;
    std::cout << "-> Loading data from file: " << samplesFile_data << std::endl;
    //std::cout << " cfg.useETHdata()" <<  cfg.useETHdata() << std::endl;

    std::vector<MT2Sample> samples_data = MT2Sample::loadSamples(samplesFile_data, "", 1, 100, cfg.useETHdata() ); // only data samples

    if( samples_data.size()==0 ) {
      std::cout << "There must be an error: samples_data is empty!" << std::endl;
      exit(1209);
    }


    TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

    MT2Analysis<MT2EstimateTree>* dataCR = new MT2Analysis<MT2EstimateTree> ( "llepCR", regionsSet );

    for( unsigned i=0; i < samples_data.size(); ++i ){
      MT2BTagSFHelper* bTagSF_data = new MT2BTagSFHelper();
      computeYield( samples_data[i], cfg, dataCR, bTagSF_data );
      bTagSF_data = nullptr;
    }

    dataCR->writeToFile( outputdir + "/data.root" );

  } // end analysis on data
    

  return 0;

}


void computeYield( const MT2Sample& sample, const MT2Config& cfg, MT2Analysis<MT2EstimateTree>* anaTree, MT2BTagSFHelper* bTagSF ){

  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  //initialization of the lepton scale factor tool
  MT2LeptonSFTool leptonSF;

  // determine if it's data or mc here
  bool  isData = (sample.id >= 1 && sample.id < 100 );
  std::cout << " sample.id=" << sample.id << " isData=" << isData << std::endl;

  // determine if it is an ETH kind of ntuple or not
  bool isETH = (isData and cfg.useETHdata()) || (!isData and cfg.useETHmc());

  // Tree initialization
  TString treeName = isETH ? "Events" : "mt2";
  TFile* file = TFile::Open(sample.file.c_str());
  TTree* tree = (TTree*)file->Get(treeName);

  MT2Tree myTree;
  myTree.Init(tree);

  std::string regionsSet = cfg.regionsSet();
  std::cout << "Using region set: " << regionsSet << std::endl;

  // Sum of weights
  double nGen=-9999; double nGenWeighted=-9999;
  if(!isData and isETH){
    nGen = getNgen(sample.file, "genEventCount");
    nGenWeighted = getNgen(sample.file, "genEventSumw");
  } 
 
  int nentries = tree->GetEntries();

  //for( int iEntry=0; iEntry<50000; ++iEntry ) {
  for( int iEntry=0; iEntry<nentries; ++iEntry ) {
    if(iEntry % 50000 == 0){
      std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;
    }
 
    if(iEntry == nentries){
      cout << "finish loop on entries for this sample" << endl;
    }
    
    myTree.GetEntry(iEntry);  

    // Do the selection here: please try to keep a consistent order
    // between this script and similar scripts

    // if( myTree.isData && !myTree.isGolden ) continue;
   
    
    // apply the filters
    // filters should be the same bw ETH and SnT
    if(isData) {
      if(!myTree.passFilters(cfg.year())) continue;
    } else {
      if(!myTree.passFiltersMC(cfg.year())) continue;
    } 
     
    // apply the triggers
    if(isData and isETH) {
      if (!myTree.passTriggerSelection("llep", cfg.year())) continue;
    }

    // apply good vertex cut once for all 
    if (isETH) {
      if(myTree.PV_npvs <= 0) continue;
    } else {
      if(myTree.nVert <= 0) continue;
    }
    
    // some additional cleanings
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
    
    // apply the main kinematic selections here
    if( !myTree.passBaselineKinematic("",cfg.year())) continue;
    
 
    // monojet id
    if ( myTree.nJet30==1 && !myTree.passMonoJetId(0) ) continue;
    
    // apply HEM veto
    if (!myTree.passHEMFailVeto(cfg.year(), isETH)) continue; 

    //cut on HEM fail for 2018 data <= this needs to be reworked
    //if(cfg.year() == 2018){
      //if(myTree.nJet30HEMFail != 0) continue;
    //} 

    // apply specific analysis region cuts: we require strictly only one lepton in this CR
    if( myTree.nLepLowMT!=1 ) continue; 
    //new cut: we ask specifically the number of leptons with high MT to be zero
    if(myTree.nLepHighMT != 0) continue;
    

    // Selection is over, now apply the weights !
    Double_t weight(1.);
  
    //weight on the cross section  
    //Double_t weight = (myTree.isData) ? 1. : myTree.evt_scale1fb;//*cfg.lumi();
    if(isData){
      weight = 1.;
    }
    else{
      if (isETH) weight =  myTree.evt_xsec * myTree.evt_kfactor * myTree.evt_filter * 1000/nGen;
      else weight = myTree.evt_scale1fb * myTree.weight_lepsf * myTree.weight_btagsf;
    }
    
    //lepton scale factor (only on !data and isETH)
    if(!isData and isETH){
      //MT2LeptonSFTool leptonSF;
      if(abs(myTree.lep_pdgId[0])<12){ //electrons (lep_pdgID = +- 11)
	bool electronHist = leptonSF.setElHist("llep"); //checks if all the electron sf files can be loaded
	if(electronHist){
	  lepSF elSFandError = leptonSF.getElSF(myTree.lep_pt[0], myTree.lep_eta[0]);
	  float elSF = elSFandError.sf;
	  weight *= elSF;
	  //cout << "je suis un electron" << endl;
	}
      }else if(abs(myTree.lep_pdgId[0])>12){//muons (lep_pdgID = +- 13)
	bool muonHist = leptonSF.setMuHist(); //checks if all the muon sf files can be loaded
	if(muonHist){
	  lepSF muSFandError = leptonSF.getMuSF(myTree.lep_pt[0], myTree.lep_eta[0]);
	  float muSF = muSFandError.sf;
	  weight *= muSF;
	  //cout << "je suis un muon" << endl;
	}
      }
    }
    
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
    
    // ISR weights , ETH does not have
    //if(!isData and !isETH){
    //  weight *= myTree.b_weight_isr;
    //}


   /* if( !myTree.isData ){
      weight *= myTree.weight_btagsf;
      weight *= myTree.weight_lepsf;
      //      weight *= myTree.weight_lepsf;

      // // ETH has a branch witht he average weight stored:
      // // Also we have a different numbering scheme...
      // if (myTree.evt_id == 302 || myTree.evt_id == 303 || myTree.evt_id == 304) //singleLep T/Tbar, Dilep
      // 	weight *= myTree.weight_isr / myTree.weight_isr_norm;

      ///AMERICAN WAY
      if (myTree.evt_id == 301 || myTree.evt_id == 302)
      	weight *= myTree.weight_isr/0.909; // nominal
      else if (myTree.evt_id == 303)
      	weight *= myTree.weight_isr/0.895;

    }*/

    // Now that you have done the pre-selection
    // you can actually make the estimates
    // define here the variables that will enter the region categorization

    

    int njets  = myTree.nJet30;
    int nbjets = myTree.nBJet20;
    float ht   = (njets>1) ? myTree.ht : myTree.jet1_pt;
    float mt2  = (njets>1) ? myTree.mt2 : ht;
    float minMTBmet = -999;  //myTree.minMTBMet;
    //we add lepton kinematics parameters on the tree
    
   
    MT2EstimateTree* thisEstimate;

    if( regionsSet=="zurich" || regionsSet=="zurichPlus" || regionsSet=="zurich2016" ){ // To avoid signal contamination in 7j 2b and 7j 3b
      if( ht>450. && njets>=7 && nbjets>2 ) continue;
      else if( ht<450 || njets<7 || nbjets<1 ) {
	thisEstimate = anaTree->get( ht, njets, nbjets, minMTBmet, mt2 );
	if( thisEstimate==0 ) continue;

	thisEstimate->assignTree( myTree, weight );
	thisEstimate->tree->Fill();
	thisEstimate->yield->Fill(mt2, weight );
      }
      else {
	thisEstimate = anaTree->get( ht, njets, 1, minMTBmet, mt2 );
	cout << "[Ht] " << ht << endl;
	if( thisEstimate==0 ) continue;
	thisEstimate->assignTree( myTree, weight );
	thisEstimate->tree->Fill();
	thisEstimate->yield->Fill(mt2, weight );

	thisEstimate = anaTree->get( ht, njets, 2, minMTBmet, mt2 );
	if( thisEstimate==0 ) continue;
	thisEstimate->assignTree( myTree, weight );
	thisEstimate->tree->Fill();
	thisEstimate->yield->Fill(mt2, weight );

	thisEstimate = anaTree->get( ht, njets, 3, minMTBmet, mt2 );
	if( thisEstimate==0 ) continue;
	thisEstimate->assignTree( myTree, weight );
	thisEstimate->tree->Fill();
	thisEstimate->yield->Fill(mt2, weight );
      }
    }
    else {
	thisEstimate = anaTree->get( ht, njets, nbjets, minMTBmet, mt2 );
	if( thisEstimate==0 ) continue;

	thisEstimate->assignTree( myTree, weight );
	thisEstimate->tree->Fill();
	thisEstimate->yield->Fill(mt2, weight );
    }


  } // for entries


  //bTagSF.~MT2BTagSFHelper();
 
  anaTree->finalize();

  delete tree;

  file->Close();
  delete file;

}

// TO DO: streamline computeSigYield similarly as computeYield
//
/*
template <class T>
MT2Analysis<T>* computeSigYield( const MT2Sample& sample, const MT2Config& cfg ) {

  bool dogenmet = false;

  TString sigSampleName(sample.name);
  TFile* sigXSFile;
  if(sigSampleName.Contains("T1qqqq") || sigSampleName.Contains("T1bbbb") || sigSampleName.Contains("T1tttt"))
    sigXSFile = TFile::Open("/shome/casal/SUSxsecs/SUSYCrossSections13TeVgluglu.root");
  else if(sigSampleName.Contains("T2bb") || sigSampleName.Contains("T2tt"))
    sigXSFile = TFile::Open("/shome/casal/SUSxsecs/SUSYCrossSections13TeVstopstop.root");
  else
    sigXSFile = TFile::Open("/shome/casal/SUSxsecs/SUSYCrossSections13TeVsquarkantisquark.root");

  TH1F* sigXS = (TH1F*) sigXSFile->Get("xs");

  if(sigSampleName.Contains("T2qq"))
    sigXS->Scale(8./10);

  std::string regionsSet = cfg.regionsSet();

  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample (computeSigYield): " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;

  TTree* tree = (TTree*)file->Get("mt2");

  MT2Tree myTree;
  myTree.Init(tree);


  std::cout << "-> Setting up MT2Analysis with name: " << sample.sname << std::endl;
  MT2Analysis<T>* analysis = new MT2Analysis<T>( sample.sname, regionsSet, sample.id );

  int nentries = tree->GetEntries();

  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);

    bool passGenMET = false;
    if(dogenmet)
      passGenMET = true;

    bool passRecoMET=true;

    if( !myTree.passBaselineKinematic() ) passRecoMET=false;

    if(dogenmet)
      if( !myTree.passBaselineKinematic("genmet") ) passGenMET=false;

    if (!passGenMET && !passRecoMET) continue;

    if( myTree.nLepLowMT==1 );
    else continue;

    if ( myTree.nJet30==1 && !myTree.passMonoJetId(0) ) continue;
    if ( myTree.nJet20BadFastsim > 0 ) continue;

    float ht   = myTree.ht;
    float met  = myTree.met_pt;
    float met_gen  = myTree.met_genPt;
    float minMTBmet = -999;//myTree.minMTBMet;
    int njets  = myTree.nJet30;
    int nbjets = myTree.nBJet20;
    float mt2  = (njets>1) ? myTree.mt2 : ht;
   

    float mt2_genmet;
    if(dogenmet)
      mt2_genmet = (njets>1) ? myTree.mt2_genmet : ht;

    int GenSusyMScan1=0;
    int GenSusyMScan2=0;
    if(  myTree.evt_id > 999){

      if(sigSampleName.Contains("T2qq")){

        GenSusyMScan1 = myTree.GenSusyMSquark;
        GenSusyMScan2 = myTree.GenSusyMNeutralino;

      }
      else if(sigSampleName.Contains("T2bb")){

        GenSusyMScan1 = myTree.GenSusyMSbottom;
        GenSusyMScan2 = myTree.GenSusyMNeutralino;

      }
      else if(sigSampleName.Contains("T2tt")){

        GenSusyMScan1 = myTree.GenSusyMStop;
        GenSusyMScan2 = myTree.GenSusyMNeutralino;

      }
      else{

	GenSusyMScan1 = myTree.GenSusyMGluino;
	GenSusyMScan2 = myTree.GenSusyMNeutralino;

      }

    }

    //Double_t weight = (myTree.isData) ? 1. : myTree.evt_scale1fb*cfg.lumi()*myTree.puWeight;
    Double_t weight = (myTree.isData) ? 1. : myTree.evt_scale1fb;//  cfg.lumi();
    //Double_t weight = (myTree.isData) ? 1. : myTree.evt_scale1fb*cfg.lumi();
    Double_t weight_syst = 1.;

    /////    weight = 1000.* myTree.evt_xsec/nentries; //Exceptionally for signal from muricans

    if( !myTree.isData ){
      weight *= myTree.weight_btagsf;
      weight *= myTree.weight_lepsf2017;

      if (myTree.evt_id == 301 || myTree.evt_id == 302)
	weight *= myTree.weight_isr/0.909; // central/average
      else if (myTree.evt_id == 303)
	weight *= myTree.weight_isr/0.895;
    }

    float sig_xs=0.;
    if( myTree.evt_id >= 1000  && myTree.evt_id < 2000){

      int thisBinX = sigXS->FindBin( GenSusyMScan1 );

      sig_xs = sigXS->GetBinContent(thisBinX);

      weight *= sig_xs;

    }

    T* thisEstimate = analysis->get( ht, njets, nbjets, minMTBmet, 201. );
    if( thisEstimate==0 ) continue;

    if( regionsSet=="zurich" || regionsSet=="zurichPlus" || regionsSet=="zurich2016" ){ // To avoid signal contamination in 7j 2b and 7j 3b

      if( ht>450 && njets>=7 && nbjets>2 ) continue;

      else if( ht<450 || njets<7 || nbjets<1) {

	if(passRecoMET){

	  thisEstimate  = analysis->get( ht, njets, nbjets, minMTBmet, mt2 );
	  if( thisEstimate==0 ) continue;

	  thisEstimate->yield->Fill( mt2, weight );
	  thisEstimate->yield3d->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight );

	}

	if(dogenmet && passGenMET){

	  thisEstimate  = analysis->get( ht, njets, nbjets, minMTBmet, mt2_genmet );
	  if( thisEstimate==0 ) continue;

	  thisEstimate->yield3d_genmet->Fill( mt2_genmet, GenSusyMScan1, GenSusyMScan2, weight );

	}

      }
      else {

        if(passRecoMET){

	  thisEstimate  = analysis->get( ht, njets, 1, minMTBmet, mt2 );
	  if( thisEstimate==0 ) continue;

          thisEstimate->yield->Fill( mt2, weight );
          thisEstimate->yield3d->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight );

        }

        if(dogenmet && passGenMET){

	  thisEstimate  = analysis->get( ht, njets, 1, minMTBmet, mt2_genmet );
	  if( thisEstimate==0 ) continue;

          thisEstimate->yield3d_genmet->Fill( mt2_genmet, GenSusyMScan1, GenSusyMScan2, weight );

        }

        if(passRecoMET){

	  thisEstimate  = analysis->get( ht, njets, 2, minMTBmet, mt2 );
	  if( thisEstimate==0 ) continue;

          thisEstimate->yield->Fill( mt2, weight );
          thisEstimate->yield3d->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight );

        }

        if(dogenmet && passGenMET){

	  thisEstimate  = analysis->get( ht, njets, 2, minMTBmet, mt2_genmet );
	  if( thisEstimate==0 ) continue;

          thisEstimate->yield3d_genmet->Fill( mt2_genmet, GenSusyMScan1, GenSusyMScan2, weight );

        }

        if(passRecoMET){

	  thisEstimate  = analysis->get( ht, njets, 3, minMTBmet, mt2 );
	  if( thisEstimate==0 ) continue;

          thisEstimate->yield->Fill( mt2, weight );
          thisEstimate->yield3d->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight );

        }

        if(dogenmet && passGenMET){

	  thisEstimate  = analysis->get( ht, njets, 3, minMTBmet, mt2_genmet );
	  if( thisEstimate==0 ) continue;

          thisEstimate->yield3d_genmet->Fill( mt2_genmet, GenSusyMScan1, GenSusyMScan2, weight );

        }


      }

    }
    else {


      if(passRecoMET){

	thisEstimate  = analysis->get( ht, njets, nbjets, minMTBmet, mt2 );
	if( thisEstimate==0 ) continue;

	thisEstimate->yield->Fill( mt2, weight );
	thisEstimate->yield3d->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight );

      }

      if(dogenmet && passGenMET){

	thisEstimate  = analysis->get( ht, njets, nbjets, minMTBmet, mt2_genmet );
	if( thisEstimate==0 ) continue;

	thisEstimate->yield3d_genmet->Fill( mt2_genmet, GenSusyMScan1, GenSusyMScan2, weight );

      }

    }

  } // for entries

  analysis->finalize();

  delete tree;

  file->Close();
  delete file;

  return analysis;

}
*/


void roundLikeData( MT2Analysis<MT2EstimateTree>* data ) {

   std::set<MT2Region> regions = data->getRegions();

   for( std::set<MT2Region>::iterator iR = regions.begin(); iR!=regions.end(); ++iR ) {

     TH1D* thisYield = data->get(*iR)->yield;

     for( int iBin=1; iBin<thisYield->GetNbinsX()+1; ++iBin ) {

       float yield = thisYield->GetBinContent(iBin);
       thisYield->SetBinContent(iBin, roundD(yield));
       thisYield->SetBinError(iBin, 0. );

     } // for bins

   } // for regions

}


float DeltaR(float eta1, float eta2, float phi1, float phi2){
  float dEta = eta1 - eta2;
  float dPhi = DeltaPhi(phi1, phi2);
  return TMath::Sqrt(dEta*dEta + dPhi*dPhi);
}

float DeltaPhi(float phi1, float phi2){
  float dPhi = phi1 - phi2;
  while (dPhi  >  TMath::Pi()) dPhi -= 2*TMath::Pi();
  while (dPhi <= -TMath::Pi()) dPhi += 2*TMath::Pi();
  return fabs(dPhi);
}
