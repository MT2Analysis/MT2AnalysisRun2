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
#include "interface/MT2PuReweightTool.h"
#include "../interface/MT2LeptonSFTool.h"
//#include "interface/MT2GoodrunClass.h"
//
#include "TRandom3.h"
//
#include "interface/Utils.h" //to get the getNgen function
//
#define mt2_cxx
#include "interface/mt2.h"
void addweight_bkg(const MT2Sample& sample, const MT2Config& cfg);
void addweight_sig(const MT2Sample& sample, const MT2Config& cfg);
void addweight_zll(const MT2Sample& sample, const MT2Config& cfg);
TH2D* h_nsig;
TH2D* h_avg_weight_btagsf ;
TH2D* h_avg_weight_btagsf_heavy_UP ;
TH2D* h_avg_weight_btagsf_light_UP ;
TH2D* h_avg_weight_btagsf_heavy_DN ;
TH2D* h_avg_weight_btagsf_light_DN ;
TH2D* h_avg_weight_isr;
TH2D* h_avg_weight_isr_UP;
TH2D* h_avg_weight_isr_DN;
int main(int argc, char* argv[]){
  if( argc<2 ) {
    std::cout << "USAGE: ./addweight [configFileName] [data/MC/zllCR/llepCR]" << std::endl;
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
  bool onlyzllCR = false;
  bool onlyllepCR = false;
  if( argc > 2 ) {
    std::string dataMC(argv[2]);
    if( dataMC=="data" ) onlyData = true;
    else if( dataMC=="MC" || dataMC=="mc" ) onlyMC = true;
    else if( dataMC=="signal" ) onlySignal = true;
    else if( dataMC=="zllCR") onlyzllCR = true;
    else if( dataMC=="llepCR") onlyllepCR = true;
    else {
      std::cout << "-> You passed a second argument that isn't 'data', nor 'mc', nor 'signal', nor 'zllCR', nor 'llepCR', so I don't know what to do about it." << std::endl;
    }
  }

  std::string signalName = "";
  if( argc > 3 ) {
    signalName = argv[3];
    std::cout << "-> Running on signal: " << signalName << std::endl;
  }



  if( argc > 3){
    std::string model = signalName;
    int year_to_use = (cfg.year()!=2018) ? cfg.year() : 2017;
    std::string filename = Form("/t3home/mratti/MT2/SigWeights/%d/nsig_weights_%s.root", year_to_use, model.c_str() );
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
    std::map<std::string, MT2Analysis<MT2EstimateTree>*> mcSRMap;

  if( cfg.useMC() && !onlyData && !onlySignal && onlyMC) {
    std::string samplesFileName = "../samples/samples_" + cfg.mcSamples() + ".dat";
    std::vector<std::string> sampleNames = {"Top", "WJets", "QCD", "ZJets"};
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;
    std::map<TString, std::vector<MT2Sample>> fSamplesMap;
    std::cout << "-> Creating sample list for Top " << std::endl;
    fSamplesMap["Top"] = (MT2Sample::loadSamples(samplesFileName, 300, 499, cfg.useETHmc()));
    std::cout << "-> Creating sample list for WJets " << std::endl;
    fSamplesMap["WJets"] =(MT2Sample::loadSamples(samplesFileName, 500, 509, cfg.useETHmc()));
    std::cout << "-> Creating sample list for QCD " << std::endl;
    fSamplesMap["QCD"] =(MT2Sample::loadSamples(samplesFileName, 100, 199, cfg.useETHmc()));
    std::cout << "-> Creating sample list for ZJets " << std::endl;
    fSamplesMap["ZJets"] =(MT2Sample::loadSamples(samplesFileName, 600, 699, cfg.useETHmc()));
    for(auto sampleName : sampleNames){
      std::cout << "-> Considering group " << sampleName << std::endl;
      for (auto fSample : fSamplesMap[sampleName]){
          addweight_bkg(fSample,cfg);
      }
      std::cout << "-> Done looping on samples for this group" << std::endl;
    }
  }

  if(cfg.useMC() && !onlyData && !onlySignal && onlyllepCR){
    std::string samplesFileName = "../samples/samples_" + cfg.llepmcSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::vector<std::string> sampleNames = {"ll"};
    std::cout << "-> Creating sample list for ll control " << std::endl;
    std::map<TString, std::vector<MT2Sample>> fSamplesMap;
    fSamplesMap["ll"] =(MT2Sample::loadSamples(samplesFileName, 300, 599, cfg.useETHmc()));
    std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;
    for(auto sampleName : sampleNames){
      std::cout << "-> Considering group " << sampleName << std::endl;
      for (auto fSample : fSamplesMap[sampleName]){
          addweight_bkg(fSample,cfg);
      }
      std::cout << "-> Done looping on samples for this group" << std::endl;
    }
  }
  
  if(cfg.useMC() && !onlyData && !onlySignal && onlyzllCR){
    std::string samplesFileName = "../samples/samples_" + cfg.zllmcSamples() + ".dat";
    std::cout << std::endl << std::endl;
    std::vector<std::string> sampleNames = {"DY"};
    std::cout << "-> Creating sample list for zll control " << std::endl;
    std::map<TString, std::vector<MT2Sample>> fSamplesMap;
    fSamplesMap["DY"] =(MT2Sample::loadSamples(samplesFileName, 700, 799, cfg.useETHmc()));
    std::cout << "-> Loading samples from file: " << samplesFileName << std::endl;
    for(auto sampleName : sampleNames){
      std::cout << "-> Considering group " << sampleName << std::endl;
      for (auto fSample : fSamplesMap[sampleName]){
          addweight_zll(fSample,cfg);
      }
      std::cout << "-> Done looping on samples for this group" << std::endl;
    }
  }


  if( cfg.sigSamples()!="" && cfg.additionalStuff()!="noSignals" && !onlyData && onlySignal) {
    std::string samplesFileName = "../samples/samples_" + cfg.sigSamples() + ".dat";
    std::string samplesMLscore;
    std::cout << std::endl << std::endl;
    std::cout << "-> Loading signal samples from file: " << samplesFileName << std::endl;
    std::cout << "     signal name " << signalName << std::endl;
    std::vector<MT2Sample> fSamplesSig;
    if       (signalName=="T1qqqq")   fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1000, 1100, cfg.useETHmc());
    else if  (signalName=="T1bbbb")   fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1100, 1200, cfg.useETHmc());
    else if  (signalName=="T1tttt")   fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1200, 1300, cfg.useETHmc());
    else if  (signalName=="T2qq")     fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1300, 1400, cfg.useETHmc());
    else if  (signalName=="T2bb")     fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1400, 1500, cfg.useETHmc());
    else if  (signalName=="T2tt")     fSamplesSig=MT2Sample::loadSamples(samplesFileName, 1500, 1600, cfg.useETHmc());
    
    std::cout << "-> Considering group " << signalName << std::endl;
    if( fSamplesSig.size()==0 ) {
      std::cout << "No signal samples found, skipping." << std::endl;
    }
    else{
      for (int i=0;i<fSamplesSig.size();i++){
            addweight_sig(fSamplesSig[i],cfg);
      }
      std::cout << "-> Done looping on samples for this group" << std::endl;
    }
  }
}



