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


  if( argc<2 ) {
    std::cout << "USAGE: ./drawLostLeptonMCRatios [configFileName] [lumi/shape]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }


  MT2DrawTools_ForRatios::setStyle();

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

  std::string cutsLabel = getCutLabel(250, -1, "H_{T}", "GeV");
  int jetMin=2, jetMax=-1;
  int bMin=0, bMax=-1;
  std::string jetCutsLabel = getJetCutLabel(jetMin, jetMax, bMin, bMax);

  // SR MC
  MT2Analysis<MT2EstimateTree>* mcSR_top = MT2Analysis<MT2EstimateTree>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "Top");
  MT2Analysis<MT2EstimateTree>* mcSR_wjets = MT2Analysis<MT2EstimateTree>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "WJets");
  MT2Analysis<MT2EstimateTree>* mcSR_zjets = MT2Analysis<MT2EstimateTree>::readFromFile(cfg.getEventYieldDir() + "/analyses.root", "ZJets");  

  TFile* tmp = new TFile("tmp.root", "RECREATE");
  MT2Analysis<MT2EstimateTree>* mcSR_llep = new MT2Analysis<MT2EstimateTree>( *(mcSR_top) );
  mcSR_llep->setName("mcSR_llep");
  (*mcSR_llep) += (*(mcSR_top));
  (*mcSR_llep) += (*(mcSR_wjets));

  // CR MC
  std::string mcFile = cfg.getEventYieldDir() + "/llepControlRegion/mc.root";
  MT2Analysis<MT2EstimateTree>* mcCR = MT2Analysis<MT2EstimateTree>::readFromFile(mcFile, "llepCR");
  
  MT2Analysis<MT2EstimateTree>* mcCR_top = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg.regionsSet(), mcCR, "id>=300 && id<500"); 
  MT2Analysis<MT2EstimateTree>* mcCR_wjets = MT2EstimateTree::makeAnalysisFromInclusiveTree("W+jets", cfg.regionsSet(), mcCR, "id>=500 && id<600"); 
  MT2Analysis<MT2EstimateTree>* mcCR_llep = MT2EstimateTree::makeAnalysisFromInclusiveTree("Top", cfg.regionsSet(), mcCR, "(id>=300 && id<500) || (id>=500 && id<600)"); 
  MT2Analysis<MT2EstimateTree>* mcCR_zll = MT2Analysis<MT2EstimateTree>::readFromFile(cfg.getEventYieldDir() + "/zllControlRegion/ZllPurityTrees.root", "DYJets");


  mcCR_top   ->setColor(855);
  mcCR_wjets ->setColor(417);
  mcCR_llep  ->setColor(kAzure+4); 
  mcCR_zll-> setColor(kZinv);

  std::vector< MT2Analysis<MT2EstimateTree>* > mcCR_wjets_;
  std::vector< MT2Analysis<MT2EstimateTree>* > mcCR_top_;
  std::vector< MT2Analysis<MT2EstimateTree>* > mcCR_llep_;
  std::vector< MT2Analysis<MT2EstimateTree>* > mcCR_zll_;

  mcCR_wjets_.push_back(mcCR_wjets);
  mcCR_top_.push_back(mcCR_top);
  mcCR_llep_.push_back(mcCR_llep);
  mcCR_zll_.push_back(mcCR_zll);  

  std::string plotsDir_top = cfg.getEventYieldDir() +  "/ratioPlots/ratios_top/";
  std::string plotsDir_wjets = cfg.getEventYieldDir() +  "/ratioPlots/ratios_wjets/";
  std::string plotsDir_llep = cfg.getEventYieldDir() +  "/ratioPlots/ratios_llep/";
  std::string plotsDir_zll = cfg.getEventYieldDir() +  "/ratioPlots/ratios_zll/";

  MT2DrawTools_ForRatios dt_wjets(plotsDir_wjets, cfg.lumi());
  dt_wjets.set_shapeNorm(shapeNorm);
  dt_wjets.set_data(mcSR_wjets);
  dt_wjets.set_mc(&mcCR_wjets_);
  dt_wjets.set_lumi(cfg.lumi());
  dt_wjets.set_year(cfg.year());

  MT2DrawTools_ForRatios dt_top(plotsDir_top, cfg.lumi());
  dt_top.set_shapeNorm(shapeNorm);
  dt_top.set_data(mcSR_top);
  dt_top.set_mc(&mcCR_top_);
  dt_top.set_lumi(cfg.lumi());
  dt_top.set_year(cfg.year());

  MT2DrawTools_ForRatios dt_llep(plotsDir_llep, cfg.lumi());
  dt_llep.set_shapeNorm(shapeNorm);
  dt_llep.set_data(mcSR_llep);
  dt_llep.set_mc(&mcCR_llep_);
  dt_llep.set_lumi(cfg.lumi());
  dt_llep.set_year(cfg.year());
 
  MT2DrawTools_ForRatios dt_zll(plotsDir_zll, cfg.lumi());
  dt_zll.set_shapeNorm(shapeNorm);
  dt_zll.set_data(mcSR_zjets);
  dt_zll.set_mc(&mcCR_zll_);
  dt_zll.set_lumi(cfg.lumi());
  dt_zll.set_year(cfg.year());
  
  std::string selection = "(ht>250 && met>250 && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.)"; 
  std::string selection_zll  = "(ht>250 && met>250 && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200. && abs(Z_mass-91.19)<20 && lep_pt0>100 && lep_pt1>30)"; 
   
  dt_wjets.drawRegionYields_fromTree("incl_ratio_mt2", "mt2", selection, selection, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
  dt_wjets.drawRegionYields_fromTree("incl_ratio_met", "met", selection, selection, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
  dt_wjets.drawRegionYields_fromTree("incl_ratio_ht", "ht", selection, selection, 9, 250, 2050, "H_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
  dt_wjets.drawRegionYields_fromTree("incl_ratio_nJets", "nJets", selection, selection, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "Wjets");
  dt_wjets.drawRegionYields_fromTree("incl_ratio_nBJets", "nBJets", selection, selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "Wjets");

  dt_top.drawRegionYields_fromTree("incl_ratio_mt2", "mt2", selection, selection, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Top");
  dt_top.drawRegionYields_fromTree("incl_ratio_met", "met", selection, selection, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Top");
  dt_top.drawRegionYields_fromTree("incl_ratio_ht", "ht", selection, selection, 9, 250, 2050, "H_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Top");
  dt_top.drawRegionYields_fromTree("incl_ratio_nJets", "nJets", selection, selection, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "Top");
  dt_top.drawRegionYields_fromTree("incl_ratio_nBJets", "nBJets", selection, selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel, 0., 2.), "Top";

  dt_llep.drawRegionYields_fromTree("incl_ratio_mt2", "mt2", selection, selection, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets+Top");
  dt_llep.drawRegionYields_fromTree("incl_ratio_met", "met", selection, selection, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets+Top");
  dt_llep.drawRegionYields_fromTree("incl_ratio_ht", "ht", selection, selection, 9, 250, 2050, "H_{T}", "GeV", cutsLabel, jetCutsLabel, 0., 2., "Wjets+Top");
  dt_llep.drawRegionYields_fromTree("incl_ratio_nJets", "nJets", selection, selection, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "Wjets+Top");
  dt_llep.drawRegionYields_fromTree("incl_ratio_nBJets", "nBJets", selection, selection, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel, 0., 2., "Wjets+Top");

  dt_zll.drawRegionYields_fromTree("incl_ratio_mt2", "mt2", selection, selection_zll, 10, 200., 1200., "M_{T2}", "GeV", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
  dt_zll.drawRegionYields_fromTree("incl_ratio_met", "met", selection, selection_zll, 10, 200., 1200., "Missing E_{T}", "GeV", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
  dt_zll.drawRegionYields_fromTree("incl_ratio_ht", "ht", selection, selection_zll, 9, 250, 2050, "H_{T}", "GeV", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
  dt_zll.drawRegionYields_fromTree("incl_ratio_nJets", "nJets", selection, selection_zll, 10, 1.5, 11.5, "Number of Jets (p_{T} > 30 GeV)", "", cutsLabel, jetCutsLabel, 3., 8., "Zjets");
  dt_zll.drawRegionYields_fromTree("incl_ratio_nBJets", "nBJets", selection, selection_zll, 6, -0.5, 5.5, "Number of b-Jets (p_{T} > 20 GeV)", "", cutsLabel, jetCutsLabel, 3., 8., "Zjets");


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
