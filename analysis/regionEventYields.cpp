// comment
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
//#include <cmath>


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
#include "interface/MT2BTagSFHelper.h"
//#include "interface/MT2GoodrunClass.h"

#include "TRandom3.h"

#include "interface/Utils.h" //to get the getNgen function

#define mt2_cxx
#include "interface/mt2.h"


using namespace std;


void randomizePoisson( MT2Analysis<MT2EstimateTree>* data );

void computeYield( const MT2Sample& sample, const MT2Config& cfg, MT2Analysis<MT2EstimateTree>* anaTree, MT2BTagSFHelper* bTagSF, std::string otherRegion="" );
template <class T>
MT2Analysis<T>* computeSigYield( const MT2Sample& sample, const MT2Config& cfg );
template <class T>
MT2Analysis<T>* mergeYields( std::vector<MT2Analysis<T> *> EventYield, const std::string& regionsSet, const std::string& name, int id_min, int id_max, const std::string& legendName );


int matchPartonToJet( int index, MT2Tree* myTree );



TH2D* h_nsig;
TH2D* h_avg_weight_btagsf ;
TH2D* h_avg_weight_btagsf_heavy_UP ;
TH2D* h_avg_weight_btagsf_light_UP ;
TH2D* h_avg_weight_btagsf_heavy_DN ;
TH2D* h_avg_weight_btagsf_light_DN ;
TH2D* h_avg_weight_isr;
TH2D* h_avg_weight_isr_UP;
TH2D* h_avg_weight_isr_DN;

