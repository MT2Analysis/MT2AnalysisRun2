#include <iostream>
#include <vector>
#include <string>
#include <cmath>

#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TLine.h"


#include "../interface/MT2Config.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2Analysis.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2Estimate.h"
#include "../interface/MT2EstimateTree.h"
//#include "../interface/MT2DrawTools.h"

#include "../interface/MT2Sample.h"
#include "../interface/MT2EstimateSyst.h"

#include <stdio.h>

using namespace std;

/*
This script aims to compare the shape of the ZllEstimates between two files (that have to be manually entered in the script). For instance, we compare the estimates between Moriond2017 and those produced with our framework and 2016 samples (in order to check if we get the same results)

It has also implemented two functions on the study of the hybrid shape: one of them loops on the different minStatistics files, and plot the estimates, the other plots the difference in the estimate to the minStatistics 50 (=reference)

The function plotEstimateVSNb studies the Zll MT2 shape as a function of Nb
 */


void plotMoriond2017vsETH2018(string, bool, bool);
void studyHybridShape(string);
void studyHybridShape_comparisonToReference(string);
void plotEstimateVSNb();
void plotEstimateVSNb_combined(); //does the same as the above function but with the three years combined 


int main( int argc, char* argv[] ) {
  // enter the name of the estimate you wish to study:
  string estimateName = "ZinvEstimateFromZll_hybrid"; //"ZinvZllRatio_TR_int"; // "zllHybrid_shape_TR"; //"purity_forHybrid"; //"alpha"; //"zllData_forHybrid"; //"bin_extrapol"; 
  
  bool doComparison = false;
  bool checkRegion = false;
  bool plotRatio = false;

  //comparison between Moriond2017 and ETH2018 estimates
  //this function can be used to compare whatever two estimates -> function should be renamed compareTwoEstimates()
  if(doComparison){
    plotMoriond2017vsETH2018(estimateName, checkRegion,  plotRatio);
  }
 
  
  //hybrid shape investigations
  bool doHybrid = false;
  if(doHybrid){
    studyHybridShape(estimateName);
  }

  bool doNormalizedHybrid = false;
  if(doNormalizedHybrid){
    studyHybridShape_comparisonToReference(estimateName);
  }

  bool MT2vsNb = false;
  if(MT2vsNb){
  //in order to check if MT2 shape depends on Nb in each region
    plotEstimateVSNb();
  }

  bool MT2vsNb_combined = true;
  if(MT2vsNb_combined){
  //in order to check if MT2 shape depends on Nb in each region with the three years combined
    plotEstimateVSNb_combined();
  }

  return 0;

}



