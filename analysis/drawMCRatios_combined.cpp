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
#include "../interface/MT2DrawTools_ForRatios.h"

using namespace std;

std::string getCutLabel( float theMin, float theMax, const std::string& name, const std::string& units="" );
std::string getJetCutLabel( int jMin, int jMax, int bMin, int bMax);



int main( int argc, char* argv[] ) {


  if( argc<4 ) {
    std::cout << "USAGE: ./drawLLeptonMCRatios [configFileName16] [configFileName17] [configFileName18] [lumi/shape]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  MT2DrawTools_ForRatios::setStyle();

  std::string configFileName16(argv[1]);
  MT2Config cfg16(configFileName16);
  std::string configFileName17(argv[2]);
  MT2Config cfg17(configFileName17);
  std::string configFileName18(argv[3]);
  MT2Config cfg18(configFileName18);

  bool shapeNorm = false;
  if( argc>4 ) {
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

  std::string cutsLabel = getCutLabel(250, -1, "H_{T}", "GeV");
  int jetMin=2, jetMax=-1;
  int bMin=0, bMax=-1;
  std::string jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  // SR MC 16
  MT2Analysis<MT2EstimateTree>* mcSR_top16 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg16.getEventYieldDir() + "/analyses.root", "Top");
  MT2Analysis<MT2EstimateTree>* mcSR_wjets16 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg16.getEventYieldDir() + "/analyses.root", "WJets");
  MT2Analysis<MT2EstimateTree>* mcSR_zjets16 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg16.getEventYieldDir() + "/analyses.root", "ZJets");

  TFile* tmp16 = new TFile("tmp16.root", "RECREATE");

  MT2Analysis<MT2EstimateTree>* mcSR_llep16 = new MT2Analysis<MT2EstimateTree>( *(mcSR_top16) );
  mcSR_llep16->setName("mcSR_llep16");
  (*mcSR_llep16) += (*(mcSR_top16));
  (*mcSR_llep16) += (*(mcSR_wjets16));

  // SR MC 17
  MT2Analysis<MT2EstimateTree>* mcSR_top17 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg17.getEventYieldDir() + "/analyses.root", "Top");
  MT2Analysis<MT2EstimateTree>* mcSR_wjets17 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg17.getEventYieldDir() + "/analyses.root", "WJets");
  MT2Analysis<MT2EstimateTree>* mcSR_zjets17 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg17.getEventYieldDir() + "/analyses.root", "ZJets");

  TFile* tmp17 = new TFile("tmp17.root", "RECREATE");

  MT2Analysis<MT2EstimateTree>* mcSR_llep17 = new MT2Analysis<MT2EstimateTree>( *(mcSR_top17) );
  mcSR_llep17->setName("mcSR_llep17");
  (*mcSR_llep17) += (*(mcSR_top17));
  (*mcSR_llep17) += (*(mcSR_wjets17));

  // SR MC 18
  MT2Analysis<MT2EstimateTree>* mcSR_top18 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg18.getEventYieldDir() + "/analyses.root", "Top");
  MT2Analysis<MT2EstimateTree>* mcSR_wjets18 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg18.getEventYieldDir() + "/analyses.root", "WJets");
  MT2Analysis<MT2EstimateTree>* mcSR_zjets18 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg18.getEventYieldDir() + "/analyses.root", "ZJets");

  TFile* tmp18 = new TFile("tmp18.root", "RECREATE");

  MT2Analysis<MT2EstimateTree>* mcSR_llep18 = new MT2Analysis<MT2EstimateTree>( *(mcSR_top18) );
  mcSR_llep18->setName("mcSR_llep18");
  (*mcSR_llep18) += (*(mcSR_top18));
  (*mcSR_llep18) += (*(mcSR_wjets18));

  // put everything together
  std::vector< MT2Analysis<MT2EstimateTree>* > mcSR_llep_;
  std::vector< MT2Analysis<MT2EstimateTree>* > mcSR_zjets_;
  mcSR_llep_.push_back(mcSR_llep16);
  mcSR_zjets_.push_back(mcSR_zjets16);
  mcSR_llep_.push_back(mcSR_llep17);
  mcSR_zjets_.push_back(mcSR_zjets17);
  mcSR_llep_.push_back(mcSR_llep18);
  mcSR_zjets_.push_back(mcSR_zjets18);

  // CR MC 16
  std::string mcFile16 = cfg16.getEventYieldDir() + "/llepControlRegion/mc.root";
  MT2Analysis<MT2EstimateTree>* mcCR16 = MT2Analysis<MT2EstimateTree>::readFromFile(mcFile16, "llepCR");

  MT2Analysis<MT2EstimateTree>* mcCR_top16 = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg16.regionsSet(), mcCR16, "id>=300 && id<500");
  MT2Analysis<MT2EstimateTree>* mcCR_wjets16 = MT2EstimateTree::makeAnalysisFromInclusiveTree("W+jets", cfg16.regionsSet(), mcCR16, "id>=500 && id<600");
  MT2Analysis<MT2EstimateTree>* mcCR_llep16 = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg16.regionsSet(), mcCR16, "(id>=300 && id<500) || (id>=500 && id<600)");
  MT2Analysis<MT2EstimateTree>* mcCR_zll16 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg16.getEventYieldDir() + "/zllControlRegion/ZllPurityTrees.root", "DYJets");

  mcCR_top16   ->setColor(855);
  mcCR_wjets16 ->setColor(417);
  mcCR_llep16  ->setColor(kAzure+4);
  mcCR_zll16-> setColor(kZinv);


  // CR MC 17
  std::string mcFile17 = cfg17.getEventYieldDir() + "/llepControlRegion/mc.root";
  MT2Analysis<MT2EstimateTree>* mcCR17 = MT2Analysis<MT2EstimateTree>::readFromFile(mcFile17, "llepCR");

  MT2Analysis<MT2EstimateTree>* mcCR_top17 = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg17.regionsSet(), mcCR17, "id>=300 && id<500");
  MT2Analysis<MT2EstimateTree>* mcCR_wjets17 = MT2EstimateTree::makeAnalysisFromInclusiveTree("W+jets", cfg17.regionsSet(), mcCR17, "id>=500 && id<600");
  MT2Analysis<MT2EstimateTree>* mcCR_llep17 = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg17.regionsSet(), mcCR17, "(id>=300 && id<500) || (id>=500 && id<600)");
  MT2Analysis<MT2EstimateTree>* mcCR_zll17 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg17.getEventYieldDir() + "/zllControlRegion/ZllPurityTrees.root", "DYJets");

  mcCR_top17   ->setColor(855);
  mcCR_wjets17 ->setColor(417);
  mcCR_llep17  ->setColor(kAzure+4);
  mcCR_zll17-> setColor(kZinv);


  // CR MC 18
  std::string mcFile18 = cfg18.getEventYieldDir() + "/llepControlRegion/mc.root";
  MT2Analysis<MT2EstimateTree>* mcCR18 = MT2Analysis<MT2EstimateTree>::readFromFile(mcFile18, "llepCR");

  MT2Analysis<MT2EstimateTree>* mcCR_top18 = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg18.regionsSet(), mcCR18, "id>=300 && id<500");
  MT2Analysis<MT2EstimateTree>* mcCR_wjets18 = MT2EstimateTree::makeAnalysisFromInclusiveTree("W+jets", cfg18.regionsSet(), mcCR18, "id>=500 && id<600");
  MT2Analysis<MT2EstimateTree>* mcCR_llep18 = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg18.regionsSet(), mcCR18, "(id>=300 && id<500) || (id>=500 && id<600)");
  MT2Analysis<MT2EstimateTree>* mcCR_zll18 = MT2Analysis<MT2EstimateTree>::readFromFile(cfg18.getEventYieldDir() + "/zllControlRegion/ZllPurityTrees.root", "DYJets");

  mcCR_top18   ->setColor(855);
  mcCR_wjets18 ->setColor(417);
  mcCR_llep18  ->setColor(kAzure+4);
  mcCR_zll18-> setColor(kZinv);


  // put everything together

  std::vector< MT2Analysis<MT2EstimateTree>* > mcCR_llep_;
  std::vector< MT2Analysis<MT2EstimateTree>* > mcCR_zll_;

  mcCR_llep_.push_back(mcCR_llep16);
  mcCR_zll_.push_back(mcCR_zll16);
  mcCR_llep_.push_back(mcCR_llep17);
  mcCR_zll_.push_back(mcCR_zll17);
  mcCR_llep_.push_back(mcCR_llep18);
  mcCR_zll_.push_back(mcCR_zll18);

  std::string plotsDir_top = cfg16.getEventYieldDir() +  "/ratioPlots_combined/ratios_top/";
  std::string plotsDir_wjets = cfg16.getEventYieldDir() +  "/ratioPlots_combined/ratios_wjets/";
  std::string plotsDir_llep = cfg16.getEventYieldDir() +  "/ratioPlots_combined/ratios_llep/";
  std::string plotsDir_zll = cfg16.getEventYieldDir() +  "/ratioPlots_combined/ratios_zll/";

  std::vector<float> lumi = {cfg16.lumi(),cfg17.lumi(),cfg18.lumi()};
  std::vector<int> year = {cfg16.year(),cfg17.year(),cfg18.year()};