int main( int argc, char* argv[] ) {


  std::cout << std::endl << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|             Running regionEventYields              |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "|                                                    |" << std::endl;
  std::cout << "------------------------------------------------------" << std::endl;
  std::cout << std::endl << std::endl;


  if( argc<2 ) {
    std::cout << "USAGE: ./regionEventYields [configFileName] [data/MC]" << std::endl;
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
      std::cout << "-> You passed a second argument that isn't 'data', nor 'mc', nor 'signal', so I don't know what to do about it." << std::endl;
    }
  }

  std::string signalName = "";
  if( argc > 3 ) {
    signalName = argv[3];
    std::cout << "-> Running on signal: " << signalName << std::endl;
  }

  std::string outputdir = cfg.getEventYieldDir();
  system(Form("mkdir -p %s", outputdir.c_str()));



  if( argc > 3){
    std::string model = signalName;
    std::string filename = Form("/scratch/mratti/WeightsForMoriond17Signals/nsig_weights_%s.root", model.c_str() ); // these come from SnT repo
    TFile* f_avWeights = new TFile(filename.c_str() );

    h_nsig                       = (TH2D*) f_avWeights->Get("h_nsig");
    h_avg_weight_btagsf          = (TH2D*) f_avWeights->Get("h_avg_weight_btagsf");
    h_avg_weight_btagsf_heavy_UP = (TH2D*) f_avWeights->Get("h_avg_weight_btagsf_heavy_UP");
    h_avg_weight_btagsf_light_UP = (TH2D*) f_avWeights->Get("h_avg_weight_btagsf_light_UP");
    h_avg_weight_btagsf_heavy_DN = (TH2D*) f_avWeights->Get("h_avg_weight_btagsf_heavy_DN");
    h_avg_weight_btagsf_light_DN = (TH2D*) f_avWeights->Get("h_avg_weight_btagsf_light_DN");
    h_avg_weight_isr             = (TH2D*) f_avWeights->Get("h_avg_weight_isr");
    h_avg_weight_isr_UP             = (TH2D*) f_avWeights->Get("h_avg_weight_isr_UP");
    h_avg_weight_isr_DN             = (TH2D*) f_avWeights->Get("h_avg_weight_isr_DN");

    h_nsig                      ->SetDirectory(0);
    h_avg_weight_btagsf         ->SetDirectory(0);
    h_avg_weight_btagsf_heavy_UP         ->SetDirectory(0);
    h_avg_weight_btagsf_light_UP         ->SetDirectory(0);
    h_avg_weight_btagsf_heavy_DN         ->SetDirectory(0);
    h_avg_weight_btagsf_light_DN         ->SetDirectory(0);
    h_avg_weight_isr            ->SetDirectory(0);
    h_avg_weight_isr_UP            ->SetDirectory(0);
    h_avg_weight_isr_DN            ->SetDirectory(0);

    f_avWeights->Close();
    delete f_avWeights;
  }


  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this


  std::vector<MT2Analysis<MT2EstimateTree>* > EventYields_toWrite; // used to handle with a for loop the writing to file of all yields

  // ********************
  // Do analysis on MC bkg
  // ********************
  std::vector<std::string> sampleNames = {"Top", "WJets", "QCD", "ZJets"};  // group of bkg mc samples
  std::map<std::string, MT2Analysis<MT2EstimateTree>*> mcSRMap;

  if( cfg.useMC() && !onlyData && !onlySignal ) { // do bkg

    // Load samples
    std::string samplesFileName = "../samples/samples_" + cfg.mcSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;

    // std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 101, 999); // not interested in signal here (see later)
    //if( fSamples.size()==0 ) {
    //  std::cout << "There must be an error: samples is empty!" << std::endl;
    //  exit(120);
    //}

    // create Groups of samples, i.e. vectors - e.g. one for Wjets, one for top, etc
    std::map<TString, std::vector<MT2Sample>> fSamplesMap;
    std::cout << "-> Creating sample list for Top " << std::endl;
    fSamplesMap["Top"] = MT2Sample::loadSamples(samplesFileName, 300, 499, cfg.useETHmc());
    std::cout << "-> Creating sample list for WJets " << std::endl;
    fSamplesMap["WJets"] = MT2Sample::loadSamples(samplesFileName, 500, 509, cfg.useETHmc());
    std::cout << "-> Creating sample list for QCD " << std::endl;
    fSamplesMap["QCD"] = MT2Sample::loadSamples(samplesFileName, 100, 199, cfg.useETHmc());
    std::cout << "-> Creating sample list for ZJets " << std::endl;
    fSamplesMap["ZJets"] = MT2Sample::loadSamples(samplesFileName, 600, 699, cfg.useETHmc());



    // Calculate the yields
    for(auto sampleName : sampleNames){
      std::cout << "-> Considering group " << sampleName << std::endl;
      // create the estimate tree for the group
      mcSRMap[sampleName] = new MT2Analysis<MT2EstimateTree> ( sampleName, cfg.regionsSet() );
      // then fill it from all samples in the group
      for (auto fSample : fSamplesMap[sampleName]){
	MT2BTagSFHelper* bTagSF = new MT2BTagSFHelper();
        computeYield( fSample, cfg, mcSRMap[sampleName], bTagSF );
	bTagSF = nullptr;
      }
      std::cout << "-> Done looping on samples for this group" << std::endl;
    }

    // add them to the output handler
    for (auto sampleName : sampleNames){
      EventYields_toWrite.push_back(mcSRMap[sampleName]);
    }

    cout << "Now ZJets samples" << endl;

    //Now create ZJets estimate in the inclusive region (needed to compute Zinv Estimates)
    vector<MT2Sample> mySample = MT2Sample::loadSamples(samplesFileName, 600, 699, cfg.useETHmc());
    MT2Analysis<MT2EstimateTree>* myEstimate = new MT2Analysis<MT2EstimateTree>("ZJets", "13TeV_2016_inclusive");

    cout << "size: " << mySample.size() << endl;

    for(int i(0); i<mySample.size(); ++i){
      cout << "Computing for sample " << i+1 << endl;
      MT2BTagSFHelper* bTagSF_ZJetsIncl = new MT2BTagSFHelper();
      computeYield(mySample[i], cfg, myEstimate, bTagSF_ZJetsIncl);
      bTagSF_ZJetsIncl = nullptr;
    }

    myEstimate->writeToFile(outputdir + "/ZJetsInclusive.root");

  } // if bkg MC samples

  // ********************
  // Do analysis on signals, if any
  // ********************

  MT2Analysis<MT2EstimateAllSigSyst>* merged_signal;

  std::vector< MT2Analysis< MT2EstimateAllSigSyst>* > signals;
  if( cfg.sigSamples()!="" && cfg.additionalStuff()!="noSignals" && !onlyData ) { // Take signals from a different sample file, compulsory, otherwise no signals analysis

    std::string samplesFileName = "../samples/samples_" + cfg.sigSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading signal samples from file: " << samplesFileName << std::endl;
    std::cout << "     signal name " << signalName << std::endl;

    //std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, signalName); // only signal (id>=1000)
    //std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 1000, -1, cfg.useETHmc()); // only signal (id>=1000)
    // only load samples related to the chosen signal scan
    std::vector<MT2Sample> fSamplesSig;
    if       (signalName=="T1qqqq")   fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1000, 1100, cfg.useETHmc());
    else if  (signalName=="T1bbbb")   fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1100, 1200, cfg.useETHmc());
    else if  (signalName=="T1tttt")   fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1200, 1300, cfg.useETHmc());
    else if  (signalName=="T2qq")     fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1300, 1400, cfg.useETHmc());
    else if  (signalName=="T2bb")     fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1400, 1500, cfg.useETHmc());
    else if  (signalName=="T2tt")     fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1500, 1600, cfg.useETHmc());

    if( fSamplesSig.size()==0 ) {
      std::cout << "No signal samples found, skipping." << std::endl;
    } else {
      for( unsigned i=0; i<fSamplesSig.size(); ++i ){
        //continue;
        signals.push_back( computeSigYield<MT2EstimateAllSigSyst>( fSamplesSig[i], cfg ) );
      }
    } // if samples != 0

    std::cout << "Merging signals" << std::endl;

    merged_signal   = mergeYields<MT2EstimateAllSigSyst>( signals, cfg.regionsSet(), signalName, 1000, 2000, signalName );

  } // if signal samples


  // ********************
  // Do analysis on data
  // ********************
  if( !(cfg.dummyAnalysis()) && cfg.dataSamples()!="" && !onlyMC  && !onlySignal ) {

    // Read the samples and create the MT2Sample for data
    std::string samplesFileNameData = "../samples/samples_" + cfg.dataSamples() + ".dat";

    std::cout << std::endl << std::endl;
    std::cout << "-> Loading data from file: " << samplesFileNameData << std::endl;

    std::vector<MT2Sample> fSamplesData = MT2Sample::loadSamples(samplesFileNameData, "", -1, 100, cfg.useETHdata()); //instead of "", do e.g. "noDupl" to load only samples matching "noDupl" epxression
    if( fSamplesData.size()==0 ) {
      std::cout << "There must be an error: fSamplesData is empty!" << std::endl;
      exit(1209);
    }

    // Compute the yields
    MT2Analysis<MT2EstimateTree> *dataSR = new MT2Analysis<MT2EstimateTree> ( "data", cfg.regionsSet() );;

    for (auto fSample : fSamplesData){
      MT2BTagSFHelper* bTagSF_data = new MT2BTagSFHelper();
      computeYield( fSample, cfg, dataSR, bTagSF_data );
      bTagSF_data = nullptr;
    }

    // Add result to output handler
    EventYields_toWrite.push_back( dataSR );

  } // end data

  // ********************
  // Write all analyses to output
  // ********************
  if( EventYields_toWrite.size()==0 && signals.size()==0 ) {
    std::cout << "-> Didn't end up with a single yield... something's wrong." << std::endl;
    exit(87);
  }

  if( EventYields_toWrite.size()>0 ){
    EventYields_toWrite[0]->writeToFile(outputdir + "/analyses.root"); // not sure why we split the writing !!!
    for( unsigned i=1; i<EventYields_toWrite.size(); ++i ) {
      EventYields_toWrite[i]->writeToFile(outputdir + "/analyses.root");
    }
  } else if( signals.size()>0 ){
    merged_signal->writeToFile(outputdir + "/analyses.root");
  }

  cfg.saveAs(outputdir + "/config.txt");


  return 0;

}