void addweight_bkg(const MT2Sample& sample, const MT2Config& cfg){
  cout<<"initialize?"<<endl;
  MT2PuReweightTool puReweight; cout<<"initialize "<<endl;
  bool puHist = puReweight.setPuWeightHist(cfg.year());
  bool  isData = (sample.id >= -1 && sample.id < 100 );
  std::cout << " sample.id=" << sample.id << " isData=" << isData << std::endl;

  bool isETH = (isData and cfg.useETHdata()) || (!isData and cfg.useETHmc());
  TString treeName = isETH ? "Events" : "mt2";
  TFile* file=new TFile(sample.file.c_str(),"update");
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;
  TTree* tree = (TTree*)file->Get(treeName);
  MT2Tree myTree(tree, isETH);
  double nGen=-9999; double nGenWeighted=-9999;
  if(!isData and isETH){
    nGen = getNgen(sample.file, "genEventCount");
    nGenWeighted = getNgen(sample.file, "genEventSumw");
  }
  int nentries = tree->GetEntries();
  double absweight;
  TBranch *newBranch=tree->Branch("absweight",&absweight,"absweight/D");
  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ){
      std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;
    }

    myTree.GetEntry(iEntry);
    tree->GetEntry(iEntry);

    Double_t weight_syst = 1.;
    Double_t weight(1.);

    if(isData){
      weight = 1.;
    }
    else{
      if (isETH) weight =  myTree.evt_xsec * myTree.evt_kfactor * myTree.evt_filter * 1000/nGen;
      else {
        weight = myTree.evt_scale1fb;
        if(myTree.genWeight < 0 && weight > 0) weight *= -1.0;

        weight *= myTree.weight_lepsf * myTree.weight_btagsf*cfg.lumi();

        if(puHist) weight *= puReweight.getPuWeight(myTree.nTrueInt);

        if (cfg.year()==2017 || cfg.year()==2016) weight *= myTree.weight_L1prefire;

        if ((sample.id==301 || sample.id==302 || sample.id==303) && cfg.year()==2016) weight *= myTree.weight_isr / myTree.getAverageISRWeight(sample.id,cfg.year(),0);

        weight *= myTree.getTTHFWeight(sample.id);
      }
      absweight=weight;
      newBranch->Fill();
    }
  }
  tree->Write("", TObject::kOverwrite);
  file->Close();
  delete file;
}



