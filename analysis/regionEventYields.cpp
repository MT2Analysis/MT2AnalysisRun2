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

#include "TRandom3.h"

#include "interface/Utils.h" //to get the getNgen function

#define mt2_cxx
#include "interface/mt2.h"


using namespace std;


void randomizePoisson( MT2Analysis<MT2EstimateTree>* data );

void computeYield( const MT2Sample& sample, const MT2Config& cfg, MT2Analysis<MT2EstimateTree>* anaTree, std::string otherRegion="" );
//template <class T>
//MT2Analysis<T>* computeSigYield( const MT2Sample& sample, const MT2Config& cfg );
//template <class T>
int matchPartonToJet( int index, MT2Tree* myTree );



TH2D* h_nsig;
TH2D* h_nsigNVtxGeq20;
TH2D* h_nsigNVtxL20;
TH2D* h_avg_weight_btagsf ;
TH2D* h_avg_weight_btagsfNVtxGeq20;
TH2D* h_avg_weight_btagsfNVtxL20;
TH2D* h_avg_weight_isr;
TH2D* h_avg_weight_isrNVtxGeq20;
TH2D* h_avg_weight_isrNVtxL20;



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
      std::cout << "-> You passed a second argument that isn't 'data', nor 'MC', nor 'signal', so I don't know what to do about it." << std::endl;
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
    std::string filename = Form("/scratch/mmasciov/signalTreesMoriond2017/nsig_weights_%s.root", model.c_str() );
    TFile* f_avWeights = new TFile(filename.c_str() );

    h_nsig                       = (TH2D*) f_avWeights->Get("h_nsig");
    h_nsigNVtxGeq20              = (TH2D*) f_avWeights->Get("h_nsigNVtxGeq20");
    h_nsigNVtxL20                = (TH2D*) f_avWeights->Get("h_nsigNVtxL20");
    h_avg_weight_btagsf          = (TH2D*) f_avWeights->Get("h_avg_weight_btagsf");
    h_avg_weight_btagsfNVtxGeq20 = (TH2D*) f_avWeights->Get("h_avg_weight_btagsfNVtxGeq20");
    h_avg_weight_btagsfNVtxL20   = (TH2D*) f_avWeights->Get("h_avg_weight_btagsfNVtxL20");
    h_avg_weight_isr             = (TH2D*) f_avWeights->Get("h_avg_weight_isr");
    h_avg_weight_isrNVtxGeq20    = (TH2D*) f_avWeights->Get("h_avg_weight_isrNVtxGeq20");
    h_avg_weight_isrNVtxL20      = (TH2D*) f_avWeights->Get("h_avg_weight_isrNVtxL20");

    h_nsig                      ->SetDirectory(0);
    h_nsigNVtxGeq20             ->SetDirectory(0);
    h_nsigNVtxL20               ->SetDirectory(0);
    h_avg_weight_btagsf         ->SetDirectory(0);
    h_avg_weight_btagsfNVtxGeq20->SetDirectory(0);
    h_avg_weight_btagsfNVtxL20  ->SetDirectory(0);
    h_avg_weight_isr            ->SetDirectory(0);
    h_avg_weight_isrNVtxGeq20   ->SetDirectory(0);
    h_avg_weight_isrNVtxL20     ->SetDirectory(0);

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