void plotMoriond2017vsETH2018(string estimateName, bool checkRegion, bool plotRatio){

  //----------------------------------------------------------------------------------
  //please add the files that you want to compare, as well as their luminosity
   
  //we select the files and estimates we want to compare together
  //file 1: ZinvEstimateFromZll_hybrid from Moriond 2017
  //string file1 = "/mnt/t3nfs01/data01/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/EventYields_dataETH_SnTMC_35p9ifb/zinvFromZll.root";
  string file1 = "/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_moriond2019_35p9ifb/zinvFromZll_beforeCleaning.root";
  MT2Analysis<MT2Estimate>* estimate1 = MT2Analysis<MT2Estimate>::readFromFile(file1, estimateName);
  
  //file 2: ZinvEstimateFromZll_hybrid from ETH framework 2018
  //string file2 = "/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/zinvFromZll.root";
  //string file2 = "/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_moriond2019_zurich2016_35p9ifb/zinvFromZll.root";
  string file2 = "/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_moriond2019_35p9ifb/zinvFromZll.root";
  MT2Analysis<MT2Estimate>* estimate2 = MT2Analysis<MT2Estimate>::readFromFile(file2, estimateName);

  float lumi1 = 35.9;
  float lumi2 = 35.9;

  //-----------------------------------------------------------------------------------
  
  // regions (from zurich2016)
  std::set<MT2Region> regions = estimate1->getRegions();
 

  //check if two files are working in the same region
   
  if(checkRegion){
    std::set<MT2Region> regions2 = estimate2->getRegions();
    cout << "region1 size: " << regions.size() << endl;
    cout << "region2 size: " << regions2.size() << endl;
  
    for(std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR){
      for(std::set<MT2Region>::iterator iR2=regions2.begin(); iR2!=regions2.end(); ++iR2){
   
	//current region
	MT2Region thisRegion((*iR));
	MT2Region thisRegion2((*iR2));

	vector<string> regionNames = thisRegion.getNiceNames();
	vector<string> regionNames2 = thisRegion2.getNiceNames();
	if((*iR) == (*iR2)){
	  if(regionNames[0]!=regionNames2[0]){
	    cout << "erreur: les deux regions ne matchent pas!" << endl;
	  }
	  else{
	    cout << "tout va bien" << endl;
	  }
	}
      }
    }
  }

  
  // we declare the histograms where the estimates that will contain the yields
  TH1D* hist1;
  TH1D* hist2;
  TH1D* hist_ratio;
  
  //for the naming of the plots
  int n(1); 
    
  // we loop on all the regions and get the yield histogram there
  for(std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR){
   
    //current region
    MT2Region thisRegion((*iR));
    vector<string> regionNames = thisRegion.getNiceNames();
    TString name = (regionNames[0]+regionNames[1]).c_str();


    hist1 = estimate1->get(*iR)->yield;
    hist2 = estimate2->get(*iR)->yield;

    //histogram that will contain the ratio of the two previous ones
    hist_ratio = (TH1D*) hist1->Clone("hist_ratio");
    hist_ratio->Divide(hist2);

    //normalization to the luminosity
    hist1->Scale(lumi1);
    hist2->Scale(lumi2);

    TCanvas *c = new TCanvas();
    // c->SetLogy(); //y log scale
    c->SetGrid();

    TAxis *Xaxis = hist1->GetXaxis();
    TAxis *Yaxis = hist1->GetYaxis();
    Xaxis->SetTitle("bins");
    Xaxis->SetTitleSize(0.045);
    Xaxis->SetLabelSize(0.045);
    Xaxis->SetTitleOffset(1.1);
    //Yaxis->SetRangeUser(-1000,1000);
    Yaxis->SetTitleSize(0.045);
    Yaxis->SetLabelSize(0.045);
    Yaxis->SetTitleOffset(1.26);

    TLegend *leg = new TLegend(0.55, 0.7, 0.75, 0.9);
    leg -> AddEntry(hist1, "Moriond 2017");
    leg -> AddEntry(hist2, "Moriond 2019, with zurich2016" );
    leg -> SetTextSize(0.04);
    leg -> SetLineColor(0);
    leg -> SetFillColorAlpha(0, 0);
    leg -> SetBorderSize(0);
    //leg -> Draw();

    gStyle->SetOptStat(0);

    hist1->SetTitle("ZinvEstimateFromZll_hybrid: " + name);
    hist1->SetLineColor(4);
    hist1->SetLineWidth(2);
    hist2->SetLineColor(2);
    hist2->SetLineWidth(2);

    //hist2->Draw();
    //hist1->Draw("same");


    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.05); // Upper and lower plot are joined
    pad1->SetGridx();         // Vertical grid
    pad1->Draw();             // Draw the upper pad: pad1
    pad1->cd();               // pad1 becomes the current pad
    hist1->SetStats(0);          // No statistics on upper plot
    hist1->Draw();
    hist2->Draw("same");
    leg->Draw("same");


    /*
    int nBins = hist_ratio->GetNbinsX();
    
    Double_t x[nBins];
    Double_t y[nBins];
   
    TGraph* graph = new TGraph();

    for(int iBin(1); iBin<=nBins; ++iBin){
      //x[iBin] = hist_ratio->GetXaxis()->GetBinCenter( hist_ratio->GetXaxis()->FindBin(iBin) );
      //x[iBin] =  hist_ratio->GetXaxis()->FindBin(iBin);
      x[iBin] = iBin;
      y[iBin] = hist_ratio->GetBinContent(iBin);
      graph->SetPoint(iBin-1, x[iBin], y[iBin]);
    }
    */
  

   

    //TLine* line = new TLine(0.1, 1, 5., 1);

    // lower plot will be in pad
    c->cd();          // Go back to the main canvas before defining pad2
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0.2);
    pad2->SetGridx(); // vertical grid
    pad2->Draw();
    pad2->cd();       // pad2 beco
    
    hist_ratio->SetLineColor(1);
    hist_ratio->SetLineWidth(2);
    hist_ratio->Draw("ep");
    //graph->Draw("A*");
    //line->Draw("same");

    TAxis *Xaxisr = hist_ratio->GetXaxis();
    TAxis *Yaxisr = hist_ratio->GetYaxis();
    Xaxisr->SetTitle("");
    Xaxisr->SetTitleSize(0.1);
    Xaxisr->SetLabelSize(0.1);
    Xaxisr->SetTitleOffset(0.3);
    //Yaxisr->SetRangeUser(-1000,1000);
    Yaxisr->SetTitle("Ratio");
    Yaxisr->SetTitleSize(0.1);
    Yaxisr->SetLabelSize(0.1);
    Yaxisr->SetTitleOffset(0.26);

    /*
    graph->GetXaxis()->SetTitle("");
    graph->GetYaxis()->SetTitle("Ratio");  
    graph->GetXaxis()->SetTitleSize(0.1);
    graph->GetXaxis()->SetLabelSize(0.1);
    graph->GetXaxis()->SetTitleOffset(1.1);     
    graph->GetYaxis()->SetTitleSize(0.1);
    graph->GetYaxis()->SetRangeUser(0.35, 1.65);
    graph->GetYaxis()->SetLabelSize(0.1);
    graph->GetYaxis()->SetTitleOffset(1.26);
    */
       
    TString nameOfEstimate = estimateName.c_str();
    TString savename = nameOfEstimate + "_" + to_string(n);
  
    c->SaveAs("EventYields_moriond2019_zurich2016_35p9ifb/plotsZllEstimates/"  + savename + ".pdf");
   
    if(plotRatio){
      //we repeat the same for the ratio
      TCanvas *cr = new TCanvas();
      // c->SetLogy(); //y log scale
      cr->SetGrid();

      TAxis *Xaxisr = hist_ratio->GetXaxis();
      TAxis *Yaxisr = hist_ratio->GetYaxis();
      Xaxisr->SetTitle("bins");
      Xaxisr->SetTitleSize(0.045);
      Xaxisr->SetLabelSize(0.045);
      Xaxisr->SetTitleOffset(1.1);
      //Yaxisr->SetRangeUser(-1000,1000);
      Yaxisr->SetTitleSize(0.045);
      Yaxisr->SetLabelSize(0.045);
      Yaxisr->SetTitleOffset(1.26);

      TLegend *legr = new TLegend(0.55, 0.7, 0.75, 0.9);
      legr -> AddEntry(hist_ratio, "Moriond 2017/Moriond 2019");
      legr -> SetTextSize(0.04);
      legr -> SetLineColor(0);
      legr -> SetFillColorAlpha(0, 1);
      legr -> SetBorderSize(0);
  
      gStyle->SetOptStat(0);

      hist_ratio->SetTitle("ZinvEstimateFromZll_hybrid: " + name);
      hist_ratio->SetLineColor(4);
      hist_ratio->SetLineWidth(2);
    
      hist_ratio->Draw();
      legr->Draw("same");
    
      cr->SaveAs("EventYields_moriond2019_zurich2016_35p9ifb/plotsZllEstimates/ratio_" + savename + ".pdf");
      // cr->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/" + nameOfEstimate + "/ratio_" + savename + ".png");
    }

    ++n;
       
  }
}





