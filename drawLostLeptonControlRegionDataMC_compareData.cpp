//This file aims to compare the shape of data between 2016 and 2017 samples. This is in particular implemented in order to get a better understanding of  the discrepancy between MC and data in the lepton pt.

#include <iostream>

#include "TCanvas.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TTree.h"


#include "../interface/MT2Config.h"
#include "../interface/MT2Analysis.h"
#include "../interface/MT2Region.h"
#include "../interface/MT2Estimate.h"
#include "../interface/MT2EstimateTree.h"
#include "../interface/MT2DrawTools.h"

using namespace std;


int main( int argc, char* argv[] ) {



  //////////////////////////////////////////////////////
  //                  DECISION BOARD                  //

  //define ht region
   int htMin_=250, htMax_=-1;
    
   //decide on plotting lepton as electron or muon
   bool plotElectron = false;
   bool plotMuon = false;

   //decide on doing normalization to lumi or not
   bool doNormalization = true;
  //////////////////////////////////////////////////////




  if( argc<2 ) {
    std::cout << "USAGE: ./drawLostLeptonControlRegionDataMC [configFileName1] [configFileName2] [lumi/shape]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

  //load basic draw style
  MT2DrawTools::setStyle();


  //load config files
  std::string configFileName1(argv[1]);
  MT2Config cfg1(configFileName1);
  int year1_ = cfg1.year();
  TString year1 = to_string(year1_);
  double_t lumi1 = cfg1.lumi();
 
  std::string configFileName2(argv[2]);
  MT2Config cfg2(configFileName2);
  int year2_ = cfg2.year();
  TString year2 = to_string(year2_);
  double_t lumi2 = cfg2.lumi();
 
 
  //lumi vs shape
  bool shapeNorm = false;
  if( argc>3 ) {
    std::string normType(argv[3]);
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


  
  //load data file
  std::string dataFile1 = cfg1.getEventYieldDir() + "/llepControlRegion/data.root";
  MT2Analysis<MT2EstimateTree>* data1 = MT2Analysis<MT2EstimateTree>::readFromFile(dataFile1, "llepCR");
 
  std::string dataFile2 = cfg2.getEventYieldDir() + "/llepControlRegion/data.root";
  MT2Analysis<MT2EstimateTree>* data2 = MT2Analysis<MT2EstimateTree>::readFromFile(dataFile2, "llepCR");
 

  //set the regions, and check that the two config files work on the same 
  set<MT2Region> regions = data1->getRegions();
  
  //make sure that the two files that we compare have the same region set
  set<MT2Region> regionsCheck = data2->getRegions();
  if(regions != regionsCheck){
    cout << "You cannot compare files not working on the same region set" << endl; 
    exit(11);
  }
  
  //loop on all the regions
  for(set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR){

    //canvas that will welcome plot
    TCanvas *c1 = new TCanvas();
    c1->SetLogy();
    
    
    //region we are looking at
    MT2Region thisRegion((*iR));
    
    
    //build the corresponding trees for data
    TTree* tree_data1 = data1->get(thisRegion)->tree;
    TTree* tree_data2 = data2->get(thisRegion)->tree;

    
    //build the corresponding histograms
    TH1D* hist_data1 = new TH1D("hist_data1", "", 70, 0, 700);
    hist_data1->SetLineColor(4);
    hist_data1->SetLineWidth(2);

    TH1D* hist_data2 = new TH1D("hist_data2", "", 70, 0, 700);
    hist_data2->SetLineColor(2);
    hist_data2->SetLineWidth(2);


    
    //define ht region
    TString htMin = to_string(htMin_);
    TString htMax = to_string(htMax_);

    
    //make the selection
    TString selection;
    if(plotElectron && !plotMuon){
       selection = "abs(lepID)<12 && ht>" + htMin + " && met>250  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
    }
    else if(plotMuon && !plotElectron){
       selection = "abs(lepID)>12 && ht>" + htMin + " && met>250  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
    }
    else{
       selection = "ht>" + htMin + " && met>250  && nJets>1 && deltaPhiMin>0.3 && diffMetMht<0.5*met && mt2>200.";
    }
      
    if(htMax_ != -1){
       selection += "&& ht<" + htMax;
    }
    
    
    //set the variable we want to plot in the histogram
    TString varName = "lepPt";
    tree_data1->Project("hist_data1", varName, selection);
    //tree_data1->Draw("lepPt>>hist_data1");
    tree_data2->Project("hist_data2", varName, selection);
    //tree_data2->Draw("lepPt>>hist_data2");

    //normalize the histograms to the luminosity
    if(doNormalization){
      hist_data1->Scale(1/lumi1); 
      hist_data2->Scale(1/lumi2);
    }
    
    //we define the histogram of the ratio
    TH1D *hist_dataRatio = (TH1D*)hist_data1->Clone("hist_dataRatio");
    hist_dataRatio->Divide(hist_data2);
   
    
    //build a TGraphAsymmErrors out of the histogram
    TGraphAsymmErrors* gr_data1 = MT2DrawTools::getPoissonGraph(hist_data1, false, "binWidth");
    gr_data1->SetMarkerStyle(20);
    gr_data1->SetMarkerColor(4);
    gr_data1->SetMarkerSize(1.2);
    gr_data1->Draw("AP");

    TGraphAsymmErrors* gr_data2 = MT2DrawTools::getPoissonGraph(hist_data2, false, "binWidth");
    gr_data2->SetMarkerStyle(20);
    gr_data2->SetMarkerColor(2);
    gr_data2->SetMarkerSize(1.2);
    gr_data2->Draw("P");

    TGraphAsymmErrors* gr_dataRatio = MT2DrawTools::getPoissonGraph(hist_dataRatio, false, "binWidth");
    gr_dataRatio->SetMarkerStyle(20);
    gr_dataRatio->SetMarkerColor(2);
    gr_dataRatio->SetMarkerSize(1.2);
    // gr_dataRatio->Draw("AP");

    //build the legend
    TLegend *leg = new TLegend(0.65, 0.7, 0.85, 0.9);
    if(plotElectron){leg -> AddEntry((TObject*)0, "Electron ", "");}
    if(plotMuon){leg -> AddEntry((TObject*)0, "Muon ", "");}
    if(htMax_==-1){
      leg -> AddEntry((TObject*)0, "ht > " + htMin, "");
    }
    else{
      leg -> AddEntry((TObject*)0, htMin + " < ht < " + htMax, "");
    } 
    leg -> AddEntry(gr_data1, "data " + year1, "P");
    leg -> AddEntry(gr_data2, "data " + year2, "P");
    leg -> SetTextSize(0.04);
    leg -> SetLineColor(0);
    leg -> SetFillColor(0);
    leg->SetBorderSize(0);
    leg -> Draw();

    //set the axis
    TAxis *Xaxis = gr_data1->GetXaxis();
    TAxis *Yaxis = gr_data1->GetYaxis();
    Xaxis->SetTitle("Lepton pT [GeV]");
    Xaxis->SetTitleSize(0.045);
    Xaxis->SetLabelSize(0.045);
    Xaxis->SetTitleOffset(1.1);
    Yaxis->SetTitleSize(0.045);
    Yaxis->SetLabelSize(0.045);
    Yaxis->SetTitleOffset(1.26);

    gStyle->SetOptStat(0);

   
    // save the file
    TString plotsDir = cfg1.getEventYieldDir() + "/llepControlRegion/plotsDataMC/dataTodataPlots/";
    if(shapeNorm){
      plotsDir += "shape/";
    }
    else{
      plotsDir += "lumi/";
    }
    
    TString fileName = htMin + "ht" + htMax;
    if(plotElectron && !plotMuon){
      fileName += "_electron";
    }
    else if(plotMuon && !plotElectron){
      fileName += "_muon";
    }
    
    if(doNormalization){
      fileName += "_normalized";
    }
    
    c1->SaveAs(plotsDir + fileName + ".pdf");
    //c1->SaveAs("test.pdf");
  }
}