void computeYield( const MT2Sample& sample, const MT2Config& cfg, MT2Analysis<MT2EstimateTree>* anaTree, MT2BTagSFHelper* bTagSF, std::string otherRegion){

  std::string regionsSet = cfg.regionsSet();
  if(otherRegion != ""){
    regionsSet = otherRegion;
  }

  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;


  // determine if it's data or mc here
  bool  isData = (sample.id >= -1 && sample.id < 100 );
  std::cout << " sample.id=" << sample.id << " isData=" << isData << std::endl;

  // determine if it is an ETH kind of ntuple or not
  bool isETH = (isData and cfg.useETHdata()) || (!isData and cfg.useETHmc());

  // Tree initialization
  TString treeName = isETH ? "Events" : "mt2";
  TFile* file = TFile::Open(sample.file.c_str());
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;
  TTree* tree = (TTree*)file->Get(treeName);

  MT2Tree myTree(tree, isETH);

  // number of gen events
  double nGen=-9999; double nGenWeighted=-9999;
  if(!isData and isETH){
    nGen = getNgen(sample.file, "genEventCount");
    nGenWeighted = getNgen(sample.file, "genEventSumw");
  }

  int nentries = tree->GetEntries();

  //for( int iEntry=0; iEntry<20000; ++iEntry ) {
  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ){
      std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;
    }

    myTree.GetEntry(iEntry);

    //if( myTree.isData && !myTree.isGolden ) continue;
    //if(isData && !myTree.isGolden ) continue;

    // apply the filters
    // filters should be the same bw ETH and SnT
    if(isData) {
      if(!myTree.passFilters(cfg.year())) continue;
    } //else {
      //if(!myTree.passFiltersMC(cfg.year())) continue;
    //}

    // apply the triggers
    if(isData and isETH) {
      if (!myTree.passTriggerSelection("SR", cfg.year())) continue;
    }

    if (isETH) {
      if(myTree.PV_npvs <= 0) continue;
    } else {
      if(myTree.nVert <= 0) continue;
    }

    // some additional cleanings
    //if( myTree.nJet200MuFrac50DphiMet > 0 ) continue; // new RA2 filter --> not in nanoAOD
    //if( myTree.met_miniaodPt/myTree.met_caloPt > 5.0 ) continue; -->not in nanoAOD

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

    // apply HEM veto
    if (!myTree.passHEMFailVeto(cfg.year(), isETH, isData)) continue;

    // kinematic selections, including lepton veto
    if( !myTree.passSelection("", cfg.year(), isETH )) continue;


    // monojet id
    if ( myTree.nJet30==1 && !myTree.passMonoJetId(0) ) continue;


    // Selection is over, now apply the weights !
    //Double_t weight = (isData) ? 1. : myTree.evt_scale1fb;//*cfg.lumi();
    Double_t weight_syst = 1.;
    Double_t weight(1.);

    //weight on the cross section
    if(isData){
      weight = 1.;
    }
    else{
      if (isETH) weight =  myTree.evt_xsec * myTree.evt_kfactor * myTree.evt_filter * 1000/nGen;
      else {
        weight = myTree.evt_scale1fb / (myTree.evt_xsec * myTree.evt_kfactor * myTree.evt_filter) * myTree.weight_lepsf * myTree.weight_btagsf;
        weight *= myTree.getXSecCorrWeight(sample.id, cfg.year());
        if (cfg.year()==2016 || cfg.year()==2017) weight *= myTree.weight_L1prefire;
        if ((sample.id==301 || sample.id==302 || sample.id==303) && cfg.year()==2016) weight *= myTree.weight_isr / myTree.getAverageISRWeight(sample.id,cfg.year(),0); // nominal
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

    // Now that you have done the pre-selection
    // you can actually make the estimates
    // define here the variables that will enter the region categorization
    float ht   = myTree.ht;
    float minMTBmet = -999; //
    int njets  = myTree.nJet30;
    int nbjets = myTree.nBJet20;
    float mt2  = (njets>1) ? myTree.mt2 : ht;

    MT2EstimateTree *thisEstimate = anaTree->get( ht, njets, nbjets, minMTBmet, mt2 );
    if( thisEstimate==0 ) continue;
    if( sample.id < 1000 ){
      thisEstimate->assignTree( myTree, weight );
      thisEstimate->tree->Fill();
    }

    thisEstimate->yield->Fill( mt2, weight );

  } // for entries

  anaTree->finalize();

  delete tree;

  file->Close();
  delete file;

}