/*    std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 101, 999); // not interested in signal here (see later)
    if( fSamples.size()==0 ) {
      std::cout << "There must be an error: samples is empty!" << std::endl;
      exit(120);
    }
*/
    // create Groups of samples, i.e. vectors - e.g. one for Wjets, one for top, ecc
    std::map<TString, std::vector<MT2Sample>> fSamplesMap;
    std::cout << "-> Creating sample list for Top " << std::endl;
    fSamplesMap["Top"] = MT2Sample::loadSamples(samplesFileName, 300, 499);
    std::cout << "-> Creating sample list for WJets " << std::endl;
    fSamplesMap["WJets"] = MT2Sample::loadSamples(samplesFileName, 500, 509);
    std::cout << "-> Creating sample list for QCD " << std::endl;
    fSamplesMap["QCD"] = MT2Sample::loadSamples(samplesFileName, 100, 199);
    std::cout << "-> Creating sample list for ZJets " << std::endl;
    fSamplesMap["ZJets"] = MT2Sample::loadSamples(samplesFileName, 600, 699);

    // Calculate the yields 
    for(auto sampleName : sampleNames){
      std::cout << "-> Considering group " << sampleName << std::endl;
      // create the estimate tree for the group
      mcSRMap[sampleName] = new MT2Analysis<MT2EstimateTree> ( sampleName, cfg.regionsSet() );
      // then fill it from all samples in the group
      for (auto fSample : fSamplesMap[sampleName]){
        computeYield( fSample, cfg, mcSRMap[sampleName] );
      }
      std::cout << "-> Done looping on samples for this group" << std::endl;
    }

    // add them to the output handler
    for (auto sampleName : sampleNames){
      EventYields_toWrite.push_back(mcSRMap[sampleName]);
    }

  } // if bkg MC samples

  // ********************
  // Do analysis on signals, if any
  // ********************

  MT2Analysis<MT2EstimateAllSigSyst>* signalYield ;

  std::vector< MT2Analysis< MT2EstimateAllSigSyst>* > signals;
  if( cfg.sigSamples()!="" && cfg.additionalStuff()!="noSignals" && !onlyData ) { // Take signals from a different sample file, compulsory, otherwise no signals analysis

    std::string samplesFileName = "../samples/samples_" + cfg.sigSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading signal samples from file: " << samplesFileName << std::endl;
    std::cout << "     signal name " << signalName << std::endl;

    //std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, signalName); // only signal (id>=1000)
    std::vector<MT2Sample> fSamples = MT2Sample::loadSamples(samplesFileName, 1000); // only signal (id>=1000)

    if( fSamples.size()==0 ) {

      std::cout << "No signal samples found, skipping." << std::endl;

    } else {

      for( unsigned i=0; i<fSamples.size(); ++i )
        continue;
        //signals.push_back( computeSigYield<MT2EstimateAllSigSyst>( fSamples[i], cfg ) );
    } // if samples != 0

    std::cout << "Merging        signals" << std::endl;

    //signalYield   = mergeYields<MT2EstimateAllSigSyst>( signals, cfg.regionsSet(), signalName, 1000, 2000 ); //old t1bbbb 1200, 1249


  } // if signal samples


  // ********************
  // Do analysis on data
  // ********************
  if( !(cfg.dummyAnalysis()) && cfg.dataSamples()!="" && !onlyMC  && !onlySignal ) {

    // Read the samples and create the MT2Sample for data    
    std::string samplesFileNameData = "../samples/samples_" + cfg.dataSamples() + ".dat";

    std::cout << std::endl << std::endl;
    std::cout << "-> Loading data from file: " << samplesFileNameData << std::endl;

    std::vector<MT2Sample> fSamplesData = MT2Sample::loadSamples(samplesFileNameData, ""); //instead of "", do e.g. "noDupl" to load only samples matching "noDupl" epxression
    if( fSamplesData.size()==0 ) {
      std::cout << "There must be an error: fSamplesData is empty!" << std::endl;
      exit(1209);
    }

    // Compute the yields
    MT2Analysis<MT2EstimateTree> *dataSR = new MT2Analysis<MT2EstimateTree> ( "data", cfg.regionsSet() );; 
 
    for (auto fSample : fSamplesData){
      computeYield( fSample, cfg, dataSR );
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
    signals[0]->writeToFile(outputdir + "/analyses.root");
    for( unsigned i=1; i<signals.size(); ++i ) {
      signals[i]->writeToFile(outputdir + "/analyses.root");
    }
  }

  // signalYield->writeToFile(outputdir + "/analyses.root");

  cfg.saveAs(outputdir + "/config.txt");
  return 0;

}


