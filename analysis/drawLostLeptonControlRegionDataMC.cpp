#include <iostream>

#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "THStack.h"
#include "TGraphErrors.h"


#include "../interface/MT2Config.h"
#include "../interface/MT2Analysis.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2Estimate.h"
#include "../interface/MT2EstimateTree.h"
#include "../interface/MT2DrawTools.h"

using namespace std;

std::string getCutLabel( float theMin, float theMax, const std::string& name, const std::string& units="" );
std::string getJetCutLabel( int jMin, int jMax, int bMin, int bMax);



int main( int argc, char* argv[] ) {


  if( argc<2 ) {
    std::cout << "USAGE: ./drawLostLeptonControlRegionDataMC [configFileName] [lumi/shape]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  MT2DrawTools::setStyle();

  std::string configFileName(argv[1]);
  MT2Config cfg(configFileName);

  bool shapeNorm = false;
  if( argc>2 ) {
    std::string normType(argv[2]);
    if( normType=="lumi" ) shapeNorm=false;
    else if( normType=="shape" ) shapeNorm=true;
    else {
      std::cout << "-> Only 'lumi' and 'shape' are supported normTypes." << std::endl;
      exit(17);
    }
  }


  if( shapeNorm )
    std::cout << "-> Using shape normalization." << std::endl;
  else
    std::cout << "-> Using lumi normalization." << std::endl;


  std::string mcFile = cfg.getEventYieldDir() + "/llepControlRegion/mc.root";
  std::string dataFile = cfg.getEventYieldDir() + "/llepControlRegion/data.root";

  MT2Analysis<MT2EstimateTree>* mc_ = MT2Analysis<MT2EstimateTree>::readFromFile(mcFile, "llepCR");
  MT2Analysis<MT2EstimateTree>* data = MT2Analysis<MT2EstimateTree>::readFromFile(dataFile, "llepCR");
  MT2Analysis<MT2EstimateTree>* mc_top = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg.regionsSet(), mc_, "id>=300 && id<500"); 
  MT2Analysis<MT2EstimateTree>* mc_wjets = MT2EstimateTree::makeAnalysisFromInclusiveTree("W+jets", cfg.regionsSet(), mc_, "id>=500 && id<600"); 
  mc_top   ->setColor(855);
  mc_wjets ->setColor(417);

  std::vector< MT2Analysis<MT2EstimateTree>* > mc;
  // order here sets order in stack plot: W+jets on top 
  mc.push_back(mc_wjets);
  mc.push_back(mc_top);


  //---------------------------------//
  /////////////////////////////////////
  //   please set cuts on Ht here    //
 
  //please consider only ints for the range of ht!
  int htMin=575, htMax=1000;
  
  std::string cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  string ht_min = to_string(htMin);
  string ht_max = to_string(htMax);
 //plot range of the ht distribution
  double htInf = htMin-10;//200;
  double htSup(2050);
  if(htMax == -1)  htSup = 2050; 
  else htSup = htMax + 10;
  //name of the directory
  string dirName = ht_min + "ht" + ht_max;

  /////////////////////////////////////
  //---------------------------------//



  //---------------------------------//
  /////////////////////////////////////
  //  please decide on e or u here   //

  bool plotElectron = false;
  bool plotMuon = false;

  if(plotElectron && !plotMuon) cout << endl <<  "Plots Control Region with an electron" << endl << endl;
  else if(plotMuon && !plotElectron) cout << endl << "Plots Control Region with a muon" << endl << endl;
  else cout << endl << "Plots Control Region with a non-specified lepton" << endl << endl;

  //REMINDER: pdgId for e = +-11
  //                    u = +-13

  /////////////////////////////////////
  //---------------------------------//



  std::string plotsDir = cfg.getEventYieldDir() + "/llepControlRegion/plotsDataMC";
  std::string plotsDir2;
  if(plotElectron && !plotMuon){
    plotsDir2 = cfg.getEventYieldDir() +  "/llepControlRegion/plotsDataMC/plots/test/electron/" + dirName;
  }
  else if(plotMuon && !plotElectron){
    plotsDir2 = cfg.getEventYieldDir() +  "/llepControlRegion/plotsDataMC/plots/test/muon/" + dirName;
  }
  else{
    plotsDir2 = cfg.getEventYieldDir() +  "/llepControlRegion/plotsDataMC/plots/test/" + dirName;
  }
 
  
  //warning: change of the directory here
  if(shapeNorm) plotsDir2 += "_shape";

  //warning: change of the directory here
  MT2DrawTools dt(plotsDir2, cfg.lumi());
  dt.set_shapeNorm(shapeNorm);

  dt.set_data(data);
  dt.set_mc(&mc);

  dt.set_lumi(cfg.lumi());
  dt.set_year(cfg.year());
 


  // +++++++++++++++++++++++++
  // +++      Inclusive    +++
  // +++++++++++++++++++++++++

 
  
  // float htMin=250, htMax=-1;
  //std::string cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  
  int jetMin=2, jetMax=-1;
  int bMin=0, bMax=-1;
  std::string jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);
  
  //std::string selection = "ht>200. && met>200  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  // std::string selection = "ht>250. && met>250  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  //std::string selection = "ht>1000. && met>30  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  // std::string selection = "ht>250 && ht<450 && met>250  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  std::string selection;
  if(plotElectron && !plotMuon){
   selection = "abs(lepID)<12 && ht>" + ht_min + "&& met>250  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  }
  else if(plotMuon && !plotElectron){
   selection = "abs(lepID)>12 && ht>" + ht_min + "&& met>250  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  }
  else{
   selection = "ht>" + ht_min + "&& met>250  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  }
  if(htMax != -1){
    selection += "&& ht<" + ht_max;
  }


 
   
  dt.drawRegionYields_fromTree("incl_nVert", "nVert", selection, 100, -0.5, 89.5, "Number of Vertices", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_mt2", "mt2", selection, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_met", "met", selection, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_ht", "ht", selection, 37, htInf, htSup, "H_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_nJets", "nJets", selection, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_nBJets", "nBJets", selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel);

  //additional inclusive plots
  // dt.drawRegionYields_fromTree("lumi", "lumi", selection, 50, 0, 3500, "Luminosity", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_deltaPhiMin", "deltaPhiMin", selection, 16, -0.5, 3.5, "#Delta #Phi_{min}", "", cutsLabel, jetCutsLabel);
  // dt.drawRegionYields_fromTree("nElectrons", "nElectrons", selection, 4, 0, 4, "Number of electrons", "", cutsLabel, jetCutsLabel);
  // dt.drawRegionYields_fromTree("nMuons", "nMuons", selection, 4, 0, 4, "Number of muons", "", cutsLabel, jetCutsLabel);
  // dt.drawRegionYields_fromTree("met_pt", "met_pt", selection, 100, 200, 1000, "MET pT", "GeV", cutsLabel, jetCutsLabel);
  
  //lepton kinematics
  dt.drawRegionYields_fromTree("incl_lepPt", "lepPt", selection, 70, 0, 700, "Lepton p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_lepEta", "lepEta", selection, 21, -3.5, 3.5, "Lepton eta", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_lepPhi", "lepPhi", selection, 40, -3, 3, "Lepton phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_lepMass", "lepMass", selection, 5, 0, 1, "Lepton mass", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_lepID", "lepID", selection, 70, -15, 15, "Lepton ID", "", cutsLabel, jetCutsLabel);

  dt.drawRegionYields_fromTree("incl_met_pt", "met_pt", selection, 100, 0, 1000, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_met_phi", "met_phi", selection, 40, -4, 4, "E_{T}^{miss} phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_mht_pt", "mht_pt", selection, 100, 0, 1000, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_mht_phi", "mht_phi", selection, 40, -4, 4, "H_{T}^{miss} phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("incl_diffMetMht", "diffMetMht", selection, 50, 0, 250, "|E_{T}^{miss}-H_{T}^{miss}|", "GeV", cutsLabel, jetCutsLabel);

  /*

  // +++++++++++++++++++++++++
  // +++      b-veto       +++
  // +++++++++++++++++++++++++

  //htMin=250, htMax=-1;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=2, jetMax=-1;
  bMin=0, bMax=0;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  //selection = "ht>200 && met>200. && nJets>1 && nBJets==0 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  // selection = "ht>250 && met>250. && nJets>1 && nBJets==0 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
   selection = "ht>" + ht_min + "&& ht<" + ht_max + "&& met>250  && nJets>1 && nBJets==0 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
    
  dt.drawRegionYields_fromTree("0b_nVert", "nVert", selection, 100, -0.5, 89.5, "Number of Vertices", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_mt2", "mt2", selection, 10, 200., 1200., "M_{T2}", "GeV" , cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_met", "met", selection, 10, 200., 1200., "Missing E_{T}" , "GeV" , cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_ht" , "ht" , selection, 37, htInf, htSup, "H_{T}", "GeV" , cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_nJets", "nJets", selection, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_nBJets", "nBJets", selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel);

  dt.drawRegionYields_fromTree("0b_deltaPhiMin", "deltaPhiMin", selection, 16, -0.5, 3.5, "Delta #Phi_{min}", "", cutsLabel, jetCutsLabel);
     
  dt.drawRegionYields_fromTree("0b_lepPt", "lepPt", selection, 70, 0, 700, "Lepton p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_lepEta", "lepEta", selection, 21, -3.5, 3.5, "Lepton eta", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_lepPhi", "lepPhi", selection, 40, -3, 3, "Lepton phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_lepMass", "lepMass", selection, 5, 0, 1, "Lepton mass", "GeV", cutsLabel, jetCutsLabel);

  dt.drawRegionYields_fromTree("0b_met_pt", "met_pt", selection, 100, 0, 1000, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_met_phi", "met_phi", selection, 40, -4, 4, "E_{T}^{miss} phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_mht_pt", "mht_pt", selection, 100, 0, 1000, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_mht_phi", "mht_phi", selection, 40, -4, 4, "H_{T}^{miss} phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("0b_diffMetMht", "diffMetMht", selection, 50, 0, 250, "|E_{T}^{miss}-H_{T}^{miss}|", "GeV", cutsLabel, jetCutsLabel);

  // +++++++++++++++++++++++++
  // +++     b-enriched    +++
  // +++++++++++++++++++++++++

  //htMin=250, htMax=-1;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  
  jetMin=2, jetMax=-1;
  bMin=2, bMax=-1;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  //selection = "ht>200 && met>200. && nJets>1 && nBJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met &&  mt2>200.";
  // selection = "ht>250 && met>250. && nJets>1 && nBJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met &&  mt2>200.";
   selection = "ht>" + ht_min + "&& ht<" + ht_max + "&& met>250  && nJets>1 && nBJets>1  && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  
  dt.drawRegionYields_fromTree("2b_nVert", "nVert", selection, 100, -0.5, 89.5, "Number of Vertices", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_mt2", "mt2", selection, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_met", "met", selection, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_ht", "ht", selection, 37, htInf, htSup, "H_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_nJets", "nJets", selection, 12, -0.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree( "2b_nBJets", "nBJets", selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel);

  dt.drawRegionYields_fromTree("2b_deltaPhiMin", "deltaPhiMin", selection, 16, -0.5, 3.5, "#Delta #Phi_{min}", "", cutsLabel, jetCutsLabel);
    
  dt.drawRegionYields_fromTree("2b_lepPt", "lepPt", selection, 70, 0, 700, "Lepton p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_lepEta", "lepEta", selection, 21, -3.5, 3.5, "Lepton eta", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_lepPhi", "lepPhi", selection, 40, -3, 3, "Lepton phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_lepMass", "lepMass", selection, 5, 0, 1, "Lepton mass", "GeV", cutsLabel, jetCutsLabel);

  dt.drawRegionYields_fromTree("2b_met_pt", "met_pt", selection, 100, 0, 1000, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_met_phi", "met_phi", selection, 40, -4, 4, "E_{T}^{miss} phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_mht_pt", "mht_pt", selection, 100, 0, 1000, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_mht_phi", "mht_phi", selection, 40, -4, 4, "H_{T}^{miss} phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("2b_diffMetMht", "diffMetMht", selection, 50, 0, 250, "|E_{T}^{miss}-H_{T}^{miss}|", "GeV", cutsLabel, jetCutsLabel);

  // +++++++++++++++++++++++++
  // +++       Monojet     +++
  // +++++++++++++++++++++++++

  dt.set_lumi( cfg.lumi() );

  //htMin=250, htMax=-1;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=1, jetMax=1;
  bMin=0, bMax=-1;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  //selection = "ht>200. && met>200  && nJets==1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  // selection = "ht>250. && met>250  && nJets==1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  selection = "ht>" + ht_min + "&& ht<" + ht_max + "&& met>250  && nJets==1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";

  //  dt.drawRegionYields_fromTree( "monojet_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  
  dt.drawRegionYields_fromTree("monojet_nVert", "nVert", selection, 100, -0.5, 89.5, "Number of Vertices", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_met", "met", selection, 10, 200., 1200., "Missing E_{T}", "GeV" , cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_ht", "ht", selection, 37, htInf, htSup, "H_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_nJets", "nJets", selection, 12, -0.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_nBJets", "nBJets", selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel);
  
  dt.drawRegionYields_fromTree("monojet_deltaPhiMin", "deltaPhiMin", selection, 16, -0.5, 3.5, "#Delta #Phi_{min}", "", cutsLabel, jetCutsLabel);
 
  dt.drawRegionYields_fromTree("monojet_lepPt", "lepPt", selection, 70, 0, 700, "Lepton p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_lepEta", "lepEta", selection, 21, -3.5, 3.5, "Lepton eta", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_lepPhi", "lepPhi", selection, 40, -3, 3, "Lepton phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_lepMass", "lepMass", selection, 5, 0, 1, "Lepton mass", "GeV", cutsLabel, jetCutsLabel);

  dt.drawRegionYields_fromTree("monojet_met_pt", "met_pt", selection, 100, 0, 1000, "E_{T}^{miss} pT", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_met_phi", "met_phi", selection, 40, -4, 4, "E_{T}^{miss} phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_mht_pt", "mht_pt", selection, 100, 0, 1000, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_mht_phi", "mht_phi", selection, 40, -4, 4, "H_{T}^{miss} phi", "", cutsLabel, jetCutsLabel);
  dt.drawRegionYields_fromTree("monojet_diffMetMht", "diffMetMht", selection, 50, 0, 250, "|E_{T}^{miss}-H_{T}^{miss}|", "GeV", cutsLabel, jetCutsLabel);
  */
/*
  // +++++++++++++++++++++++++
  // +++      b-veto       +++
  // +++++++++++++++++++++++++

  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=1, jetMax=1;
  bMin=0, bMax=0;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>200 && met>200. && nJets==1 && nBJets==0 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  
  //  dt.drawRegionYields_fromTree( "monojet_0b_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "monojet_0b_met"              , "met"                , selection, 18, 0.    , 900.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "monojet_0b_ht"               , "ht"                 , selection, 37, 200.  , 2050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "monojet_0b_nJets"            , "nJets"              , selection, 12, -0.5   , 11.5  , "Number of Jets (p_{T} > 30 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "monojet_0b_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 20 GeV)", ""    , cutsLabel, jetCutsLabel );



  // +++++++++++++++++++++++++
  // +++     b-enriched    +++
  // +++++++++++++++++++++++++

  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=1, jetMax=1;
  bMin=2, bMax=-1;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>200 && met>200. && nJets==1 && nBJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met &&  mt2>200.";

  //dt.drawRegionYields_fromTree( "monojet_2b_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "monojet_2b_met"              , "met"                , selection, 18, 0.    , 900.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "monojet_2b_ht"               , "ht"                 , selection, 37, 200.  , 2050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "monojet_2b_nJets"            , "nJets"              , selection, 12, -0.5  , 11.5  , "Number of Jets (p_{T} > 30 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "monojet_2b_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 20 GeV)", ""    , cutsLabel, jetCutsLabel );


  // +++++++++++++++++++++++++
  // +++     Very Low HT   +++
  // +++++++++++++++++++++++++

  dt.set_lumi( cfg.lumi() );

  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=2, jetMax=-1;
  bMin=0, bMax=-1;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>200. && ht<450. && met>200  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";

  dt.drawRegionYields_fromTree( "veryLowHT_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_met"              , "met"                , selection, 18, 0.    , 900.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_ht"               , "ht"                 , selection, 37, 200.  , 2050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_nJets"            , "nJets"              , selection, 10, 1.5   , 11.5  , "Number of Jets (p_{T} > 30 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 20 GeV)", ""    , cutsLabel, jetCutsLabel );



  // +++++++++++++++++++++++++
  // +++      b-veto       +++
  // +++++++++++++++++++++++++

  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=2, jetMax=-1;
  bMin=0, bMax=0;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>200. && ht<450. && met>200  && nJets>1 && nBJets==0 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  
  dt.drawRegionYields_fromTree( "veryLowHT_0b_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_0b_met"              , "met"                , selection, 18, 0.    , 900.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_0b_ht"               , "ht"                 , selection, 37, 200.  , 2050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_0b_nJets"            , "nJets"              , selection, 10, 1.5   , 11.5  , "Number of Jets (p_{T} > 30 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_0b_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 20 GeV)", ""    , cutsLabel, jetCutsLabel );



  // +++++++++++++++++++++++++
  // +++     b-enriched    +++
  // +++++++++++++++++++++++++

  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=2, jetMax=-1;
  bMin=2, bMax=-1;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>200. && ht<450. && met>200  && nJets>1 && nBJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met &&  mt2>200.";

  dt.drawRegionYields_fromTree( "veryLowHT_2b_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_2b_met"              , "met"                , selection, 18, 0.    , 900.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_2b_ht"               , "ht"                 , selection, 37, 200.  , 2050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_2b_nJets"            , "nJets"              , selection, 12, -0.5  , 11.5  , "Number of Jets (p_{T} > 30 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "veryLowHT_2b_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 20 GeV)", ""    , cutsLabel, jetCutsLabel );


  // +++++++++++++++++++++++++
  // +++     High HT   +++
  // +++++++++++++++++++++++++

  dt.set_lumi( cfg.lumi() );

  htMin=450, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=2, jetMax=-1;
  bMin=0, bMax=-1;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>450. && met>200  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";

  dt.drawRegionYields_fromTree( "LowHT_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_met"              , "met"                , selection, 18, 0.    , 900.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_ht"               , "ht"                 , selection, 37, 200.  , 2050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_nJets"            , "nJets"              , selection, 10, 1.5   , 11.5  , "Number of Jets (p_{T} > 30 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 20 GeV)", ""    , cutsLabel, jetCutsLabel );



  // +++++++++++++++++++++++++
  // +++      b-veto       +++
  // +++++++++++++++++++++++++


  jetMin=2, jetMax=-1;
  bMin=0, bMax=0;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>450. && met>200  && nJets>1 && nBJets==0 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
  
  dt.drawRegionYields_fromTree( "LowHT_0b_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_0b_met"              , "met"                , selection, 18, 0.    , 900.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_0b_ht"               , "ht"                 , selection, 37, 200.  , 2050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_0b_nJets"            , "nJets"              , selection, 10, 1.5   , 11.5  , "Number of Jets (p_{T} > 30 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_0b_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 20 GeV)", ""    , cutsLabel, jetCutsLabel );



  // +++++++++++++++++++++++++
  // +++     b-enriched    +++
  // +++++++++++++++++++++++++

  jetMin=2, jetMax=-1;
  bMin=2, bMax=-1;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>450. && met>200  && nJets>1 && nBJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met &&  mt2>200.";

  dt.drawRegionYields_fromTree( "LowHT_2b_mt2"              , "mt2"                , selection, 12, 0.    , 600.  , "M_{T2}"          , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_2b_met"              , "met"                , selection, 18, 0.    , 900.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_2b_ht"               , "ht"                 , selection, 37, 200.  , 2050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_2b_nJets"            , "nJets"              , selection, 12, -0.5  , 11.5  , "Number of Jets (p_{T} > 30 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "LowHT_2b_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 20 GeV)", ""    , cutsLabel, jetCutsLabel );


  // +++++++++++++++++++++++++
  // +++     EAs common    +++
  // +++++++++++++++++++++++++

  dt.set_lumi( cfg.lumi() );

  htMin=500, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  jetMin=2, jetMax=-1;
  bMin=0, bMax=-1;
  jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  selection = "ht>500. && met>250.  && nJets40>1";

  dt.drawRegionYields_fromTree( "EAs_lepPt"              , "lep_pt"                , selection, 24, 0.    , 600.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "EAs_met"              , "met"                , selection, 27, 200.    , 1550.  , "Missing E_{T}"                    , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "EAs_ht"               , "ht"                 , selection, 52, 450.  , 3050. , "H_{T}"                            , "GeV" , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "EAs_nJets"            , "nJets40"              , selection, 10, 1.5   , 11.5  , "Number of Jets (p_{T} > 40 GeV)"  , ""    , cutsLabel, jetCutsLabel );
  dt.drawRegionYields_fromTree( "EAs_nBJets"           , "nBJets"             , selection, 6 , -0.5  , 5.5   , "Number of b-Jets (p_{T} > 40 GeV)", ""    , cutsLabel, jetCutsLabel );

*/


  return 0;

}

std::string getCutLabel( float theMin, float theMax, const std::string& name, const std::string& units ) {

  std::string cutLabel;
  if( theMax>theMin ) cutLabel = std::string(Form("%.0f < %s < %.0f %s", theMin, name.c_str(), theMax, units.c_str()) );
  else                cutLabel = std::string(Form("%s > %.0f %s", name.c_str(), theMin, units.c_str()) );

  return cutLabel;

}

std::string getJetCutLabel( int jetMin, int jetMax, int bMin, int bMax ) {

  std::string jetCutLabel;
  if( jetMax>jetMin ) jetCutLabel = std::string(Form("%d-%dj, ", jetMin, jetMax));
  else if( jetMax==jetMin ) jetCutLabel = std::string(Form("%dj, ", jetMin));
  else                jetCutLabel = std::string(Form("#geq%dj, ", jetMin));
  
  if( bMax>bMin )     jetCutLabel = jetCutLabel + (std::string)(Form("%d-%db", bMin, bMax));
  else if (bMax<bMin) jetCutLabel = jetCutLabel + (std::string)(Form("#geq%db", bMin));
  else                jetCutLabel = jetCutLabel + (std::string)(Form("%db", bMin));
  
  return jetCutLabel;

}