//  MT2DrawTools_ForRatios dt_wjets(plotsDir_wjets, cfg.lumi());
//  dt_wjets.set_shapeNorm(shapeNorm);
//  dt_wjets.set_data(mcSR_wjets);
//  dt_wjets.set_mc(&mcCR_wjets_);
//  dt_wjets.set_lumi(cfg.lumi());
//  dt_wjets.set_year(cfg.year());
//
//  MT2DrawTools_ForRatios dt_top(plotsDir_top, cfg.lumi());
//  dt_top.set_shapeNorm(shapeNorm);
//  dt_top.set_data(mcSR_top);
//  dt_top.set_mc(&mcCR_top_);
//  dt_top.set_lumi(cfg.lumi());
//  dt_top.set_year(cfg.year());

  MT2DrawTools_ForRatios dt_llep(plotsDir_llep, lumi);
  dt_llep.set_shapeNorm(shapeNorm);
  dt_llep.set_data(&mcSR_llep_);
  dt_llep.set_mc(&mcCR_llep_);
  dt_llep.set_lumi(lumi);
  dt_llep.set_year(year);

  MT2DrawTools_ForRatios dt_zll(plotsDir_zll, lumi);
  dt_zll.set_shapeNorm(shapeNorm);
  dt_zll.set_data(&mcSR_zjets_);
  dt_zll.set_mc(&mcCR_zll_);
  dt_zll.set_lumi(lumi);
  dt_zll.set_year(year);

  std::string selection = "(ht>250 && met>250 && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.)";
  std::string selection_zll  = "(ht>250 && met>250 && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200. && abs(Z_mass-91.19)<20 && lep_pt0>100 && lep_pt1>30)";