void addweight_sig(const MT2Sample& sample, const MT2Config& cfg){
  MT2PuReweightTool puReweight;
  bool puHist = puReweight.setPuWeightHist(cfg.year());
  bool dogenmet = true;
  TString sigSampleName(sample.name);
  TFile* sigXSFile;
  TString fileName;
  TString histoName;
  if(sigSampleName.Contains("T1qqqq") || sigSampleName.Contains("T1bbbb") || sigSampleName.Contains("T1tttt")){
    fileName = "gluino.root";
    histoName = "gluglu";
  }
  else if (sigSampleName.Contains("T2bb") || sigSampleName.Contains("T2tt") || sigSampleName.Contains("T2cc")|| sigSampleName.Contains("T2bW") || sigSampleName.Contains("T2bt")) {
    fileName = "stop.root";
    histoName = "thirdGen";
  }else{
    fileName = "squark.root";
    histoName = "sqsq";
  }
  sigXSFile = TFile::Open("/work/mratti/SUSxsecs/NNLO_approx_NNLL_80X_compatible/" + fileName);
  TH1F* sigXS = (TH1F*) sigXSFile->Get(histoName);


  if(sigSampleName.Contains("T2qq"))
    sigXS->Scale(8./10);
  std::string regionsSet = cfg.regionsSet();
  bool  isData = (sample.id >= -1 && sample.id < 100 );
  bool isETH = (isData and cfg.useETHdata()) || (!isData and cfg.useETHmc());
  TString treeName = isETH ? "Events" : "mt2";
  TFile* file=new TFile(sample.file.c_str(),"update");
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;
  TTree* tree = (TTree*)file->Get(treeName);
  MT2Tree myTree(tree, isETH);
  int nentries = tree->GetEntries();
  double absweight;
  TBranch *newBranch=tree->Branch("absweight",&absweight,"absweight/D");
  for( int iEntry=0; iEntry<nentries; ++iEntry ) {

    if( iEntry % 50000 == 0 ) std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;
    myTree.GetEntry(iEntry);
    tree->GetEntry(iEntry);
    float ht   = myTree.ht;
    float minMTBmet = -999;
    int njets  = myTree.nJet30;
    int nbjets = myTree.nBJet20;
    float mt2  = (njets>1) ? myTree.mt2 : ht;
    float mt2_genmet;

    if(dogenmet) mt2_genmet = (njets>1) ? myTree.mt2_genmet : ht;
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
      GenSusyMScan1 = myTree.GenSusyMScan1;
      GenSusyMScan2 = myTree.GenSusyMScan2;
    }
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
    Double_t weight = 1.;
    weight = weight_btagsf * weight_lepsf * weight_isr / (nevts*weight_avg_isr*weight_avg_btagsf) ;
    float sig_xs=0.;
    if( myTree.evt_id >= 1000  && myTree.evt_id < 2000){

      int thisBinX = sigXS->FindBin( GenSusyMScan1 );
      sig_xs = sigXS->GetBinContent(thisBinX);
      if (isETH){
        weight *= sig_xs;
      }
      else{
        weight *= sig_xs*myTree.evt_filter*1000*cfg.lumi();
        if(cfg.year()==2016 || cfg.year()==2017) weight *= myTree.weight_L1prefire;
        if(puHist) weight *= puReweight.getPuWeight(myTree.nTrueInt);
      }
    }
    else  std::cout << "THIS SHOULD NOT HAPPEN, PLEASE CHECK" << std::endl;
    absweight=weight;
    newBranch->Fill();
  }
  tree->Write("", TObject::kOverwrite);
  file->Close();
  delete file;
}