void computeYield( const MT2Sample& sample, const MT2Config& cfg, MT2Analysis<MT2EstimateTree>* anaTree, std::string otherRegion){

  std::string regionsSet = cfg.regionsSet();
  if(otherRegion!="")
    regionsSet = otherRegion;

  std::cout << std::endl << std::endl;
  std::cout << "-> Starting computation for sample: " << sample.name << std::endl;

  TFile* file = TFile::Open(sample.file.c_str());
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;

  //For 2016 ntuples, the tree is called mt2
  //TTree* tree = (TTree*)file->Get("mt2");
  //For 2017 ntuples, the tree is called Events
  TTree* tree = (TTree*)file->Get("Events");

  MT2Tree myTree;
  myTree.Init(tree);

  //We temporally add this line, since it is not a branch of the MT2tree yet
  Bool_t isData = (sample.id >= 1 && sample.id < 100 );
  std::cout << "evt_id=" << myTree.evt_id << " sample.id=" << sample.id << "  isData="<< isData << std::endl;

  double nGen=-9999; double nGenWeighted=-9999;
  if(!isData){
    nGen = getNgen(sample.file, "genEventCount");
    nGenWeighted = getNgen(sample.file, "genEventSumw");
  }

  int nentries = tree->GetEntries();

  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;
    myTree.GetEntry(iEntry);
    //if( myTree.isData && !myTree.isGolden ) continue;
    //if(isData && !myTree.isGolden ) continue;
    if(isData) {
      if(cfg.year() == 2016) if( !myTree.passFilters2016() ) continue;
      if(cfg.year() == 2017) if( !myTree.passFilters2017() ) continue;
    } else {
      if(cfg.year() == 2016) if( !myTree.passFiltersMC2016() ) continue;
      if(cfg.year() == 2017) if( !myTree.passFiltersMC2017() ) continue;
    } 
    if (isData) {
      if(cfg.year() == 2017) if (!myTree.passTriggerSelection2017("SR")) continue;
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
    // kinematic selections, including lepton veto
    if( !myTree.passSelection() ) continue;
    // monojet id, not needed for 2017, as all jets have tight id
    //if ( myTree.nJet30==1 && !myTree.passMonoJetId(0) ) continue;

    // Selection is over, now apply the weights !
    //Double_t weight = (isData) ? 1. : myTree.evt_scale1fb;//*cfg.lumi();
    Double_t weight_syst = 1.;
    Double_t weight(1.);
    if(isData){
      weight = 1.;
    }
    else{
      weight =  myTree.evt_xsec * myTree.evt_kfactor * myTree.evt_filter * 1000/nGen;
    }
    // if(!isData){
    // weight *= myTree.weight_btagsf;
    // weight *= myTree.weight_lepsf;

    //// // ETH has a branch witht he average weight stored:
    //// // Also we have a different numbering scheme...
    //// if (myTree.evt_id == 302 || myTree.evt_id == 303 || myTree.evt_id == 304) //singleLep T/Tbar, Dilep
    //// 	weight *= myTree.weight_isr / myTree.weight_isr_norm;

    /////AMERICAN WAY
    //if (myTree.evt_id == 301 || myTree.evt_id == 302)
    //	weight *= myTree.weight_isr/0.909; // nominal
    //else if (myTree.evt_id == 303)
    //	weight *= myTree.weight_isr/0.895;

    //}

    // Now that you have done the pre-selection
    // you can actually make the estimates
    // define here the variables that will enter the region categorization
    float ht   = myTree.ht;
    float minMTBmet = -999; // 
    int njets  = myTree.nJet30;
    int nbjets = myTree.nBJet20;
    float mt2  = (njets>1) ? myTree.mt2 : ht;

    //int GenSusyMScan1=0;
    //int GenSusyMScan2=0;
    //if(  myTree.evt_id > 999){
    //  GenSusyMScan1 = myTree.GenSusyMGluino;
    // GenSusyMScan2 = myTree.GenSusyMNeutralino;
    // }
    MT2EstimateTree *thisEstimate = anaTree->get( ht, njets, nbjets, minMTBmet, mt2 );
    if( thisEstimate==0 ) continue;
    if( sample.id < 1000 ){
      thisEstimate->assignTree( myTree, weight );
      thisEstimate->tree->Fill();
    }
   
    thisEstimate->yield->Fill( mt2, weight );
    // once we have the GenSusyMScan variables, we can add the 3D EventYields_toWrite
    //thisEstimate->yield3d->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight );

  } // for entries

  anaTree->finalize();

  delete tree;

  file->Close();
  delete file;

}
/*
template <class T>
MT2Analysis<T>* computeSigYield( const MT2Sample& sample, const MT2Config& cfg ) {

  bool dogenmet = true;

  TString sigSampleName(sample.name);
  TFile* sigXSFile;
  if(sigSampleName.Contains("T1qqqq") || sigSampleName.Contains("T1bbbb") || sigSampleName.Contains("T1tttt"))
    sigXSFile = TFile::Open("/shome/casal/SUSxsecs/SUSYCrossSections13TeVgluglu.root");
  else if(sigSampleName.Contains("T2bb") || sigSampleName.Contains("T2tt") || sigSampleName.Contains("T2cc")
 || sigSampleName.Contains("T2bW") || sigSampleName.Contains("T2bt"))
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

  TTree* tree = (TTree*)file->Get("Events");


  MT2Tree myTree;
  myTree.Init(tree);



  std::cout << "-> Setting up MT2Analysis with name: " << sample.sname << std::endl;
  MT2Analysis<T>* analysis = new MT2Analysis<T>( sample.sname, regionsSet, sample.id );



  int nentries = tree->GetEntries();

  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;

    myTree.GetEntry(iEntry);

    bool passGenMET=false;
    if(dogenmet) passGenMET =true;

    bool passRecoMET=true;

//    if( regionsSet!="13TeV_noCut" )
//      if( !myTree.passSelection(cfg.additionalStuff()) ) continue;

    if( regionsSet!="13TeV_noCut" ){

      if( !myTree.passSelection(cfg.additionalStuff()) ) passRecoMET=false;

      if(dogenmet)
	if( !myTree.passSelection("genmet") ) passGenMET=false;

      if (!passGenMET && !passRecoMET) continue;

    }


    //if( myTree.nJet30==1 && !myTree.passMonoJetId(0) ) continue;
    //uncomment this line when we have implemented the function

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

    // if(dogenmet)
    //  mt2_genmet = (njets>1) ? myTree.mt2_genmet : ht;

    //for the moment we comment all the lines involving the weights
    float weight = 1.;

    //float weight_isr = myTree.weight_isr    / myTree.weight_isr_av;
    //float isr_UP     = myTree.weight_isr_UP / myTree.weight_isr_UP_av;
    //float isr_DN     = myTree.weight_isr_DN / myTree.weight_isr_DN_av;

    //float weight_lepsf = myTree.weight_lepsf;
    //float lepsf_UP     = myTree.weight_lepsf_UP;
    //float lepsf_DN     = myTree.weight_lepsf_DN;

    //float weight_btagsf = myTree.weight_btagsf;
    //float btag_heavy_UP = myTree.weight_btagsf_heavy_UP;
    //float btag_heavy_DN = myTree.weight_btagsf_heavy_DN;
    //float btag_light_UP = myTree.weight_btagsf_light_UP;
    //float btag_light_DN = myTree.weight_btagsf_light_DN;



    //int GenSusyMScan1=0;
    //int GenSusyMScan2=0;
    //if(  myTree.evt_id > 999){

      //ETH WAY
      //      if(sigSampleName.Contains("T2qq")){

      // 	GenSusyMScan1 = myTree.GenSusyMSquark;
      // 	GenSusyMScan2 = myTree.GenSusyMNeutralino;

      //       }
      //       else if(sigSampleName.Contains("T2bb")){

      // 	GenSusyMScan1 = myTree.GenSusyMSbottom;
      // 	GenSusyMScan2 = myTree.GenSusyMNeutralino;

      //       }
      //       else if( sigSampleName.Contains("T2tt") || sigSampleName.Contains("T2cc") || sigSampleName.Contains("T2bW") || sigSampleName.Contains("T2bt") ){

      // 	GenSusyMScan1 = myTree.GenSusyMStop;
      // 	GenSusyMScan2 = myTree.GenSusyMNeutralino;

      //       }
      //       else{

      //       GenSusyMScan1 = myTree.GenSusyMGluino;
      //       GenSusyMScan2 = myTree.GenSusyMNeutralino;

      // //      GenSusyMScan1 = myTree.GenSusyMScan1;
      // //      GenSusyMScan2 = myTree.GenSusyMScan2;

      //       }

      //SNT WAY
      //GenSusyMScan1 = myTree.GenSusyMScan1;
      //GenSusyMScan2 = myTree.GenSusyMScan2;

      //    std::cout << "Masses are " << GenSusyMScan1 << " and " << GenSusyMScan2 << std::endl;

      // }

    //Double_t weight = (myTree.isData) ? 1. : myTree.evt_scale1fb*cfg.lumi()*myTree.puWeight;
    
    Double_t weight = 1.;
    Double_t weight_geq20 = 1.;
    Double_t weight_l20 = 1.;



    //    Double_t weight = (myTree.isData) ? 1. : myTree.evt_scale1fb;//  *cfg.lumi();
    //Double_t weight = (myTree.isData) ? 1. : myTree.evt_scale1fb*cfg.lumi(); //Keeping normalization to luminosity for signal?
    Double_t weight_syst = 1.;

    //weight = 1000./nentries*cfg.lumi(); //Exceptionally for signal from muricans


    int binx = h_avg_weight_isr->GetXaxis()->FindBin( GenSusyMScan1 );
    int biny = h_avg_weight_isr->GetYaxis()->FindBin( GenSusyMScan2 );


    float weight_isr_geq20 = h_avg_weight_isrNVtxGeq20->GetBinContent( binx, biny );
    float weight_isr_l20 = h_avg_weight_isrNVtxL20->GetBinContent( binx, biny );

    float weight_btag_geq20 = h_avg_weight_btagsfNVtxGeq20->GetBinContent( binx, biny );
    float weight_btag_l20 = h_avg_weight_btagsfNVtxL20->GetBinContent( binx, biny );

    //will not be necessary anymore later on
    Bool_t isData = (myTree.evt_id >= 1 && myTree.evt_id < 100);

    if(!isData){

      weight *= weight_btagsf;
      weight *= weight_lepsf;
      weight *= weight_isr ;


      // std::cout << "isr geq20 = " <<  weight_isr_geq20 << " l20 = " << weight_isr_l20 << std::endl;
      //std::cout << "btag geq20 = " <<  weight_btag_geq20 << " l20 = " << weight_btag_l20 << std::endl;

      weight_geq20 *= myTree.weight_btagsf / weight_btag_geq20;
      //weight_geq20 *= myTree.weight_lepsf;
      weight_geq20 *= myTree.weight_isr / weight_isr_geq20;

      weight_l20 *= myTree.weight_btagsf / weight_btag_l20;
      //weight_l20 *= myTree.weight_lepsf;
      weight_l20 *= myTree.weight_isr / weight_isr_l20;

    }


//    if( myTree.evt_id > 1000 )
//      weight_syst = myTree.weight_isr;


    float sig_xs=0.;
    if( myTree.evt_id >= 1000  && myTree.evt_id < 2000){

      int thisBinX = sigXS->FindBin( GenSusyMScan1 );

      sig_xs = sigXS->GetBinContent(thisBinX);
      // std::cout << " sig_xs " << sig_xs << std::endl;
      weight *= sig_xs;

    }

    

    
    T* thisEstimate = analysis->get( ht, njets, nbjets, minMTBmet, mt2 );
    //T* thisEstimate = analysis->get( ht, njets, nbjets, mt2 );
    if( thisEstimate==0 ) continue;

    if(passRecoMET){

      thisEstimate->yield->Fill( mt2, weight );
      // thisEstimate->yield3d->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight );

      //    thisEstimate->yield3d_systUp->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight*(1.+(weight_syst-1.)));
      //    thisEstimate->yield3d_systDown->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight*(1.-(weight_syst-1.)));
      //    thisEstimate->yield3d_isr_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight*(1.+(isr-1.)));
      //    thisEstimate->yield3d_isr_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight*(1.-(isr-1.)));
      //    thisEstimate->yield3d_btag_heavy_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight*(1.+(btag_heavy_UP-1.)));
      //    thisEstimate->yield3d_btag_heavy_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight*(1.-(btag_heavy_DN-1.)));
      //    thisEstimate->yield3d_btag_light_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight*(1.+(btag_light_UP-1.)));
      //    thisEstimate->yield3d_btag_light_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight*(1.-(btag_light_DN-1.)));


      
      thisEstimate->yield3d_isr_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_isr*isr_UP);
      thisEstimate->yield3d_isr_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_isr*isr_DN);

      thisEstimate->yield3d_lepsf_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_lepsf*lepsf_UP);
      thisEstimate->yield3d_lepsf_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_lepsf*lepsf_DN);

      thisEstimate->yield3d_btag_heavy_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_btagsf*btag_heavy_UP);
      thisEstimate->yield3d_btag_heavy_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_btagsf*btag_heavy_DN);

      thisEstimate->yield3d_btag_light_UP->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_btagsf*btag_light_UP);
      thisEstimate->yield3d_btag_light_DN->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight/weight_btagsf*btag_light_DN);

      if( myTree.PV_npvsGood >= 20 )
	thisEstimate->yield3d_gt20->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight_geq20);
      else
	thisEstimate->yield3d_st20->Fill( mt2, GenSusyMScan1, GenSusyMScan2, weight_l20);

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
*/


