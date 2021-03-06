#include "TFile.h"
#include "TMath.h"
#include "TF1.h"
#include "TProfile.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TTree.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TGaxis.h"
#include "THStack.h"
#include "TLorentzVector.h"
#include "TGraphErrors.h"

#include "interface/MT2Config.h"
#include "interface/MT2Sample.h"
#include "interface/MT2Region.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"
#include "interface/MT2EstimateTree.h"
#include "interface/MT2DrawTools.h"


#include <iostream>
#include "string.h"


#define mt2_cxx
#include "interface/mt2.h"


double lumiErr = 0.046;
bool shapeNorm = false;


void drawMll( const std::string& outputdir,  std::vector<MT2Analysis<MT2EstimateTree>* > bgYields, MT2Analysis<MT2EstimateTree>* data, bool of, float lumi ); 
void drawStacks(std::string fullPath, float *binss, unsigned int size,  std::string name, std::vector<MT2Analysis<MT2EstimateTree>* > bgYields,  MT2Analysis<MT2EstimateTree>* data,const MT2Region thisRegion, std::string cut, float lumi);
void drawYields( MT2Config cfg, MT2Analysis<MT2EstimateTree>* data, std::vector<MT2Analysis<MT2EstimateTree>* >  bgYields, const std::string& saveName, const std::string& varName, const std::string& selection, int nBins, float xMin, float xMax, std::string axisName="", const std::string& units="" );
std::string getCutLabel( float theMin, float theMax, const std::string& name, const std::string& units );