void studyHybridShape(string estimateName){
 
  //we loop on all the files with different statistics number (hybrid shape)
  vector<double> minStatistics = {1., 10., 50.}; //{50., 60., 70.};
  unsigned int size = minStatistics.size();
 
  vector<std::string> file(size);
  vector<MT2Analysis<MT2Estimate>*> estimate(size);

  //we get the files and the estimate
  for(unsigned int k(0); k<size; ++k){
    string numberFile = to_string(minStatistics[k]);
    file[k] = "/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/zinvFromZll_" + numberFile + ".root";
    estimate[k] = MT2Analysis<MT2Estimate>::readFromFile(file[k], estimateName);
  }
    
 
  // regions (from zurich2016)
  std::set<MT2Region> regions = estimate[0]->getRegions();
  
  vector<TH1D*> hist(size);
  int n(1); 
  
  // we loop on all the regions and get the yield histogram there
  for(std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR){
     //current region
      MT2Region thisRegion((*iR));
      vector<string> regionNames = thisRegion.getNiceNames();
      TString name = (regionNames[0]+regionNames[1]).c_str();

      TLegend *leg = new TLegend(0.55, 0.40, 0.75, 0.88);

      TCanvas *c = new TCanvas();
      c->SetGrid();

      for(unsigned int k(0); k<size; ++k){

	hist[k] = estimate[k]->get(*iR)->yield;

	TAxis *Xaxis = hist[0]->GetXaxis();
	TAxis *Yaxis = hist[0]->GetYaxis();
	Xaxis->SetTitle("bins");
	Xaxis->SetTitleSize(0.045);
	Xaxis->SetLabelSize(0.045);
	Xaxis->SetTitleOffset(1.1);
	//Yaxis->SetRangeUser(-1000,1000);
	Yaxis->SetTitleSize(0.045);
	Yaxis->SetLabelSize(0.045);
	Yaxis->SetTitleOffset(1.26);

	int doubleToInt = floor(minStatistics[k]);
	TString legendName = to_string(doubleToInt);
	if(k==0 && legendName == "111"){
	  leg -> AddEntry(hist[0], "Only data");
	}
	else if(k==0 && legendName == "222"){
	  leg -> AddEntry(hist[0], "Only MC");
	}
	else{
	  leg -> AddEntry(hist[k], "Min Statistics = " + legendName);
	}
	leg -> SetTextSize(0.04);
	leg -> SetLineColor(0);
	leg -> SetFillColorAlpha(0, 0);
	leg -> SetBorderSize(0);

	gStyle->SetOptStat(0);

	hist[0]->SetTitle("ZinvEstimateFromZll_hybrid: " + name);
	hist[k]->SetLineColor(k+1);
	hist[k]->SetLineWidth(2);
     
	hist[k]->Draw("same");

	leg->Draw("same");
    
	TString savename = to_string(n);//+"_" + name;
	TString nameOfEstimate = estimateName.c_str();
    
    }
     
    TString savename = to_string(n);//+"_" + name;
    TString nameOfEstimate = estimateName.c_str();
    c->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/" + nameOfEstimate + "/hybridShape/" + savename + ".pdf");
    c->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/" + nameOfEstimate + "/hybridShape/" + savename + ".png");
    //c->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/hybridShape" + nameOfEstimate + "/" + savename + ".pdf");
    //c->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/hybridShape" + nameOfEstimate + "/" + savename + ".png");
      
    ++n;
  }
}