void addweight_zll(const MT2Sample& sample, const MT2Config& cfg){
  MT2PuReweightTool puReweight;
  bool puHist = puReweight.setPuWeightHist(cfg.year());
  bool  isData = (sample.id >= -1 && sample.id < 100 );
  MT2LeptonSFTool leptonSF;
  bool electronHist = leptonSF.setElHist("zll");
  bool muonHist = leptonSF.setMuHist();
  bool diLepTrigEffHist = leptonSF.setDiLepTriggerHist(cfg.year());
  bool isETH = (isData and cfg.useETHdata()) || (!isData and cfg.useETHmc());
  TString treeName = isETH ? "Events" : "mt2";
  TFile* file=new TFile(sample.file.c_str(),"update");
  std::cout << "-> Getting mt2 tree from file: " << sample.file << std::endl;
  TTree* tree = (TTree*)file->Get(treeName);
  MT2Tree myTree(tree, isETH);
  double nGen=-9999; double nGenWeighted=-9999;
  if(!isData and isETH){
    nGen = getNgen(sample.file, "genEventCount");
    nGenWeighted = getNgen(sample.file, "genEventSumw");
  }
  int nentries = tree->GetEntries();
  double absweight;
  TBranch *newBranch=tree->Branch("absweight",&absweight,"absweight/D");
  for( int iEntry=0; iEntry<nentries; ++iEntry ) {
    if( iEntry % 50000 == 0 ){
      std::cout << "    Entry: " << iEntry << " / " << nentries << std::endl;
    }
    myTree.GetEntry(iEntry);
    tree->GetEntry(iEntry);
    int ID = myTree.evt_id;
    int lep0_pdgId_to_use = -1;
    int lep1_pdgId_to_use = -1;
    lep0_pdgId_to_use = myTree.lep_pdgId[0];
    lep1_pdgId_to_use = myTree.lep_pdgId[1];
    Double_t weight_syst = 1.;
    Double_t weight(1.);
    if(isData){
      weight = 1.;
    }
    else{
      if (isETH) weight =  myTree.evt_xsec * myTree.evt_kfactor * myTree.evt_filter * 1000/nGen;
      else {
        weight = myTree.evt_scale1fb;
        if(myTree.genWeight < 0 && weight > 0) weight *= -1.0;
        weight *= myTree.weight_lepsf * myTree.weight_btagsf*cfg.lumi();
        if(puHist) weight *= puReweight.getPuWeight(myTree.nTrueInt);
        if (cfg.year()==2017 || cfg.year()==2016) weight *= myTree.weight_L1prefire;
        if ((sample.id==301 || sample.id==302 || sample.id==303) && cfg.year()==2016) weight *= myTree.weight_isr / myTree.getAverageISRWeight(sample.id,cfg.year(),0);
        weight *= myTree.getTTHFWeight(sample.id);
      }
    }
    if(ID >999)
      weight = 1000.*myTree.evt_xsec / nentries;
    float HLT_weight=1.;
    if (!isData && diLepTrigEffHist) {
      lepSF diLepTrigEffSF = leptonSF.getDiLepTriggerSF(myTree.lep_pt[0],lep0_pdgId_to_use,myTree.lep_pt[1],lep0_pdgId_to_use);
      HLT_weight = diLepTrigEffSF.sf;
      weight *= HLT_weight;
    }
    absweight=weight;
    newBranch->Fill();
  }
  tree->Write("", TObject::kOverwrite);
  file->Close();
  delete file;
}