int main(int argc, char* argv[]){

  std::string regionsSet = "zurich";


  if( argc>2 ) {
    std::string normType(argv[4]);
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

  if( argc<4 ) {
    std::cout << "USAGE: ./drawZllControlRegion [configFileName1] [configFileName2] [configFileName3] (lumi/shape)" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

  std::string configFileName1(argv[1]);
  MT2Config cfg1( configFileName1);

  std::string configFileName2(argv[2]);
  MT2Config cfg2( configFileName2);

  std::string configFileName3(argv[3]);
  MT2Config cfg3( configFileName3);
 
  regionsSet = cfg1.regionsSet();

  std::string outputdir1 = cfg1.getEventYieldDir() + "/zllPurity";
  std::string outputdir_of1 = cfg1.getEventYieldDir() + "/zllPurity_of";

  std::string outputdir2 = cfg2.getEventYieldDir() + "/zllPurity";
  std::string outputdir_of2 = cfg2.getEventYieldDir() + "/zllPurity_of";

  std::string outputdir3 = cfg3.getEventYieldDir() + "/zllPurity";
  std::string outputdir_of3 = cfg3.getEventYieldDir() + "/zllPurity_of";

  std::cout << "-> Using regions: " << regionsSet << std::endl;
  MT2DrawTools::setStyle();
 
  TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

  
  double intpart;
  double fracpart1 = modf(cfg1.lumi(), &intpart);
  std::string suffix1;
  if( fracpart1>0. )
    suffix1 = std::string( Form("_%.0fp%.0ffb", intpart, 10.*fracpart1 ) );
  else
    suffix1 = std::string( Form("_%.0ffb", intpart ) );
  
  
  //system(Form("mkdir -p %s", outputdir.c_str()));


  std::string ZllDir1 = cfg1.getEventYieldDir() + "/zllControlRegion";
  std::string ZllDir_of1 = cfg1.getEventYieldDir() + "/zllControlRegion";

  std::string ZllDir2 = cfg2.getEventYieldDir() + "/zllControlRegion";
  std::string ZllDir_of2 = cfg2.getEventYieldDir() + "/zllControlRegion";

  std::string ZllDir3 = cfg3.getEventYieldDir() + "/zllControlRegion";
  std::string ZllDir_of3 = cfg3.getEventYieldDir() + "/zllControlRegion";

  MT2Analysis<MT2EstimateTree>* Zll1 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees.root", ZllDir1.c_str() ), "DYJets");
  MT2Analysis<MT2EstimateTree>* Zll2 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees.root", ZllDir2.c_str() ), "DYJets");
  MT2Analysis<MT2EstimateTree>* Zll3 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees.root", ZllDir3.c_str() ), "DYJets");

  if( Zll1==0 || Zll2==0 || Zll3==0 ) {
    std::cout << "-> Please run zllControlRegion first. I need to get the yields from there." << std::endl;    std::cout << "-> Thank you for your cooperation." << std::endl;        exit(197);
  } 

  Zll1->setColor(kZinv);
  Zll2->setColor(kZinv);
  Zll3->setColor(kZinv);

  // MT2Analysis<MT2EstimateTree>* qcd = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees.root", ZllDir.c_str()  ), "QCD");
  // qcd->setColor(kQCD);

  MT2Analysis<MT2EstimateTree>* top1 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees.root", ZllDir1.c_str() ), "Top");
  MT2Analysis<MT2EstimateTree>* top2 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees.root", ZllDir2.c_str() ), "Top");
  MT2Analysis<MT2EstimateTree>* top3 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees.root", ZllDir3.c_str() ), "Top");

  top1->setColor(kTop);
  top2->setColor(kTop);
  top3->setColor(kTop);

  //MT2Analysis<MT2EstimateTree>* wjets = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees.root", ZllDir.c_str() ), "WJets");
  //wjets->setColor(kWJets);

  MT2Analysis<MT2EstimateTree>* data1 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/data.root", ZllDir1.c_str()) , "data");
  MT2Analysis<MT2EstimateTree>* data2 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/data.root", ZllDir2.c_str()) , "data");
  MT2Analysis<MT2EstimateTree>* data3 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/data.root", ZllDir3.c_str()) , "data");
 
  data1->setFullName("Data1");
  data2->setFullName("Data2");
  data3->setFullName("Data3");

  Zll1->setFullName("Z+jets1");
  Zll2->setFullName("Z+jets2");
  Zll3->setFullName("Z+jets3");

 
  std::vector<MT2Analysis<MT2EstimateTree>* > bgYields1; 
  std::vector<MT2Analysis<MT2EstimateTree>* > bgYields2; 
  std::vector<MT2Analysis<MT2EstimateTree>* > bgYields3; 

  bgYields1.push_back( Zll1 );
  bgYields2.push_back( Zll2 );
  bgYields3.push_back( Zll3 );
  
  bgYields1.push_back( top1 );
  bgYields2.push_back( top2 );
  bgYields3.push_back( top3 );


  //OPPOSITE FLAVOR TREES
  MT2Analysis<MT2EstimateTree>* Zll_of1 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees_of.root", ZllDir_of1.c_str() ), "DYJets");
  MT2Analysis<MT2EstimateTree>* Zll_of2 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees_of.root", ZllDir_of2.c_str() ), "DYJets");
  MT2Analysis<MT2EstimateTree>* Zll_of3 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees_of.root", ZllDir_of3.c_str() ), "DYJets");

  Zll_of1->setColor(kZinv);
  Zll_of2->setColor(kZinv);
  Zll_of3->setColor(kZinv);

  // MT2Analysis<MT2EstimateTree>* qcd_of = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees_of.root", ZllDir_of.c_str()  ), "QCD");
  // qcd_of->setColor(kQCD);

  MT2Analysis<MT2EstimateTree>* top_of1 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees_of.root", ZllDir_of1.c_str() ), "Top");
  MT2Analysis<MT2EstimateTree>* top_of2 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees_of.root", ZllDir_of2.c_str() ), "Top");
  MT2Analysis<MT2EstimateTree>* top_of3 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees_of.root", ZllDir_of3.c_str() ), "Top");

  top_of1->setColor(kTop);
  top_of2->setColor(kTop);
  top_of3->setColor(kTop);


  //MT2Analysis<MT2EstimateTree>* wjets_of = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/ZllPurityTrees_of.root", ZllDir_of.c_str() ), "WJets");
  // wjets_of->setColor(kWJets);

  MT2Analysis<MT2EstimateTree>* data_of1 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/data_of.root", ZllDir_of1.c_str() ) , "data_of");
  MT2Analysis<MT2EstimateTree>* data_of2 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/data_of.root", ZllDir_of2.c_str() ) , "data_of");
  MT2Analysis<MT2EstimateTree>* data_of3 = MT2Analysis<MT2EstimateTree>::readFromFile(Form("%s/data_of.root", ZllDir_of3.c_str() ) , "data_of");

  Zll_of1->setFullName("Z+jets");
  Zll_of2->setFullName("Z+jets");
  Zll_of3->setFullName("Z+jets");

  //  wjets_of->setFullName("W+jets");

  data_of1->setFullName("Data");
  data_of2->setFullName("Data");
  data_of3->setFullName("Data");

  std::vector<MT2Analysis<MT2EstimateTree>* > bgYields_of1; 
  std::vector<MT2Analysis<MT2EstimateTree>* > bgYields_of2; 
  std::vector<MT2Analysis<MT2EstimateTree>* > bgYields_of3; 

  bgYields_of1.push_back( top_of1 );
  bgYields_of2.push_back( top_of2 );
  bgYields_of3.push_back( top_of3 );

  bgYields_of1.push_back( Zll_of1 );
  bgYields_of2.push_back( Zll_of2 );
  bgYields_of3.push_back( Zll_of3 );

  std::string plotsDir = cfg1.getEventYieldDir() + "/combined_CRplots/zllControlRegion/SF";
  if( shapeNorm ) plotsDir += "_shape";

  std::string plotsDir_of = cfg1.getEventYieldDir() + "/combined_CRplots/zllControlRegion/OF";
  if( shapeNorm ) plotsDir_of += "_shape";


  MT2DrawTools dt(plotsDir, cfg1.lumi(), cfg2.lumi(), cfg3.lumi());
  dt.set_shapeNorm( shapeNorm );
  dt.set_data( data1, data2, data3 );
  dt.set_mc( &bgYields1, &bgYields2, &bgYields3 );
  dt.set_lumi( cfg1.lumi(), cfg2.lumi(), cfg3.lumi() );
  dt.set_year( cfg1.year() );
 
  MT2DrawTools dt_of(plotsDir_of, cfg1.lumi(), cfg2.lumi(), cfg3.lumi());
  dt_of.set_shapeNorm( shapeNorm );
  dt_of.set_data( data_of1, data_of2, data_of3 );
  dt_of.set_mc( &bgYields_of1, &bgYields_of2, &bgYields_of3 );
  dt_of.set_lumi( cfg1.lumi(), cfg2.lumi(), cfg3.lumi() );
 
  
// +++++++++++++++++++++++++
  // +++      monojet   +++
  // +++++++++++++++++++++++++

  float htMin=250, htMax=-1;
  std::string cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  // std::string selection = "(ht>250. && nJets==1 && met>250 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<20 )";
  std::string selection = "(ht>250. && nJets==1  && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<20 )";
     

  // std::string selection = "(ht>200. && nJets==1 && met>200 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<10 && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)) )";


  //savename, var, sel, nbins, xmin, xmax, axis label, units, cutslabel
  dt.drawRegionYields_fromTree("monojet_mt2", "mt2", selection, 40, 10., 810., "M_{T2}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_met", "met", selection, 30, 200., 800., "ME_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_ht", "ht", selection, 50, 200., 1200., "H_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_nJets", "nJets", selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_Z_pt", "Z_pt", selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_lep_pt0", "lep_pt0", selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_lep_pt1", "lep_pt1", selection, 25, 0., 500., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_lep_eta0", "lep_eta0", selection, 60, -3., 3., "Leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_lep_eta1", "lep_eta1", selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "=1j, #geq0b");
 
  
  dt.drawRegionYields_fromTree("monojet_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("monojet_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
 

  std::string selection_mass = "(ht>200. &&nJets==1 && met>200 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met  && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)))";
  dt.drawRegionYields_fromTree("monojet_mll", "Z_mass", selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel, "=1j, #geq0b");
  std::string selection_mass_el = "(ht>200. && Z_pt>180 &&nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==11  && lep_tightId0>0 && lep_tightId1>0)";
  dt.drawRegionYields_fromTree("monojet_mll_el", "Z_mass", selection_mass_el, 50, 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  std::string selection_mass_mu = "ht>200. && nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==13";
  dt.drawRegionYields_fromTree("monojet_mll_mu", "Z_mass", selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  
   
  //plots in 4 different ranges of Ht
  /*
  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. && ht<450 && nJets==1  && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<20 )";
  dt.drawRegionYields_fromTree("HT250to450_monojet_Z_pt", "Z_pt", selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  
  dt.drawRegionYields_fromTree("HT250to450_monojet_mt2", "mt2", selection, 45, 10., 460, "M_{T2}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_met", "met", selection, 30, 200., 800., "ME_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_ht", "ht", selection, 50, 200., 500., "H_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_nJets", "nJets", selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_lep_pt0", "lep_pt0", selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_lep_pt1", "lep_pt1", selection, 25, 0., 500., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_lep_eta0", "lep_eta0", selection, 30, -3., 3., "Leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_lep_eta1", "lep_eta1", selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_zll_met_pt", "zll_met_pt", selection, 50, 0, 500, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_zll_mht_pt", "zll_mht_pt", selection, 50, 0, 500, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_monojet_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
 

  selection_mass = "(ht>250 && ht<450 && nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met  && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)) )";
  dt.drawRegionYields_fromTree("HT250to450_monojet_mll", "Z_mass", selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_el = "(ht>250 && ht<450 && Z_pt>180 &&nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==11  && lep_tightId0>0 && lep_tightId1>0 )";
  dt.drawRegionYields_fromTree("HT250to450_monojet_mll_el", "Z_mass", selection_mass_el, 50, 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_mu = "ht>250 && ht<450 && nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==13";
  dt.drawRegionYields_fromTree("HT250to450_monojet_mll_mu", "Z_mass", selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  

  htMin=450, htMax=575;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>450 && ht<575";
  selection = "(ht>450. && ht<575 && nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<20 )";
  dt.drawRegionYields_fromTree("HT450to575_monojet_Z_pt", "Z_pt", selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_mt2", "mt2", selection, 20, 400, 600, "M_{T2}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_met", "met", selection, 30, 200., 800., "ME_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_ht", "ht", selection, 20, 400., 600., "H_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_nJets", "nJets", selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_lep_pt0", "lep_pt0", selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_lep_pt1", "lep_pt1", selection, 25, 0., 500., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_lep_eta0", "lep_eta0", selection, 30, -3., 3., "Leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_lep_eta1", "lep_eta1", selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_zll_met_pt", "zll_met_pt", selection, 20, 400, 600, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_zll_mht_pt", "zll_mht_pt", selection, 20, 400, 600, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_monojet_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  
  selection_mass = "(ht>450 && ht<575 &&nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met  && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)) )";
  dt.drawRegionYields_fromTree("HT450to575_monojet_mll", "Z_mass", selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_el = "(ht>450 && ht<575 && Z_pt>180 &&nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==11  && lep_tightId0>0 && lep_tightId1>0 )";
  dt.drawRegionYields_fromTree("HT450to575_monojet_mll_el", "Z_mass", selection_mass_el, 50, 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_mu = "ht>450 && ht<575 && nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==13";
  dt.drawRegionYields_fromTree("HT450to575_monojet_mll_mu", "Z_mass", selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "=1j, #geq0b");

  htMin=575, htMax=1000;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //plotsDir = cfg.getEventYieldDir() + "/zllControlRegion/test/ht575to1000/plotsDataMC";
  //if( shapeNorm ) plotsDir += "_shape";
  selection = "(ht>575. && ht<1000 && nJets==1  && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<20 )";
  dt.drawRegionYields_fromTree("HT575to1000_monojet_Z_pt", "Z_pt", selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_mt2", "mt2", selection, 45, 550, 1000, "M_{T2}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_met", "met", selection, 30, 200., 800., "ME_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_ht", "ht", selection, 45, 550., 1000., "H_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_nJets", "nJets", selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_lep_pt0", "lep_pt0", selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_lep_pt1", "lep_pt1", selection, 25, 0., 500., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_lep_eta0", "lep_eta0", selection, 30, -3., 3., "Leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_lep_eta1", "lep_eta1", selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_zll_met_pt", "zll_met_pt", selection, 45, 550, 1000, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_zll_mht_pt", "zll_mht_pt", selection, 45, 550, 1000, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_monojet_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");

  selection_mass = "(ht>575 && ht<1000 &&nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met  && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)) )";
  dt.drawRegionYields_fromTree("HT575to1000_monojet_mll", "Z_mass", selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_el = "(ht>575 && ht<1000 && Z_pt>180 &&nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==11  && lep_tightId0>0 && lep_tightId1>0 )";
  dt.drawRegionYields_fromTree("HT575to1000_monojet_mll_el", "Z_mass", selection_mass_el, 50, 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_mu = "ht>575 && ht<1000 && nJets==1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==13";
  dt.drawRegionYields_fromTree("HT575to1000_monojet_mll_mu", "Z_mass", selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "=1j, #geq0b");

  
  //htMin=1000, htMax=1500;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>1000 && ht<1500";
  //dt.drawRegionYields_fromTree( "HT1000to1500_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");

  //htMin=1500, htMax=-1;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>1500";
  //dt.drawRegionYields_fromTree( "HT1500toInf_Z_pt"   , "Z_pt"   , selection, 20, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  

  //and set them back to the general values
  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  */
   

  // +++++++++++++++++++++++++
  // +++     inclusive     +++
  // +++++++++++++++++++++++++
  // selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20)";
  selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<20 && lep_pt0>100 && lep_pt1>30)";

  dt.drawRegionYields_fromTree( "incl_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_ht"    , "ht"    , selection, 50, 250., 2250., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "incl_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
 
  dt.drawRegionYields_fromTree("incl_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("incl_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("incl_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("incl_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("incl_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("incl_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");

  //MLL 
  selection_mass = "(ht>250. && nJets>=1 && mt2>200. )";
  dt.drawRegionYields_fromTree( "incl_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250. && nJets>=1 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "incl_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250. && nJets>=1 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "incl_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  
  //plots in 4 different ranges of Ht
  /*
  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht<450";
  dt.drawRegionYields_fromTree( "HT250to450_incl_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_ht"    , "ht"    , selection, 30, 200., 500., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_incl_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_incl_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_incl_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_incl_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_incl_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_incl_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_incl_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>250 && ht<450 && nJets>=1 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT250to450_incl_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250 && ht<450 && nJets>=1 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT250to450_incl_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250 && ht<450 && nJets>=1 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT250to450_incl_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=450, htMax=575;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>450 && ht<575";
  dt.drawRegionYields_fromTree( "HT450to575_incl_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
 dt.drawRegionYields_fromTree( "HT450to575_incl_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_ht"    , "ht"    , selection, 20, 400., 600., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_incl_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_incl_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_incl_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_incl_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_incl_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_incl_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_incl_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>450 && ht<575 && nJets>=1 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT450to575_incl_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>450 && ht<575 && nJets>=1 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT450to575_incl_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>450 && ht<575 && nJets>=1 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT450to575_incl_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=575, htMax=1000;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>575 && ht<1000";
  dt.drawRegionYields_fromTree( "HT575to1000_incl_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
 dt.drawRegionYields_fromTree( "HT575to1000_incl_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_ht"    , "ht"    , selection, 70, 500, 1200., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_incl_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_incl_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_incl_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_incl_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_incl_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_incl_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_incl_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>575 && ht<1000 && nJets>=1 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT575to1000_incl_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>575 && ht<1000 && nJets>=1 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT575to1000_incl_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>575 && ht<1000 && nJets>=1 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT575to1000_incl_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");

  
  //htMin=1000, htMax=1500;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>1000 && ht<1500";
  //dt.drawRegionYields_fromTree( "HT1000to1500_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");

  //htMin=1500, htMax=-1;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>1500";
  //dt.drawRegionYields_fromTree( "HT1500toInf_Z_pt"   , "Z_pt"   , selection, 20, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  

  //and set them back to the general values
  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  */
  

  // +++++++++++++++++++++++++
  // +++     b vetoed     +++
  // +++++++++++++++++++++++++
  selection = "(ht>250. &&nJets>=1 && nBJets==0 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20)";

  dt.drawRegionYields_fromTree( "0b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_ht"    , "ht"    , selection, 50, 250., 2250., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_lep_pt0"   , "lep_pt0"   , selection, 40, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "0b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
 
  dt.drawRegionYields_fromTree("0b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("0b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("0b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("0b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("0b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("0b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");

  //MLL 
  selection_mass = "(ht>250. && nJets>=1 && nBJets==0 && mt2>200. )";
  dt.drawRegionYields_fromTree( "0b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250. && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "0b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250. && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "0b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");

  //plots in 4 different ranges of Ht
  /*
  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets==0 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht<450";
  dt.drawRegionYields_fromTree( "HT250to450_0b_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_ht"    , "ht"    , selection, 30, 200., 500, "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_0b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_0b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_0b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_0b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_0b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_0b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_0b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  selection_mass = "(ht>250 && ht<450 && nJets>=1 && nBJets==0 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT250to450_0b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250 && ht<450 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT250to450_0b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250 && ht<450 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT250to450_0b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=450, htMax=575;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets==0 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>450 && ht<575";
  dt.drawRegionYields_fromTree( "HT450to575_0b_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
 dt.drawRegionYields_fromTree( "HT450to575_0b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_ht"    , "ht"    , selection, 20, 400., 600., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_0b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_0b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_0b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_0b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_0b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_0b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_0b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
 
  selection_mass = "(ht>450 && ht<575 && nJets>=1 && nBJets==0 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT450to575_0b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>450 && ht<575 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT450to575_0b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>450 && ht<575 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT450to575_0b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=575, htMax=1000;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets==0 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>575 && ht<1000";
  dt.drawRegionYields_fromTree( "HT575to1000_0b_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
 dt.drawRegionYields_fromTree( "HT575to1000_0b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_ht"    , "ht"    , selection, 70, 500., 1200., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_0b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_0b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_0b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_0b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_0b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_0b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_0b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
 
  selection_mass = "(ht>575 && ht<1000 && nJets>=1 && nBJets==0 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT575to1000_0b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>575 && ht<1000 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT575to1000_0b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>575 && ht<1000 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT575to1000_0b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");

  
  //htMin=1000, htMax=1500;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>1000 && ht<1500";
  //dt.drawRegionYields_fromTree( "HT1000to1500_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");

  //htMin=1500, htMax=-1;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>1500";
  //dt.drawRegionYields_fromTree( "HT1500toInf_Z_pt"   , "Z_pt"   , selection, 20, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  

  //and set them back to the general values
  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  */

// +++++++++++++++++++++++++
  // +++     b enriched     +++
  // +++++++++++++++++++++++++
  selection = "(ht>250. &&nJets>=1 && nBJets>1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20)";

  dt.drawRegionYields_fromTree( "2b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_ht"    , "ht"    , selection, 50, 250., 2250., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "2b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
 
  dt.drawRegionYields_fromTree("2b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("2b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("2b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("2b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("2b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("2b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");

  //MLL 
  selection_mass = "(ht>250 && nJets>=1 && nBJets>1 && mt2>200. )";
  dt.drawRegionYields_fromTree( "2b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "2b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "2b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");

  //plots in 4 different ranges of Ht
  /*
  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets>1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht<450";
  dt.drawRegionYields_fromTree( "HT250to450_2b_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_ht"    , "ht"    , selection, 30, 200., 500., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT250to450_2b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_2b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_2b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_2b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_2b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_2b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT250to450_2b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>250 && ht<450 && nJets>=1 && nBJets>1 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT250to450_2b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250 && ht<450 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT250to450_2b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250 && ht<450 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT250to450_2b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=450, htMax=575;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets>1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>450 && ht<575";
  dt.drawRegionYields_fromTree( "HT450to575_2b_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_ht"    , "ht"    , selection, 20, 400., 600., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT450to575_2b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_2b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_2b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_2b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_2b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_2b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT450to575_2b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>450 && ht<575 && nJets>=1 && nBJets>1 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT450to575_2b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>450 && ht<575 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT450to575_2b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>450 && ht<575 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT450to575_2b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=575, htMax=1000;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets>1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>575 && ht<1000";
  dt.drawRegionYields_fromTree( "HT575to1000_2b_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_ht"    , "ht"    , selection, 70, 500., 1200., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree( "HT575to1000_2b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_2b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_2b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_2b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_2b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_2b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt.drawRegionYields_fromTree("HT575to1000_2b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>575 && ht<1000 && nJets>=1 && nBJets>1 && mt2>200. )";
  dt.drawRegionYields_fromTree( "HT575to1000_2b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>575 && ht<1000 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==11)";
  dt.drawRegionYields_fromTree( "HT575to1000_2b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>575 && ht<1000 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==13 )";
  dt.drawRegionYields_fromTree( "HT575to1000_2b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");

  
  //htMin=1000, htMax=1500;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>1000 && ht<1500";
  //dt.drawRegionYields_fromTree( "HT1000to1500_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");

  // htMin=1500, htMax=-1;
  //cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  //selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>1500";
  //dt.drawRegionYields_fromTree( "HT1500toInf_Z_pt"   , "Z_pt"   , selection, 20, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  

  //and set them back to the general values
  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");

  */




  

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                            ///////////////// opposite flavour ///////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  

  // +++++++++++++++++++++++++
  // +++     inclusive     +++
  // +++++++++++++++++++++++++
  selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20)";
  //  selection = "(ht>200. &&nJets>=1 && met>200. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<20)";

  dt_of.drawRegionYields_fromTree( "incl_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_ht"    , "ht"    , selection, 50, 250., 2250., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "incl_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
 
  dt_of.drawRegionYields_fromTree("incl_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("incl_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("incl_zll_met_pt", "zll_met_pt", selection, 200, 0, 1400, "E_{T}^{miss} p_{T}", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("incl_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("incl_zll_mht_pt", "zll_mht_pt", selection, 200, 0, 1400, "H_{T}^{miss} p_{T}", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("incl_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");

  //MLL 
  selection_mass = "(ht>250. && nJets>=1 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "incl_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250. && nJets>=1 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "incl_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250. && nJets>=1 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "incl_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");

  //plots in 4 different ranges of Ht
  /*
  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht<450";
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_ht"    , "ht"    , selection, 30, 200., 500., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_incl_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_incl_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_incl_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_incl_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_incl_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_incl_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>250 && ht<450 && nJets>=1 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250 && ht<450 && nJets>=1 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250 && ht<450 && nJets>=1 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT250to450_incl_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=450, htMax=575;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>450 && ht<575";
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_ht"    , "ht"    , selection, 20, 400., 600., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_incl_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_incl_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_incl_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_incl_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_incl_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_incl_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>450 && ht<575 && nJets>=1 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>450 && ht<575 && nJets>=1 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>450 && ht<575 && nJets>=1 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT450to575_incl_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=575, htMax=1000;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>575 && ht<1000";
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_ht"    , "ht"    , selection, 70, 500., 1200., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_incl_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_incl_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_incl_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_incl_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_incl_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_incl_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>575 && ht<1000 && nJets>=1 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>575 && ht<1000 && nJets>=1 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>575 && ht<1000 && nJets>=1 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT575to1000_incl_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b"); 

  //and set them back to the general values
  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  */

// +++++++++++++++++++++++++
  // +++     b vetoed     +++
  // +++++++++++++++++++++++++
  selection = "(ht>250. &&nJets>=1 && nBJets==0 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20)";

  dt_of.drawRegionYields_fromTree( "0b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_ht"    , "ht"    , selection, 50, 250., 2250., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "0b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
 
  dt_of.drawRegionYields_fromTree("0b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("0b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("0b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("0b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("0b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("0b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");

  //MLL 
  selection_mass = "(ht>250. && nJets>=1 && nBJets==0 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "0b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250. && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "0b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250. && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "0b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");

  //plots in 4 different ranges of Ht
  /*
  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets==0 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht<450";
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_ht"    , "ht"    , selection, 30, 200., 500., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_0b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_0b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_0b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_0b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_0b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_0b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>250 && ht<450 && nJets>=1 && nBJets==0 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250 && ht<450 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250 && ht<450 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT250to450_0b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=450, htMax=575;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets==0 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>450 && ht<575";
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_ht"    , "ht"    , selection, 20, 400., 600., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_0b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_0b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_0b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_0b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_0b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_0b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
 
  selection_mass = "(ht>450 && ht<575 && nJets>=1 && nBJets==0 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>450 && ht<575 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>450 && ht<575 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT450to575_0b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=575, htMax=1000;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets==0 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>575 && ht<1000";
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_ht"    , "ht"    , selection, 70, 500., 1200., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_0b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_0b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_0b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_0b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_0b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_0b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>575 && ht<1000 && nJets>=1 && nBJets==0 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>575 && ht<1000 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>575 && ht<1000 && nJets>=1 && nBJets==0 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT575to1000_0b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");


  //and set them back to the general values
  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  */
// +++++++++++++++++++++++++
  // +++     b enriched     +++
  // +++++++++++++++++++++++++
  selection = "(ht>250. &&nJets>=1 && nBJets>1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20)";

  dt_of.drawRegionYields_fromTree( "2b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_ht"    , "ht"    , selection, 50, 250., 2250., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "2b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
 
  dt_of.drawRegionYields_fromTree("2b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("2b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("2b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("2b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("2b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("2b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");

  //MLL 
  selection_mass = "(ht>250 && nJets>=1 && nBJets>1 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "2b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "2b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "2b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");

  //plots in 4 different ranges of Ht
  /*
  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets>1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht<450";
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_ht"    , "ht"    , selection, 30, 200., 500., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_2b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_2b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_2b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_2b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_2b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_2b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  
  selection_mass = "(ht>250 && ht<450 && nJets>=1 && nBJets>1 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>250 && ht<450 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>250 && ht<450 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT250to450_2b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=450, htMax=575;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets>1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>450 && ht<575";
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_Z_pt"   , "Z_pt"   , selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_ht"    , "ht"    , selection, 20, 400., 600., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_2b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_2b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_2b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_2b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_2b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_2b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
 
  selection_mass = "(ht>450 && ht<575 && nJets>=1 && nBJets>1 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>450 && ht<575 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>450 && ht<575 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT450to575_2b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
 

  htMin=575, htMax=1000;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets>=1 && nBJets>1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>575 && ht<1000";
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_Z_pt"   , "Z_pt"   , selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_mt2"   , "mt2"   , selection, 40, 200., 1000., "M_{T2}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_met"   , "met"   , selection, 40, 200, 1000, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_raw_met"   , "raw_met"   , selection, 40, 0, 400, "ME_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_ht"    , "ht"    , selection, 70, 500, 1200., "H_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_nJets" , "nJets" , selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_lep_pt0"   , "lep_pt0"   , selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_lep_pt1"   , "lep_pt1"   , selection, 40, 0., 400., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_lep_eta0"   , "lep_eta0"   , selection,30 , -3., 3., "Leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_lep_eta1"   , "lep_eta1"   , selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_2b_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel,  "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_2b_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_2b_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_2b_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_2b_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_2b_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "#geq1j, #geq0b");
 
  selection_mass = "(ht>575 && ht<1000 && nJets>=1 && nBJets>1 && mt2>200. )";
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_mll"   , "Z_mass"   , selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel ,"#geq1j, #geq0b");
  selection_mass_el = "(ht>575 && ht<1000 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==11)";
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_mll_el"   , "Z_mass"   , selection_mass_el, 50 , 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  selection_mass_mu = "(ht>575 && ht<1000 && nJets>=1 && nBJets>1 && mt2>200. && Z_lepId==13 )";
  dt_of.drawRegionYields_fromTree( "HT575to1000_2b_mll_mu"   , "Z_mass"   , selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "#geq1j, #geq0b");
  

  //and set them back to the general values
  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  */
 // +++++++++++++++++++++++++
  // +++      monojet   +++
  // +++++++++++++++++++++++++

  selection = "(ht>250. && nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && abs(Z_mass-91.19)<20 )";

  dt_of.drawRegionYields_fromTree("monojet_mt2", "mt2", selection, 40, 10., 810., "M_{T2}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_met", "met", selection, 30, 200., 800., "ME_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_ht", "ht", selection, 50, 200., 1200., "H_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_nJets", "nJets", selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_Z_pt", "Z_pt", selection, 50, 10., 1010., "Z p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_lep_pt0", "lep_pt0", selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_lep_pt1", "lep_pt1", selection, 25, 0., 500., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_lep_eta0", "lep_eta0", selection, 30, -3., 3., "Leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_lep_eta1", "lep_eta1", selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "=1j, #geq0b");
 
  
  dt_of.drawRegionYields_fromTree("monojet_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_zll_met_pt", "zll_met_pt", selection, 140, 0, 1400, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_zll_mht_pt", "zll_mht_pt", selection, 140, 0, 1400, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("monojet_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
 

  selection_mass = "(ht>250. &&nJets==1. && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met  && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)))";
  dt_of.drawRegionYields_fromTree("monojet_mll", "Z_mass", selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_el = "(ht>250. && Z_pt>180 &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==11  && lep_tightId0>0 && lep_tightId1>0)";
  dt_of.drawRegionYields_fromTree("monojet_mll_el", "Z_mass", selection_mass_el, 50, 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_mu = "ht>250. && nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==13";
  dt_of.drawRegionYields_fromTree("monojet_mll_mu", "Z_mass", selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "=1j, #geq0b");

  //plots in 4 different ranges of Ht
  /*
  htMin=250, htMax=450;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht<450";
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_Z_pt", "Z_pt", selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_mt2", "mt2", selection, 30, 200., 500, "M_{T2}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_met", "met", selection, 30, 200., 800., "ME_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_ht", "ht", selection, 30, 200., 500, "H_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_nJets", "nJets", selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_lep_pt0", "lep_pt0", selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_lep_pt1", "lep_pt1", selection, 25, 0., 500., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_lep_eta0", "lep_eta0", selection, 30, -3., 3., "Leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_lep_eta1", "lep_eta1", selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_zll_met_pt", "zll_met_pt", selection, 30, 200, 500, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_zll_mht_pt", "zll_mht_pt", selection, 30, 200, 500, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
 
  selection_mass = "(ht>250 && ht<450 &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met  && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)) )";
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_mll", "Z_mass", selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_el = "(ht>250 && ht<450 && Z_pt>180 &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==11  && lep_tightId0>0 && lep_tightId1>0 )";
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_mll_el", "Z_mass", selection_mass_el, 50, 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_mu = "ht>250 && ht<450 && nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==13";
  dt_of.drawRegionYields_fromTree("HT250to450_monojet_mll_mu", "Z_mass", selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
 

  htMin=450, htMax=575;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  selection = "(ht>250. &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>450 && ht<575";
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_Z_pt", "Z_pt", selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_mt2", "mt2", selection, 20, 400., 600, "M_{T2}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_met", "met", selection, 30, 200., 800., "ME_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_ht", "ht", selection, 20, 400., 600., "H_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_nJets", "nJets", selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_lep_pt0", "lep_pt0", selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_lep_pt1", "lep_pt1", selection, 25, 0., 500., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_lep_eta0", "lep_eta0", selection, 30, -3., 3., "Leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_lep_eta1", "lep_eta1", selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_zll_met_pt", "zll_met_pt", selection, 20, 400, 600, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_zll_mht_pt", "zll_mht_pt", selection, 20, 400, 600, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  
  selection_mass = "(ht>450 && ht<575 &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met  && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)) )";
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_mll", "Z_mass", selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_el = "(ht>450 && ht<575 && Z_pt>180 &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==11  && lep_tightId0>0 && lep_tightId1>0 )";
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_mll_el", "Z_mass", selection_mass_el, 50, 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_mu = "ht>450 && ht<575 && nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==13";
  dt_of.drawRegionYields_fromTree("HT450to575_monojet_mll_mu", "Z_mass", selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "=1j, #geq0b");

  htMin=575, htMax=1000;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  plotsDir = cfg.getEventYieldDir() + "/zllControlRegion/test/ht575to1000/plotsDataMC";
  if( shapeNorm ) plotsDir += "_shape";
  selection = "(ht>250. &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && fabs(Z_mass-91.19)<=20) && ht>575 && ht<1000";
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_Z_pt", "Z_pt", selection, 40, 10., 810., "Z p_{T}", "GeV", cutsLabel, "#geq1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_mt2", "mt2", selection, 70, 500, 1200., "M_{T2}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_met", "met", selection, 30, 200., 800., "ME_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_ht", "ht", selection, 70, 500., 1200., "H_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_nJets", "nJets", selection, 13, 0.5, 13.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_nBJets", "nBJets", selection, 7, -0.5, 6.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_lep_pt0", "lep_pt0", selection, 35, 0., 700., "Leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_lep_pt1", "lep_pt1", selection, 25, 0., 500., "Sub-leading Lepton p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_lep_eta0", "lep_eta0", selection, 30, -3., 3., "Leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_lep_eta1", "lep_eta1", selection, 30, -3., 3., "Sub-leading Lepton #eta", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_Z_lepId", "Z_lepId", selection, 5, 9.5, 14.5, "Lepton Id", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_lep_phi0", "lep_phi0", selection, 40, -4, 4, "Leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_lep_phi1", "lep_phi1", selection, 40, -4, 4, "Sub-leading Lepton #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_zll_met_pt", "zll_met_pt", selection, 70, 500, 1200, "E_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_zll_met_phi", "zll_met_phi", selection, 40, -4, 4, "E_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_zll_mht_pt", "zll_mht_pt", selection, 70, 500, 1200, "H_{T}^{miss} p_{T}", "GeV", cutsLabel, "=1j, #geq0b");
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_zll_mht_phi", "zll_mht_phi", selection, 40, -4, 4, "H_{T}^{miss} #Phi", "", cutsLabel, "=1j, #geq0b");
 
  selection_mass = "(ht>575 && ht<1000 &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met  && (Z_lepId==13 || (Z_lepId==11 && lep_tightId0>0 && lep_tightId1>0)) )";
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_mll", "Z_mass", selection_mass, 50, 50., 150., "M_{ll}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_el = "(ht>575 && ht<1000 && Z_pt>180 &&nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==11  && lep_tightId0>0 && lep_tightId1>0 )";
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_mll_el", "Z_mass", selection_mass_el, 50, 50., 150., "M_{e^{+}e^{-}}", "GeV", cutsLabel, "=1j, #geq0b");
  selection_mass_mu = "ht>575 && ht<1000 && nJets==1 && mt2>200. && deltaPhiMin>0.3 && diffMetMht<0.5*met && Z_lepId==13";
  dt_of.drawRegionYields_fromTree("HT575to1000_monojet_mll_mu", "Z_mass", selection_mass_mu, 50, 50., 150., "M_{#mu^{+}#mu^{-}}", "GeV", cutsLabel, "=1j, #geq0b");

  //and set them back to the general values
  htMin=250, htMax=-1;
  cutsLabel = getCutLabel(htMin, htMax, "H_{T}", "GeV");
  
  */
  return 0;

}





std::string getCutLabel( float theMin, float theMax, const std::string& name, const std::string& units ) {

  std::string cutLabel;
  if( theMax>theMin ) cutLabel = std::string(Form("%.0f < %s < %.0f %s", theMin, name.c_str(), theMax, units.c_str()) );
  else                cutLabel = std::string(Form("%s > %.0f %s", name.c_str(), theMin, units.c_str()) );

  return cutLabel;

}



void drawYields( MT2Config cfg, MT2Analysis<MT2EstimateTree>* data, std::vector<MT2Analysis<MT2EstimateTree>* >  bgYields, const std::string& saveName, const std::string& varName, const std::string& selection, int nBins, float xMin, float xMax, std::string axisName, const std::string& units ) {


  float binWidth = (xMax-xMin)/nBins;
  if( axisName=="" ) axisName = varName;

  std::vector<int> colors;
  if( bgYields.size()==3 ) { // estimates
    colors.push_back(402); 
    colors.push_back(430); 
    colors.push_back(418); 
  } else { // mc
    colors.push_back(430); // other=zll
    colors.push_back(401); // qcd
    colors.push_back(417); // w+jets
    //    colors.push_back(419); // z+jets
    colors.push_back(855); // top
    //colors.push_back(); // other
  }

  std::string fullPathPlots = cfg.getEventYieldDir() + "/plotsDataMC";
  if( shapeNorm ) fullPathPlots += "_shape";
  system( Form("mkdir -p %s", fullPathPlots.c_str()) );

  TH1::AddDirectory(kTRUE); // stupid ROOT memory allocation needs this

  std::set<MT2Region> MT2Regions = data->getRegions();
  
  for( std::set<MT2Region>::iterator iMT2 = MT2Regions.begin(); iMT2!=MT2Regions.end(); ++iMT2 ) {
  
    MT2Region thisRegion( (*iMT2) );

    TTree* tree_data = data->get(thisRegion)->tree;
    TH1D* h1_data = new TH1D("h1_data", "", nBins, xMin, xMax );
    tree_data->Project( "h1_data", varName.c_str(), selection.c_str() );

    TGraphAsymmErrors* gr_data = MT2DrawTools::getPoissonGraph(h1_data);
    gr_data->SetMarkerStyle(20);
    gr_data->SetMarkerSize(1.2);


    std::vector< TH1D* > histos_mc;
    for( unsigned i=0; i<bgYields.size(); ++i ) { 
      TTree* tree_mc = (bgYields[i]->get(thisRegion)->tree);
      std::string thisName = "h1_" + bgYields[i]->getName();
      TH1D* h1_mc = new TH1D( thisName.c_str(), "", nBins, xMin, xMax );
      h1_mc->Sumw2();
      if( selection!="" )
	//tree_mc->Project( thisName.c_str(), varName.c_str(), Form("%s/puWeight", selection.c_str()) );
	tree_mc->Project( thisName.c_str(), varName.c_str(), Form("%s", selection.c_str()) );
      else
        tree_mc->Project( thisName.c_str(), varName.c_str(), "" );
      histos_mc.push_back(h1_mc);
    }

    TH1::AddDirectory(kFALSE); // stupid ROOT memory allocation needs this

    TH1D* mc_sum;
    for( unsigned i=0; i<histos_mc.size(); ++i ) { 
      if( i==0 ) {
        mc_sum = new TH1D( *histos_mc[i] );
        mc_sum->SetName("mc_sum");
      } else {
        mc_sum->Add( histos_mc[i] );
      }
    }

    std::cout << "Integrals: " << h1_data->Integral(0, nBins+1) << "\t" << mc_sum->Integral(0, nBins+1) << std::endl;
    float scaleFactor = h1_data->Integral(0, nBins+1)/mc_sum->Integral(0, nBins+1);   
    if( shapeNorm ) 
      std::cout << "SF: " << scaleFactor << std::endl;

    TH1D* histo_mc;
    THStack bgStack("bgStack", "");
    for( unsigned i=0; i<histos_mc.size(); ++i ) { 
      int index = bgYields.size() - i - 1;
      histos_mc[index]->SetFillColor( colors[index] );
      histos_mc[index]->SetLineColor( kBlack );
      if( shapeNorm ) {
        histos_mc[index]->Scale( scaleFactor );
      }
      else
	histos_mc[index]->Scale( 145.0/106.5 );

      if(i==0) histo_mc = (TH1D*) histos_mc[index]->Clone("histo_mc");
      else histo_mc->Add(histos_mc[index]);
      bgStack.Add(histos_mc[index]);
    }


    TCanvas* c1 = new TCanvas("c1", "", 600, 600);
    c1->cd();
    TPad *pad1 = MT2DrawTools::getCanvasMainPad();
    TPad *pad2 = MT2DrawTools::getCanvasRatioPad();
        
    TCanvas* c1_log = new TCanvas("c1_log", "", 600, 600);
    c1_log->cd();
    TPad *pad1_log = MT2DrawTools::getCanvasMainPad( true );
    TPad *pad2_log = MT2DrawTools::getCanvasRatioPad( true );
 
    float yMaxScale = 1.1;
    float yMax1 = h1_data->GetMaximum()*yMaxScale;
    float yMax2 = yMaxScale*(h1_data->GetMaximum() + sqrt(h1_data->GetMaximum()));
    float yMax3 = yMaxScale*(bgStack.GetMaximum());
    float yMax = (yMax1>yMax2) ? yMax1 : yMax2;
    if( yMax3 > yMax ) yMax = yMax3;
    if( h1_data->GetNbinsX()<2 ) yMax *=3.;

    std::string xAxisTitle;
    if( units!="" ) 
      xAxisTitle = (std::string)(Form("%s [%s]", axisName.c_str(), units.c_str()) );
    else
      xAxisTitle = (std::string)(Form("%s", axisName.c_str()) );

    std::string binWidthText;
    if( binWidth>=1. )         binWidthText = (std::string)Form("%.0f", binWidth);
    else if( binWidth>=0.1 )   binWidthText = (std::string)Form("%.1f", binWidth);
    else if( binWidth>=0.01 )  binWidthText = (std::string)Form("%.2f", binWidth);
    else if( binWidth>=0.001 ) binWidthText = (std::string)Form("%.3f", binWidth);
    else                       binWidthText = (std::string)Form("%.4f", binWidth);

    std::string yAxisTitle;
    if( units!="" ) 
      yAxisTitle = (std::string)(Form("Events / (%s %s)", binWidthText.c_str(), units.c_str()));
    else
      yAxisTitle = (std::string)(Form("Events / (%s)", binWidthText.c_str()));

    TH2D* h2_axes = new TH2D("axes", "", 10, xMin, xMax, 10, 0., yMax );
    h2_axes->SetXTitle(xAxisTitle.c_str());
    h2_axes->SetYTitle(yAxisTitle.c_str());

    c1->cd();
    pad1->Draw();
    pad1->cd();
    h2_axes->Draw();
   
    TH2D* h2_axes_log = new TH2D("axes_log", "", 10, xMin, xMax, 10, 0.1, yMax*2.0 );
    h2_axes_log->SetXTitle(xAxisTitle.c_str());
    h2_axes_log->SetYTitle(yAxisTitle.c_str());

    c1_log->cd();
    pad1_log->Draw();
    pad1_log->cd();
    h2_axes_log->Draw();
   
    std::vector<std::string> niceNames = thisRegion.getNiceNames();

    for( unsigned i=0; i<niceNames.size(); ++i ) {
      float yMax = 0.9-(float)i*0.05;
      float yMin = yMax - 0.05;
      TPaveText* regionText = new TPaveText( 0.18, yMin, 0.55, yMax, "brNDC" );
      regionText->SetTextSize(0.04);
      regionText->SetTextFont(42);
      regionText->SetFillColor(0);
      regionText->SetTextAlign(11);
      regionText->AddText( niceNames[i].c_str() );

      pad1->cd();
      // regionText->Draw("same");
      pad1_log->cd();
      //  regionText->Draw("same");
    }
    
    if( shapeNorm ) {
      TPaveText* normText = new TPaveText( 0.45, 0.8, 0.68, 0.9, "brNDC" );
      normText->SetFillColor(0);
      normText->SetTextSize(0.035);
      normText->AddText( "#splitline{Shape}{Norm.}" );
      pad1->cd();
      //normText->Draw("same");
      pad1_log->cd();
      //normText->Draw("same");
    }

    TLegend* legend = new TLegend( 0.7, 0.9-(bgYields.size()+1)*0.06, 0.93, 0.9 );
    legend->SetTextSize(0.04);
    legend->SetTextFont(42);
    legend->SetFillColor(0);
    legend->AddEntry( gr_data, "Data", "P" );
    for( unsigned i=0; i<histos_mc.size(); ++i ) {  
      legend->AddEntry( histos_mc[i], bgYields[i]->getFullName().c_str(), "F" );
    }


    TPaveText* labelTop = MT2DrawTools::getLabelTop(cfg.lumi());
    
    TPaveText* ratioText = new TPaveText( 0.133, -0.051, 0.4, 0.1 , "brNDC" );
    ratioText->SetTextSize(0.04);
    ratioText->SetTextFont(40);
    ratioText->SetTextColor(2);
    ratioText->SetFillColor(0);
    ratioText->SetTextAlign(11);
    ratioText->AddText( Form("Data/MC = %.2f", scaleFactor) );
    //  ratioText->AddText( Form("Data/MC = %.2f +/- %.2f", scaleFactor, error_datamc) );
     

    TLine* line = new TLine(xMin, 1.0, xMax, 1.0);
    line->SetLineColor(1);
    
    TLine* lineSF = new TLine(xMin, scaleFactor, xMax, scaleFactor);
    lineSF->SetLineColor(2);

    float yMinR=0.0;
    float yMaxR=2.0;

    TH2D* h2_axes_ratio = MT2DrawTools::getRatioAxes( xMin, xMax, yMinR, yMaxR );
    TGraphAsymmErrors* g_ratio = MT2DrawTools::getRatioGraph(h1_data, histo_mc);
 
    TLine* lineCentral = new TLine(xMin, 1.0, xMax, 1.0);
    lineCentral->SetLineColor(1);
    TGraphErrors* systBand = MT2DrawTools::getSystBand(xMin, xMax, lumiErr);
   
    //    TH1D* mcBand = MT2DrawTools::getMCBandHisto( histo_mc, lumiErr );
    TF1* fSF = MT2DrawTools::getSFFit(g_ratio, xMin, xMax);
    TGraphErrors* SFFitBand = MT2DrawTools::getSFFitBand(fSF, xMin, xMax);
    TPaveText* fitText = MT2DrawTools::getFitText( fSF );


    c1->cd();
    pad1->cd();
    legend->Draw("same");
    bgStack.Draw("histo same");
    gr_data->Draw("p same");
    labelTop->Draw("same");
    if( !shapeNorm )
      fitText->Draw("same");
    // ratioText->Draw("same");
  
    gPad->RedrawAxis();

    c1_log->cd();
    pad1_log->cd();
    legend->Draw("same");
    bgStack.Draw("histo same");
    gr_data->Draw("p same");
    labelTop->Draw("same");
    if( !shapeNorm )
     fitText->Draw("same");
    //  ratioText->Draw("same");

    gPad->RedrawAxis();

   /*
    TLine* line = new TLine(xMin, 1.0, xMax, 1.0);
    line->SetLineColor(1);
    TLine* lineSF = MT2DrawTools::getSFLine(integral_data, integral_mc, xMin, xMax);
    TGraphErrors* SFband = MT2DrawTools::getSFBand(integral_data, error_data, integral_mc, error_mc, xMin, xMax);
    */

    c1->cd();
    //   TPad* pad2 = MT2DrawTools::getCanvasRatioPad();
    pad2->Draw();
    pad2->cd();

    h2_axes_ratio->Draw("");
 
    /*  line->Draw("same");
    SFband->Draw("3,same");
    lineSF->Draw("same");
    */
    lineCentral->Draw("same");
    if( !shapeNorm ){

      systBand->Draw("3,same");
      lineCentral->Draw("same");

      SFFitBand->Draw("3,same");
      fSF->Draw("same");
    }

    g_ratio->Draw("PE,same");    
    gPad->RedrawAxis();


    c1_log->cd();
    // TPad* pad2_log = MT2DrawTools::getCanvasRatioPad( true );
    pad2_log->Draw();
    pad2_log->cd();

    h2_axes_ratio->Draw(""); 
    
    lineCentral->Draw("same");
    if( !shapeNorm ){

      systBand->Draw("3,same");
      lineCentral->Draw("same");

      SFFitBand->Draw("3,same");
      fSF->Draw("same");
    }
    /*
    line->Draw("same");
    SFband->Draw("3,same");
    lineSF->Draw("same"); */
    g_ratio->Draw("PE,same");
    gPad->RedrawAxis();


    c1->SaveAs( Form("%s/%s_%s.eps", fullPathPlots.c_str(), saveName.c_str(), thisRegion.getName().c_str()) );
    c1->SaveAs( Form("%s/%s_%s.png", fullPathPlots.c_str(), saveName.c_str(), thisRegion.getName().c_str()) );
    c1->SaveAs( Form("%s/%s_%s.pdf", fullPathPlots.c_str(), saveName.c_str(), thisRegion.getName().c_str()) );

    c1_log->SaveAs( Form("%s/%s_%s_log.eps", fullPathPlots.c_str(), saveName.c_str(), thisRegion.getName().c_str()) );
    c1_log->SaveAs( Form("%s/%s_%s_log.png", fullPathPlots.c_str(), saveName.c_str(), thisRegion.getName().c_str()) );
    c1_log->SaveAs( Form("%s/%s_%s_log.pdf", fullPathPlots.c_str(), saveName.c_str(), thisRegion.getName().c_str()) );

    delete c1;
    delete h2_axes;

    delete c1_log;
    delete h2_axes_log;

    delete h2_axes_ratio;

    delete h1_data;
  
    for( unsigned i=0; i<histos_mc.size(); ++i )
      delete histos_mc[i];

  }// for MT2 regions

}