void studyHybridShape_comparisonToReference(string estimateName){
 
  //we loop on all the files with different statistics number (hybrid shape)
  vector<double> minStatistics = {50., 0., 1., 10.}; //{50., 10., 20., 30., 40., 60., 70., 80., 90.};//, 100., 200., 300., 400.}; //{50., 60., 70.};
  unsigned int size = minStatistics.size();
 
  vector<std::string> file(size);
  vector<MT2Analysis<MT2Estimate>*> estimate(size);

  //we get the files and the estimate
  for(unsigned int k(0); k<size; ++k){
    string numberFile = to_string(minStatistics[k]);
    file[k] = "/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/zinvFromZll_" + numberFile + ".root";
    estimate[k] = MT2Analysis<MT2Estimate>::readFromFile(file[k], estimateName);
  }
    
 
  // regions (from zurich2016)
  std::set<MT2Region> regions = estimate[0]->getRegions();
  
  vector<TH1D*> hist_ratio(size-1);
  int n(1); 
  
  // we loop on all the regions and get the yield histogram there
  for(std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR){
     //current region
      MT2Region thisRegion((*iR));
      vector<string> regionNames = thisRegion.getNiceNames();
      TString name = (regionNames[0]+regionNames[1]).c_str();

      TLegend *leg = new TLegend(0.55, 0.40, 0.75, 0.88);

      TCanvas *c = new TCanvas();
      c->SetGrid();

      for(unsigned int k(1); k<size; ++k){

	//we take as the reference the estimate with minStatistics = 50.
	TH1D* hist_ref = estimate[0]->get(*iR)->yield;

	//we first fill hist_ratio by the corresponding yield in each region
	hist_ratio[k] = estimate[k]->get(*iR)->yield;
	
	//we then divide it by the reference
	hist_ratio[k]->Divide(hist_ref);

	TAxis *Xaxis = hist_ref->GetXaxis();
	TAxis *Yaxis = hist_ref->GetYaxis();
	Xaxis->SetTitle("bins");
	Xaxis->SetTitleSize(0.045);
	Xaxis->SetLabelSize(0.045);
	Xaxis->SetTitleOffset(1.1);
	//Yaxis->SetRangeUser(-1000,1000);
	Yaxis->SetTitle("Normalized to Min Statistics = 50");
	Yaxis->SetTitleSize(0.045);
	Yaxis->SetLabelSize(0.045);
	Yaxis->SetTitleOffset(1.26);

	int doubleToInt = floor(minStatistics[k]);
	TString legendName = to_string(doubleToInt);
	leg -> AddEntry(hist_ratio[k], "Min Statistics = " + legendName);
	leg -> SetTextSize(0.04);
	leg -> SetLineColor(0);
	leg -> SetFillColorAlpha(0, 0);
	leg -> SetBorderSize(0);

	gStyle->SetOptStat(0);

	hist_ref->SetTitle("ZinvEstimateFromZll_hybrid: " + name);
	hist_ratio[k]->SetLineColor(k+1);
	hist_ratio[k]->SetLineWidth(2);
     
	hist_ratio[k]->Draw("same");

	leg->Draw("same");
    
	TString savename = to_string(n);//+"_" + name;
	TString nameOfEstimate = estimateName.c_str();
    
    }
     
    TString savename = "normalized_" + to_string(n);//+"_" + name;
    TString nameOfEstimate = estimateName.c_str();
    c->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/" + nameOfEstimate + "/hybridShape/" + savename + ".pdf");
    c->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/" + nameOfEstimate + "/hybridShape/" + savename + ".png");
    //c->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/hybridShape" + nameOfEstimate + "/" + savename + ".pdf");
    //c->SaveAs("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis_2016compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/plotsZllEstimates/hybridShape" + nameOfEstimate + "/" + savename + ".png");
      
    ++n;
  }
}