template <class T>
MT2Analysis<T>* computeSigYield( const MT2Sample& sample, const MT2Config& cfg ) {

  bool dogenmet = true;

  // Old cross-sections from file
  TString sigSampleName(sample.name);
  TFile* sigXSFile;
  if(sigSampleName.Contains("T1qqqq") || sigSampleName.Contains("T1bbbb") || sigSampleName.Contains("T1tttt"))
    sigXSFile = TFile::Open("/shome/mratti/SUSxsecs/SUSYCrossSections13TeVgluglu.root");
  else if(sigSampleName.Contains("T2bb") || sigSampleName.Contains("T2tt") || sigSampleName.Contains("T2cc")
       || sigSampleName.Contains("T2bW") || sigSampleName.Contains("T2bt"))
    sigXSFile = TFile::Open("/shome/mratti/SUSxsecs/SUSYCrossSections13TeVstopstop.root");
  else
    sigXSFile = TFile::Open("/shome/mratti/SUSxsecs/SUSYCrossSections13TeVsquarkantisquark.root");

  TH1F* sigXS = (TH1F*) sigXSFile->Get("xs");

  if(sigSampleName.Contains("T2qq"))
    sigXS->Scale(8./10);
  ////// FIXME: update to NLO and apply coherently

  std::string regionsSet = cfg.regionsSet();

  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample (computeSigYield): " << sample.name << std::endl;

  // determine if it's data or mc here
  bool  isData = (sample.id >= -1 && sample.id < 100 );
  std::cout << " sample.id=" << sample.id << " isData=" << isData << std::endl;

  if(isData) std::cout << "OOOOOOOOOOOOh , you sure that this is data???" << std::endl;

  // determine if it is an ETH kind of ntuple or not
  bool isETH = (isData and cfg.useETHdata()) || (!isData and cfg.useETHmc());

  // Tree initialization
  TString treeName = isETH ? "Events" : "mt2";
  TFile* file = TFile::Open(sample.file.c_str());
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;
  TTree* tree = (TTree*)file->Get(treeName);

  MT2Tree myTree(tree, isETH);

  std::cout << "-> Setting up MT2Analysis with name: " << sample.sname << std::endl;
  MT2Analysis<T>* analysis = new MT2Analysis<T>( sample.sname, regionsSet, sample.id );

  //int nentries= 10000; //= tree->GetEntries();
  int nentries = tree->GetEntries();

  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;
    myTree.GetEntry(iEntry);

    if (isETH) {
      if(myTree.PV_npvs <= 0) continue;
    } else {
      if(myTree.nVert <= 0) continue;
    }

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

    // apply HEM veto
    if (!myTree.passHEMFailVeto(cfg.year(), isETH, isData)) continue;

    // Gen MET cut
    bool passGenMET=false;
    if(dogenmet) passGenMET =true;

    bool passRecoMET=true;

    if( regionsSet!="13TeV_noCut" ){
      if( !myTree.passSelection(cfg.additionalStuff()) ) passRecoMET=false;
      if(dogenmet)
        if( !myTree.passSelection("genmet") ) passGenMET=false;
      if (!passGenMET && !passRecoMET) continue;
    }

    if( myTree.nJet30==1 && !myTree.passMonoJetId(0) ) continue;

    //if( myTree.nJet20BadFastsim > 0 ) continue;
    //if( myTree.nJet200MuFrac50DphiMet > 0 ) continue; // new RA2 filter


    float ht   = myTree.ht;
    ////    float met  = myTree.met_pt;
    //float met_gen  = myTree.met_genPt;
    float minMTBmet = -999;
    int njets  = myTree.nJet30;
    int nbjets = myTree.nBJet20;
    float mt2  = (njets>1) ? myTree.mt2 : ht;
    float mt2_genmet;

     if(dogenmet) mt2_genmet = (njets>1) ? myTree.mt2_genmet : ht;


    // Selection is over, now weight things up (or down) :)
    // First get the leaves into a local variable
    float weight_isr        = myTree.weight_isr;
    float weight_isr_UP     = myTree.weight_isr_UP;
    float weight_isr_DN     = myTree.weight_isr_DN;

    float weight_lepsf        = myTree.weight_lepsf;
    float weight_lepsf_UP     = myTree.weight_lepsf_UP;
    float weight_lepsf_DN     = myTree.weight_lepsf_DN;

    float weight_btagsf        = myTree.weight_btagsf;
    float weight_btagsf_heavy_UP = myTree.weight_btagsf_heavy_UP;
    float weight_btagsf_heavy_DN = myTree.weight_btagsf_heavy_DN;
    float weight_btagsf_light_UP = myTree.weight_btagsf_light_UP;
    float weight_btagsf_light_DN = myTree.weight_btagsf_light_DN;

    int GenSusyMScan1=0;
    int GenSusyMScan2=0;


    if(  myTree.evt_id > 999){
      //SNT WAY
      GenSusyMScan1 = myTree.GenSusyMScan1;
      GenSusyMScan2 = myTree.GenSusyMScan2;
      //    std::cout << "Masses are " << GenSusyMScan1 << " and " << GenSusyMScan2 << std::endl;
    }

    // how it should be in the end

    // get the avg weights from the histograms
    int binx = h_nsig->GetXaxis()->FindBin( GenSusyMScan1 );
    int biny = h_nsig->GetYaxis()->FindBin( GenSusyMScan2 );
    float nevts                        = h_nsig->GetBinContent( binx, biny );
    float weight_avg_isr               = h_avg_weight_isr->GetBinContent( binx, biny );
    float weight_avg_isr_UP            = h_avg_weight_isr_UP->GetBinContent( binx, biny );
    float weight_avg_isr_DN            = h_avg_weight_isr_DN->GetBinContent( binx, biny );
    float weight_avg_btagsf            = h_avg_weight_btagsf->GetBinContent( binx, biny );
    float weight_avg_btagsf_light_UP   = h_avg_weight_btagsf_light_UP->GetBinContent( binx, biny );
    float weight_avg_btagsf_light_DN   = h_avg_weight_btagsf_light_DN->GetBinContent( binx, biny );
    float weight_avg_btagsf_heavy_UP   = h_avg_weight_btagsf_heavy_UP->GetBinContent( binx, biny );
    float weight_avg_btagsf_heavy_DN   = h_avg_weight_btagsf_heavy_DN->GetBinContent( binx, biny );

    // now  calculate  nominal weight
    Double_t weight = 1.;
    weight = weight_btagsf * weight_lepsf * weight_isr / (nevts*weight_avg_isr*weight_avg_btagsf) ; // from histogram / number of events

    // multiply by cross-sections
    float sig_xs=0.;
    if( myTree.evt_id >= 1000  && myTree.evt_id < 2000){

      if (isETH){
        int thisBinX = sigXS->FindBin( GenSusyMScan1 );
        sig_xs = sigXS->GetBinContent(thisBinX);
        // std::cout << " sig_xs " << sig_xs << std::endl;
        weight *= sig_xs; // * 1000 ?
      } else{
        weight *= myTree.evt_xsec*myTree.evt_filter*1000;  // NOTE: normalized to 1/fb
        if (cfg.year()==2016 || cfg.year()==2017) weight *= myTree.weight_L1prefire;
      }

    } else  std::cout << "THIS SHOULD NOT HAPPEN, PLEASE CHECK" << std::endl;

    T* thisEstimate = analysis->get( ht, njets, nbjets, minMTBmet, mt2 );
    if( thisEstimate==0 ) continue;

    if(passRecoMET){
      thisEstimate->yield->Fill( mt2, weight );
      thisEstimate->yield3d->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight );

      thisEstimate->yield3d_isr_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_isr*weight_avg_isr*weight_isr_UP/weight_avg_isr_UP );
      thisEstimate->yield3d_isr_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_isr*weight_avg_isr*weight_isr_DN/weight_avg_isr_DN );

      thisEstimate->yield3d_lepsf_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_lepsf*weight_lepsf_UP);
      thisEstimate->yield3d_lepsf_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_lepsf*weight_lepsf_DN);

      thisEstimate->yield3d_btag_heavy_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_btagsf*weight_avg_btagsf*weight_btagsf_heavy_UP/weight_avg_btagsf_heavy_UP );
      thisEstimate->yield3d_btag_heavy_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_btagsf*weight_avg_btagsf*weight_btagsf_heavy_DN/weight_avg_btagsf_heavy_DN );

      thisEstimate->yield3d_btag_light_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_btagsf*weight_avg_btagsf*weight_btagsf_light_UP/weight_avg_btagsf_light_UP );
      thisEstimate->yield3d_btag_light_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_btagsf*weight_avg_btagsf*weight_btagsf_light_DN/weight_avg_btagsf_light_DN );
    }

    if(dogenmet && passGenMET){
      thisEstimate->yield3d_genmet->Fill( mt2_genmet, GenSusyMScan1, GenSusyMScan2, weight );
    }

  } // for entries

  //ofs.close();

  analysis->finalize();

  delete tree;

  file->Close();
  delete file;

  return analysis;

}