//  dt_wjets.drawRegionYields_fromTree("incl_ratio_mt2", "mt2", selection, selection, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
//  dt_wjets.drawRegionYields_fromTree("incl_ratio_met", "met", selection, selection, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
//  dt_wjets.drawRegionYields_fromTree("incl_ratio_ht", "ht", selection, selection, 9, 250, 2050, "H_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
//  dt_wjets.drawRegionYields_fromTree("incl_ratio_nJets", "nJets", selection, selection, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
//  dt_wjets.drawRegionYields_fromTree("incl_ratio_nBJets", "nBJets", selection, selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
//
//  dt_top.drawRegionYields_fromTree("incl_ratio_mt2", "mt2", selection, selection, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Top");
//  dt_top.drawRegionYields_fromTree("incl_ratio_met", "met", selection, selection, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Top");
//  dt_top.drawRegionYields_fromTree("incl_ratio_ht", "ht", selection, selection, 9, 250, 2050, "H_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Top");
//  dt_top.drawRegionYields_fromTree("incl_ratio_nJets", "nJets", selection, selection, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "Top");
//  dt_top.drawRegionYields_fromTree("incl_ratio_nBJets", "nBJets", selection, selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel, 0., 2.), "Top";

  dt_llep.drawRegionYields_fromTree("incl_ratio_mt2", "mt2", selection, selection, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "LLep");
  dt_llep.drawRegionYields_fromTree("incl_ratio_met", "met", selection, selection, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "LLep");
  dt_llep.drawRegionYields_fromTree("incl_ratio_ht", "ht", selection, selection, 9, 250, 2050, "H_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "LLep");
  dt_llep.drawRegionYields_fromTree("incl_ratio_nJets", "nJets", selection, selection, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "LLep");
  dt_llep.drawRegionYields_fromTree("incl_ratio_nBJets", "nBJets", selection, selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "LLep");

  dt_zll.drawRegionYields_fromTree("incl_ratio_mt2", "mt2", selection, selection_zll, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
  dt_zll.drawRegionYields_fromTree("incl_ratio_met", "met", selection, selection_zll, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
  dt_zll.drawRegionYields_fromTree("incl_ratio_ht", "ht", selection, selection_zll, 9, 250, 2050, "H_{T}", "GeV", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
  dt_zll.drawRegionYields_fromTree("incl_ratio_nJets", "nJets", selection, selection_zll, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
  dt_zll.drawRegionYields_fromTree("incl_ratio_nBJets", "nBJets", selection, selection_zll, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
//

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