void plotEstimateVSNb(){

  TFile* file = TFile::Open("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_41p9ifb_2017/zllControlRegion/mc_forZinvEst.root");
  // TFile* file = TFile::Open("/mnt/t3nfs01/data01/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/EventYields_dataETH_SnTMC_35p9ifb/zllControlRegion/mc_forZinvEst.root");
  string fileR = "/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_41p9ifb_2017/zllControlRegion/mc_forZinvEst.root";
  //string fileR = "/mnt/t3nfs01/data01/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/EventYields_dataETH_SnTMC_35p9ifb/zllControlRegion/mc_forZinvEst.root";

  MT2Analysis<MT2Estimate>* estimate = MT2Analysis<MT2Estimate>::readFromFile(fileR, "zllCR");
  
  set<MT2Region> regions = estimate->getRegions();
 
  int p(1);
  int color(1);
  int legIndex(1);
  double i(0.2);
  int fileIndex(1);
  vector<TLegend*> tableLeg(legIndex);
  TH1D* hist;
  TTree* myTree;
  TCanvas *c = new TCanvas;
  c->SetLogy(); 
  int htMin_precedingRegion(250);
  int htMax_precedingRegion(450);
  int NjMin_precedingRegion(2);
  int NjMax_precedingRegion(3);

  for(std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR){
   
    MT2Region* thisRegion = new MT2Region(*iR);
  
    vector<string> regionName = thisRegion->getNiceNames();
                              
    int htMinInt = floor(thisRegion->htMin());
    string htMin = to_string(htMinInt);
    
    int htMaxInt = floor(thisRegion->htMax());
    string htMax = to_string(htMaxInt);
    
    int NjMinInt = floor(thisRegion->nJetsMin());
    string NjMin = to_string(NjMinInt);
    
    int NjMaxInt = floor(thisRegion->nJetsMax());
    string NjMax = to_string(NjMaxInt);
    
    int NbMinInt = floor(thisRegion->nBJetsMin());
    string NbMin = to_string(NbMinInt);

    int NbMaxInt = floor(thisRegion->nBJetsMax());
    string NbMax = to_string(NbMaxInt);
    string regionFile;
  

    //if(htMaxInt!=350 || htMinInt !=350){
    if(NjMinInt != NjMaxInt){
      if(htMax != "-1"){
	if(NjMax != "-1"){
	  if(NbMin==NbMax){
	    regionFile = "zllCR/HT" + htMin + "to" + htMax + "_j" + NjMin + "to" + NjMax + "_b" + NbMin + "/yield_zllCR_HT" + htMin + "to" + htMax + "_j" + NjMin + "to" + NjMax + "_b" + NbMin;
	  }
	  else if(NbMax == "-1"){
	    regionFile = "zllCR/HT" + htMin + "to" + htMax + "_j" + NjMin + "to" + NjMax + "_b" + NbMin + "toInf"  + "/yield_zllCR_HT" + htMin + "to" + htMax + "_j" + NjMin + "to" + NjMax + "_b" + NbMin + "toInf";
	  }
	}
	else if(NjMax == "-1"){
	  if(NbMin==NbMax){
	    regionFile = "zllCR/HT" + htMin + "to" + htMax + "_j" + NjMin + "toInf_b" + NbMin + "/yield_zllCR_HT" + htMin + "to" + htMax + "_j" + NjMin + "toInf_b" + NbMin;
	  }
	  else if(NbMax == "-1"){
	    regionFile = "zllCR/HT" + htMin + "to" + htMax + "_j" + NjMin + "toInf_b" + NbMin + "toInf"  + "/yield_zllCR_HT" + htMin + "to" + htMax + "_j" + NjMin + "toInf_b" + NbMin + "toInf";
	  }
	}
      }
       else if (htMax == "-1"){
	  if(NjMax != "-1"){
	    if(NbMin==NbMax){
	      regionFile = "zllCR/HT" + htMin + "toInf_j" + NjMin + "to" + NjMax + "_b" + NbMin + "/yield_zllCR_HT" + htMin + "toInf_j" + NjMin + "to" + NjMax + "_b" + NbMin;
	    }
	    else if(NbMax == "-1"){
	      regionFile = "zllCR/HT" + htMin + "toInf_j" + NjMin + "to" + NjMax + "_b" + NbMin + "toInf"  + "/yield_zllCR_HT" + htMin + "toInf_j" + NjMin + "to" + NjMax + "_b" + NbMin + "toInf";
	    }
	  }
	  else if(NjMax == "-1"){
	    if(NbMin==NbMax){
	      regionFile = "zllCR/HT" + htMin + "toInf_j" + NjMin + "toInf_b" + NbMin + "/yield_zllCR_HT" + htMin + "toInf_j" + NjMin + "toInf_b" + NbMin;
	    }
	    else if(NbMax == "-1"){
	      regionFile = "zllCR/HT" + htMin + "toInf_j" + NjMin + "toInf_b" + NbMin + "toInf"  + "/yield_zllCR_HT" + htMin + "toInf_j" + NjMin + "toInf_b" + NbMin + "toInf";
	    }
	  }
	}
      
      myTree  = (TTree*) file->Get(regionFile.c_str())->Clone("myTree");
      float Z_pT;
      float Z_mass;
      float ht;
      float mt2, lep_pt0, lep_pt1;
      float weight;
      float HLT_weight;
      myTree -> SetBranchAddress("Z_pt", &Z_pT);
      myTree -> SetBranchAddress("Z_mass", &Z_mass);
      myTree -> SetBranchAddress("ht", &ht);
      myTree -> SetBranchAddress("mt2", &mt2);
      myTree -> SetBranchAddress("lep_pt0", &lep_pt0);
      myTree -> SetBranchAddress("lep_pt1", &lep_pt1);
      myTree -> SetBranchAddress("weight", &weight);
      myTree -> SetBranchAddress("HLT_weight", &HLT_weight);
      
       
      cout << p << " : " << regionFile << endl;
     
      hist = (TH1D*) file->Get(regionFile.c_str())->Clone("hist"); 
      
      //we normalize the histogram
      double integral_hist = hist->Integral(1,-1);
      hist->Scale(1/integral_hist);

      TString hi = to_string(htMinInt);
      TString ha = to_string(htMaxInt);
      TString ji = to_string(NjMinInt);
      TString ja = to_string(NjMaxInt);
      hist->SetTitle("HT" + hi + "to" + ha + "_j" + ji + "to" + ja);
      hist->SetLineWidth(3);

      TAxis *Xaxis = hist->GetXaxis();
      TAxis *Yaxis = hist->GetYaxis();
      Xaxis->SetTitle("M_{T2} shape");
      Xaxis->SetTitleSize(0.045);
      Xaxis->SetLabelSize(0.045);
      Xaxis->SetTitleOffset(1.1);
      Yaxis->SetTitleSize(0.045);
      Yaxis->SetLabelSize(0.045);
      Yaxis->SetTitleOffset(1.26);

      gStyle->SetOptStat(0);
      
      TString n=to_string(p);
      TString n_previous=to_string(p-1);
      TString nb = to_string(NbMinInt);


      if(p==1){
	tableLeg[legIndex-1] = new TLegend(0.8, 0.7, 0.85, 0.9);
        if(NbMaxInt == -1){
	  tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb + "toInf");
        }
	else{
	  tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb);
        }
	tableLeg[legIndex-1] -> SetTextSize(0.04);
	tableLeg[legIndex-1] -> SetLineColor(0);
	tableLeg[legIndex-1] -> SetFillColorAlpha(0, 0);
	tableLeg[legIndex-1] -> SetBorderSize(0);
	
	hist->Draw();
	tableLeg[legIndex-1]->Draw();
	c->SaveAs("test/" + n + ".pdf");
	++legIndex;
	++color;
      }

      if(p>1){
	//cout << "htMin: " << htMin << " htMin_preceding: " << htMin_precedingRegion << endl;
	//cout << "htMax: " << htMax << " htMax_preceding: " << htMax_precedingRegion << endl;
	//cout << "NjMin: " << NjMin << " NjMin_preceding: " << NjMin_precedingRegion << endl;
	//cout << "NjMax: " << NjMax << " NjMax_preceding: " << NjMax_precedingRegion << endl;
	fileIndex = legIndex;
	if((htMinInt == htMin_precedingRegion) && (htMaxInt == htMax_precedingRegion) && (NjMinInt == NjMin_precedingRegion) && (NjMaxInt == NjMax_precedingRegion)){
	  tableLeg[legIndex-1] = new TLegend(0.8, 0.7-i, 0.85, 0.9-i);
	  if(NbMaxInt == -1){
	    tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb + "toInf");
	  }
	  else{
	    tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb);
	  }
	  tableLeg[legIndex-1] -> SetTextSize(0.04);
	  tableLeg[legIndex-1] -> SetLineColor(0);
	  tableLeg[legIndex-1] -> SetFillColorAlpha(0, 0);
	  hist->SetLineColor(color);
	  hist->Draw("same");
	  //(Z_pT>200 && Z_mass > 71.19 && Z_mass < 111.19 && lep_pt0>100. && lep_pt1>30.)
	  for(int k(0); k<legIndex; ++k){
	    ///Z_pt>200 && (Z_mass>71.19 || Z_mass<111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight/HLT_weight
	    tableLeg[k]->Draw("same");
	  }
	  i += 0.2;
	  ++legIndex;
	  ++color;
	}
       	else{
	  color = 2;
	  legIndex = 1;
	  tableLeg[legIndex-1] = new TLegend(0.8, 0.7, 0.85, 0.9);
	  if(NbMaxInt == -1){
	    tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb + "toInf");
	  }
	  else{
	    tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb);
	  }
	  tableLeg[legIndex-1] -> SetTextSize(0.04);
	  tableLeg[legIndex-1] -> SetLineColor(0);
	  tableLeg[legIndex-1] -> SetFillColorAlpha(0, 0);
	  tableLeg[legIndex-1] -> SetBorderSize(0);
	  hist->SetLineColor(color);
	  hist->Draw();
	  tableLeg[legIndex-1]->Draw();
	  i=0.2;
	  ++legIndex;
	  ++color;
	  }
	c->SaveAs("EventYields_dataETH_SnTMC_41p9ifb_2017/plotsZllEstimates/MT2vsNb/" + n + "_normalized.pdf");
	if(legIndex>fileIndex){
	  remove("EventYields_dataETH_SnTMC_41p9ifb_2017/plotsZllEstimates/MT2vsNb/" + n_previous + "_normalized.pdf");
	}
      }
      
       htMin_precedingRegion = htMinInt;
       htMax_precedingRegion = htMaxInt;
       NjMin_precedingRegion = NjMinInt;
       NjMax_precedingRegion = NjMaxInt;

       ++p;
    }//end of valid region
  
  }//end of for on regions

}