/*
template <class T>
MT2Analysis<T>* mergeYields( std::vector<MT2Analysis<T> *> EventYield, const std::string& regionsSet, const std::string& name, int id_min, int id_max, const std::string& legendName ) {
  cout << "I am in mergeYields" << endl;
  if( id_max<0 ) id_max=id_min;

  MT2Analysis<T>* return_EventYield = new MT2Analysis<T>(name, regionsSet, id_min, legendName);

  for( unsigned i=0; i<EventYield.size(); ++i ) {
    std::cout << "i=" << i << std::endl;
    cout << "je suis dans la boucle" << endl;
    if( EventYield[i]->getId() >= id_min && EventYield[i]->getId() <= id_max ) {
       std::cout << "before sum" << std::endl;
       *(return_EventYield) += *(EventYield[i]);
       std::cout << "after sum" << std::endl;

    }

  } // for EventYield

  cout << "Am I out of the loop" << endl;
  return return_EventYield;

}


*/


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


int matchPartonToJet( int index, MT2Tree* myTree ) {

  float deltaRMin = 0.3; // at least 0.3
  int foundId = 0;

  TLorentzVector jet;
  jet.SetPtEtaPhiM( myTree->jet_pt[index], myTree->jet_eta[index], myTree->jet_phi[index], myTree->Jet_mass[index] );

  /*
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
      }*/

  return foundId;

}