template <class T>
MT2Analysis<T>* mergeYields( std::vector<MT2Analysis<T> *> EventYield, const std::string& regionsSet, const std::string& name, int id_min, int id_max, const std::string& legendName ) {
  if( id_max<0 ) id_max=id_min;

  MT2Analysis<T>* return_EventYield = new MT2Analysis<T>(name, regionsSet, id_min, legendName);

  for( unsigned i=0; i<EventYield.size(); ++i ) {

    if( EventYield[i]->getId() >= id_min && EventYield[i]->getId() <= id_max ) {
       *(return_EventYield) += *(EventYield[i]);
    }
  } // for EventYield

  return return_EventYield;

}





void randomizePoisson( MT2Analysis<MT2EstimateTree>* data ) {

  TRandom3 rand(13);


//  std::set<MT2HTRegion> HTRegions = data->getHTRegions();
//  std::set<MT2SignalRegion> signalRegions = data->getSignalRegions();

  std::set<MT2Region> MT2Regions = data->getRegions();

//  for( std::set<MT2HTRegion>::iterator iHT = HTRegions.begin(); iHT!=HTRegions.end(); ++iHT ) {
//    for( std::set<MT2SignalRegion>::iterator iSR = signalRegions.begin(); iSR!=signalRegions.end(); ++iSR ) {

      for( std::set<MT2Region>::iterator iMT2 = MT2Regions.begin(); iMT2!=MT2Regions.end(); ++iMT2 ) {

	MT2Region thisRegion( (*iMT2) );

	TH1D* h1_data = data->get(thisRegion)->yield;

	for( int ibin=1; ibin<h1_data->GetXaxis()->GetNbins()+1; ++ibin ) {

	  int poisson_data = rand.Poisson(h1_data->GetBinContent(ibin, 0, 0));
	  h1_data->SetBinContent(ibin, 0, 0, poisson_data);
	  h1_data->SetBinError(ibin, 0, 0,  0.);

	}  // for bins

      }// for MT2 regions

//    }// for signal regions
//  }// for HT regions

}

/*
int matchPartonToJet( int index, MT2Tree* myTree ) {

  float deltaRMin = 0.3; // at least 0.3
  int foundId = 0;

  TLorentzVector jet;
  jet.SetPtEtaPhiM( myTree->jet_pt[index], myTree->jet_eta[index], myTree->jet_phi[index], myTree->Jet_mass[index] );


  for( int i=0; i<myTree->ngenPart; ++i ) {

    if( myTree->genPart_status[i]!=23 ) continue;
    if( !(myTree->genPart_pdgId[i]==21 || abs(myTree->genPart_pdgId[i]<6)) ) continue;

    TLorentzVector thisPart;
    thisPart.SetPtEtaPhiM( myTree->genPart_pt[i], myTree->genPart_eta[i], myTree->genPart_phi[i], myTree->genPart_mass[i] );

    float thisDeltaR = jet.DeltaR(thisPart);
    if( thisDeltaR<deltaRMin ) {
      deltaRMin = thisDeltaR;
      foundId = myTree->genPart_pdgId[i];
      }
      }

  return foundId;

}

*/