void plotEstimateVSNb_combined(){

  TH1::AddDirectory(kFALSE);
  TH1::SetDefaultSumw2(kTRUE);

  bool doZll = true;
  bool doZinv = false;

  TFile* file1;
  TFile* file2;
  TFile* file3;

  //TString directoryToSave = "./MT2vsNb/Zll/";
  TString directoryToSave = "./MT2vsNb/Zinv/";

  //we get the files for the three years
  //Zll:
  if(doZll){
    file1 = TFile::Open("/work/anlyon/EventYields_moriond2019_35p9ifb/zllControlRegion/specialBinning/mc_forZinvEst.root");
    file2 = TFile::Open("/work/anlyon/EventYields_moriond2019_41p9ifb_2017/zllControlRegion/specialBinning/mc_forZinvEst.root");
    file3 = TFile::Open("/work/anlyon/EventYields_moriond2019_59p9ifb_2018/zllControlRegion/specialBinning/mc_forZinvEst.root");
  }

  //Zinv:
  if(doZinv){
    file1 = TFile::Open("/work/anlyon/EventYields_moriond2019_35p9ifb/specialBinning/analyses.root");
    file2 = TFile::Open("/work/anlyon/EventYields_moriond2019_41p9ifb_2017/specialBinning/analyses.root");
    file3 = TFile::Open("/work/anlyon/EventYields_moriond2019_59p9ifb_2018/specialBinning/analyses.root");
  }
  
  //only needed to get the regions
  string fileR = "/work/anlyon/EventYields_moriond2019_35p9ifb/zllControlRegion/specialBinning/mc_forZinvEst.root";
  
  MT2Analysis<MT2Estimate>* estimate = MT2Analysis<MT2Estimate>::readFromFile(fileR, "zllCR");
  
  set<MT2Region> regions = estimate->getRegions();
 
  int p(1);
  int color(1);
  int legIndex(1);
  double i(0.15);
  int fileIndex(1);
  int index_fixed_nJ(1);
  vector<TLegend*> tableLeg(5);
  TH1D* hist1 = 0;
  TH1D* hist2 = 0;
  TH1D* hist3 = 0;
  TH1D* hist = 0; //will contain the three years combined
  TH1D* histRatio = 0;
  TH1D* histDummy = 0;
  vector<TH1D*> histos; // histograms at fixed Njet, several b-tag multiplicities
  //TTree* myTree;
  TCanvas *c = new TCanvas("c", "c", 600, 600);
  c->SetLogy(); 

  // define the two pads
  c->cd();
  TPad *pad1 = new TPad("pad1","pad1",0,0.3-0.1,1,1);
  pad1->SetBottomMargin(0.15);
  pad1->SetLogy();
  pad1->Draw();
  TPad *pad2 = new TPad("pad2","pad2",0,0,1,0.21);
  pad2->SetTopMargin(0.01);
  pad2->SetBottomMargin(0.15);
  pad2->SetGridy();
  pad2->Draw();

  int htMin_precedingRegion(250);
  int htMax_precedingRegion(450);
  int NjMin_precedingRegion(2);
  int NjMax_precedingRegion(3);


  for(std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR){
   
    MT2Region* thisRegion = new MT2Region(*iR);
  
    //all the three years have the same regions set 
    vector<string> regionName = thisRegion->getNiceNames();
                              
    int htMinInt = floor(thisRegion->htMin());
    string htMin = to_string(htMinInt);
    
    int htMaxInt = floor(thisRegion->htMax());
    string htMax = to_string(htMaxInt);
    
    int NjMinInt = floor(thisRegion->nJetsMin());
    string NjMin = to_string(NjMinInt);
    
    int NjMaxInt = floor(thisRegion->nJetsMax());
    string NjMax = to_string(NjMaxInt);
    
    int NbMinInt = floor(thisRegion->nBJetsMin());
    string NbMin = to_string(NbMinInt);

    int NbMaxInt = floor(thisRegion->nBJetsMax());
    string NbMax = to_string(NbMaxInt);
    string regionFile;

    string name;
    if(doZll){
      name = "zllCR";
    }
    else if(doZinv){
      name = "ZJets";
    }
  
    if(NjMinInt == 2){
	if(NjMaxInt == 6) continue; //we don't do the study on j3to6 since there is only one bin in Nb
      } 

   
    if((NjMinInt != NjMaxInt)){
    //if((htMinInt == 1500) && (htMaxInt == -1) && (NjMinInt == 7) && (NjMaxInt == 9)){
      if(htMax != "-1"){
	if(NjMax != "-1"){
	  if(NbMin==NbMax){
	    regionFile = name + "/HT" + htMin + "to" + htMax + "_j" + NjMin + "to" + NjMax + "_b" + NbMin + "/yield_" + name + "_HT" + htMin + "to" + htMax + "_j" + NjMin + "to" + NjMax + "_b" + NbMin;
	  }
	  else if(NbMax == "-1"){
	    regionFile = name + "/HT" + htMin + "to" + htMax + "_j" + NjMin + "to" + NjMax + "_b" + NbMin + "toInf"  + "/yield_" + name + "_HT" + htMin + "to" + htMax + "_j" + NjMin + "to" + NjMax + "_b" + NbMin + "toInf";
	  }
	}
	else if(NjMax == "-1"){
	  if(NbMin==NbMax){
	    regionFile = name + "/HT" + htMin + "to" + htMax + "_j" + NjMin + "toInf_b" + NbMin + "/yield_" + name + "_HT" + htMin + "to" + htMax + "_j" + NjMin + "toInf_b" + NbMin;
	  }
	  else if(NbMax == "-1"){
	    regionFile = name + "/HT" + htMin + "to" + htMax + "_j" + NjMin + "toInf_b" + NbMin + "toInf"  + "/yield_" + name + "_HT" + htMin + "to" + htMax + "_j" + NjMin + "toInf_b" + NbMin + "toInf";
	  }
	}
      }
      else if (htMax == "-1"){
	if(NjMax != "-1"){
	  if(NbMin==NbMax){
	    regionFile = name + "/HT" + htMin + "toInf_j" + NjMin + "to" + NjMax + "_b" + NbMin + "/yield_" + name + "_HT" + htMin + "toInf_j" + NjMin + "to" + NjMax + "_b" + NbMin;
	  }
	  else if(NbMax == "-1"){
	    regionFile = name + "/HT" + htMin + "toInf_j" + NjMin + "to" + NjMax + "_b" + NbMin + "toInf"  + "/yield_" + name + "_HT" + htMin + "toInf_j" + NjMin + "to" + NjMax + "_b" + NbMin + "toInf";
	  }
	}
	else if(NjMax == "-1"){
	  if(NbMin==NbMax){
	    regionFile = name + "/HT" + htMin + "toInf_j" + NjMin + "toInf_b" + NbMin + "/yield_" + name + "_HT" + htMin + "toInf_j" + NjMin + "toInf_b" + NbMin;
	  }
	  else if(NbMax == "-1"){
	    regionFile = name + "/HT" + htMin + "toInf_j" + NjMin + "toInf_b" + NbMin + "toInf"  + "/yield_" + name + "_HT" + htMin + "toInf_j" + NjMin + "toInf_b" + NbMin + "toInf";
	  }
	}
      }
            
      cout << p << " : " << regionFile << endl;
      
      hist1 = (TH1D*) file1->Get(regionFile.c_str())->Clone("hist1");
      hist2 = (TH1D*) file2->Get(regionFile.c_str())->Clone("hist2");
      hist3 = (TH1D*) file3->Get(regionFile.c_str())->Clone("hist3");
       
      hist = (TH1D*) hist1->Clone("hist");
      hist->Add(hist2);
      hist->Add(hist3);

     
      //check if final histogram is the sum of the three years
      //for(unsigned int iBin(0); iBin<hist->GetSize(); ++iBin){
      //cout << "hist1: " << hist1->GetBinContent(iBin) << endl;
      //cout << "hist2: " << hist2->GetBinContent(iBin) << endl;
      //cout << "hist3: " << hist3->GetBinContent(iBin) << endl;
      //cout << "histTot: " << hist->GetBinContent(iBin) << endl;
      //cout << "check bin " << iBin << ": "  << hist->GetBinContent(iBin) - (hist1->GetBinContent(iBin)+hist2->GetBinContent(iBin)+hist3->GetBinContent(iBin)) << endl;
      //}
      
      //we normalize the histogram
      double integral_hist = hist->Integral();
      if(integral_hist!=0)hist->Scale(1./integral_hist);
      
      TString hi = to_string(htMinInt);
      TString ha = to_string(htMaxInt);
      TString ji = to_string(NjMinInt);
      TString ja = to_string(NjMaxInt);
      TString title = "HT" + hi + "to" + ha + "_j" + ji + "to" + ja;
      hist->SetTitle(title);
      hist->SetLineWidth(3);

      histos.push_back(hist);

      TAxis *Xaxis = hist->GetXaxis();
      TAxis *Yaxis = hist->GetYaxis();
      Xaxis->SetTitle("M_{T2} shape");
      Xaxis->SetTitleSize(0.045);
      Xaxis->SetLabelSize(0.045);
      Xaxis->SetTitleOffset(1.1);

      Yaxis->SetTitleSize(0.045);
      Yaxis->SetLabelSize(0.045);
      Yaxis->SetTitleOffset(1.26);
      Yaxis->SetRangeUser(0.001,1.);

      gStyle->SetOptStat(0);

      histDummy = (TH1D*) hist->Clone();
      histDummy->SetLineColor(kBlack);
      histDummy->SetLineWidth(1);
      for (int iBin=1; iBin<histDummy->GetNbinsX()+2; iBin++){ // also overflow bin to be safe
        histDummy->SetBinContent(iBin,1);
        histDummy->SetBinError(iBin,0);
      }

      histDummy->GetYaxis()->SetRangeUser(0., 2.);
      histDummy->SetTitle("");

      TAxis *Xaxisr = histDummy->GetXaxis();
      TAxis *Yaxisr = histDummy->GetYaxis();
      Xaxisr->SetTitle("");
      Xaxisr->SetTitleSize(0.1);
      Xaxisr->SetLabelSize(0.15);
      Xaxisr->SetTitleOffset(0.3);
      //Xaxisr->SetRangeUser(-1000,1000);
      Yaxisr->SetTitle("Ratio");
      Yaxisr->SetTitleSize(0.15);
      Yaxisr->SetLabelSize(0.15);
      Yaxisr->SetTitleOffset(0.3);
      Yaxisr->SetNdivisions(505);
      
      TString n=to_string(p);
      TString n_previous=to_string(p-1);
      TString nb = to_string(NbMinInt);
      
      
      
      if(p==1){
	tableLeg[legIndex-1] = new TLegend(0.8, 0.7, 0.85, 0.9);
        if(NbMaxInt == -1){
	  tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb + "toInf");
        }
	else{
	  tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb);
        }
	tableLeg[legIndex-1] -> SetTextSize(0.04);
	tableLeg[legIndex-1] -> SetLineColor(0);
	tableLeg[legIndex-1] -> SetFillColorAlpha(0, 0);
	tableLeg[legIndex-1] -> SetBorderSize(0);

        //c->cd();
        pad1->cd();       
	hist->Draw();
        pad2->cd();
        histDummy->Draw();
	tableLeg[legIndex-1]->Draw();
	//c->SaveAs(directoryToSave + n + ".pdf");
	++legIndex;
	++color;
      }

      if(p>1){

        //cout << "debug index_fixed_nJ=" << index_fixed_nJ << " regName=" << iR->getName() << std::endl;
	//cout << "htMin: " << htMin << " htMin_preceding: " << htMin_precedingRegion << endl;
	//cout << "htMax: " << htMax << " htMax_preceding: " << htMax_precedingRegion << endl;
	//cout << "NjMin: " << NjMin << " NjMin_preceding: " << NjMin_precedingRegion << endl;
	//cout << "NjMax: " << NjMax << " NjMax_preceding: " << NjMax_precedingRegion << endl;
	fileIndex = legIndex;
	if(((htMinInt == htMin_precedingRegion) && (htMaxInt == htMax_precedingRegion) && (NjMinInt == NjMin_precedingRegion) && (NjMaxInt == NjMax_precedingRegion))){
	  tableLeg[legIndex-1] = new TLegend(0.8, 0.7-i, 0.85, 0.9-i);
	  if(NbMaxInt == -1){
	    tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb + "toInf");
	  }
	  else{
	    tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb);
	  }
	  tableLeg[legIndex-1] -> SetTextSize(0.04);
	  tableLeg[legIndex-1] -> SetLineColor(0);
	  tableLeg[legIndex-1] -> SetFillColorAlpha(0, 0);
	  hist->SetLineColor(color);
          pad1->cd();
	  hist->Draw("same");

	  for(int k(0); k<legIndex; ++k){
	    tableLeg[k]->Draw("same");
	  }

          pad2->cd();
          histRatio = (TH1D*)histos[index_fixed_nJ-1]->Clone();
          histRatio->Divide(hist);
          histRatio->SetLineColor(hist->GetLineColor());
          pad2->cd();
          histRatio->Draw("same"); 

	  i += 0.15;
	  ++legIndex;
	  //++color;
	  color = color + 2;
	}
       	else{

          index_fixed_nJ = p;

	  color = 2;
	  legIndex = 1;
	  tableLeg[legIndex-1] = new TLegend(0.8, 0.7, 0.85, 0.9);
	  if(NbMaxInt == -1){
	    tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb + "toInf");
	  }
	  else{
	    tableLeg[legIndex-1] -> AddEntry(hist, "b" + nb);
	  }
	  tableLeg[legIndex-1] -> SetTextSize(0.04);
	  tableLeg[legIndex-1] -> SetLineColor(0);
	  tableLeg[legIndex-1] -> SetFillColorAlpha(0, 0);
	  tableLeg[legIndex-1] -> SetBorderSize(0);
	  hist->SetLineColor(color);
          pad1->cd();
	  hist->Draw();
	  tableLeg[legIndex-1]->Draw();

          pad2->cd();
          histDummy->Draw(); 

	  i=0.2;
	  ++legIndex;
	  //++color;
	  color = color + 2;
	  }


	TString thisName = thisRegion->getName();
	c->SaveAs(directoryToSave + n + "_normalized.pdf");
	c->SaveAs(directoryToSave + n + "_normalized.png");
	
	if(legIndex>fileIndex){
	  remove(directoryToSave + n_previous + "_normalized.pdf"); 
	  remove(directoryToSave + n_previous + "_normalized.png"); 
	}
	
      }
      
      htMin_precedingRegion = htMinInt;
      htMax_precedingRegion = htMaxInt;
      NjMin_precedingRegion = NjMinInt;
      NjMax_precedingRegion = NjMaxInt;
      
      ++p; //we increment the index of the region 

       
    }//end of valid region
  
    delete thisRegion;
    
       
  }//end of for on regions

  // delete myTree;
  
  delete c;
  delete hist1;
  delete hist2;
  delete hist3;
  delete hist;
  delete histRatio; 
  delete histDummy; 
}


