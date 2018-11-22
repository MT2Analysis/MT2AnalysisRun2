#include <iostream>
#include <cmath>

#include "TCanvas.h"
#include "TTree.h"
#include "TLegend.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TFile.h"
#include "THStack.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TLine.h"
#include "TPaveText.h"
#include "TString.h"
#include "TChain.h"

using namespace std;

int main(){

  ///////////////////////////////////////////////////
  //                 style settings                //
  ///////////////////////////////////////////////////
 
  // set the TStyle
  TStyle* style = new TStyle("DrawBaseStyle", "");
  gStyle->SetCanvasColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetOptStat(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBottomMargin(0.12);
  gStyle->SetPadLeftMargin(0.12);
  gStyle->cd();
  // For the canvas:
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetCanvasDefH(600); //Height of canvas
  gStyle->SetCanvasDefW(600); //Width of canvas
  gStyle->SetCanvasDefX(0); //POsition on screen
  gStyle->SetCanvasDefY(0);
  // For the Pad:
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadColor(kWhite);
  gStyle->SetPadGridX(false);
  gStyle->SetPadGridY(false);
  gStyle->SetGridColor(0);
  gStyle->SetGridStyle(3);
  gStyle->SetGridWidth(1);
  // For the frame:
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(1);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameFillStyle(0);
  gStyle->SetFrameLineColor(1);
  gStyle->SetFrameLineStyle(1);
  gStyle->SetFrameLineWidth(1);
  // Margins:
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);//0.13);
  gStyle->SetPadLeftMargin(0.15);//0.16);
  gStyle->SetPadRightMargin(0.05);//0.02);
  // For the Global title:
  gStyle->SetOptTitle(0);
  gStyle->SetTitleFont(42);
  gStyle->SetTitleColor(1);
  gStyle->SetTitleTextColor(1);
  gStyle->SetTitleFillColor(10);
  gStyle->SetTitleFontSize(0.05);
  // For the axis titles:
  gStyle->SetTitleColor(1, "XYZ");
  gStyle->SetTitleFont(42, "XYZ");
  gStyle->SetTitleSize(0.05, "XYZ");
  gStyle->SetTitleXOffset(1.15);//0.9);
  gStyle->SetTitleYOffset(1.5); // => 1.15 if exponents
  // For the axis labels:
  gStyle->SetLabelColor(1, "XYZ");
  gStyle->SetLabelFont(42, "XYZ");
  gStyle->SetLabelOffset(0.007, "XYZ");
  gStyle->SetLabelSize(0.045, "XYZ");
  // For the axis:
  gStyle->SetAxisColor(1, "XYZ");
  gStyle->SetStripDecimals(kTRUE);
  gStyle->SetTickLength(0.03, "XYZ");
  gStyle->SetNdivisions(510, "XYZ");
  gStyle->SetPadTickX(1); // To get tick marks on the opposite side of the frame
  gStyle->SetPadTickY(1);
  // for histograms:
  gStyle->SetHistLineColor(1);


  ///////////////////////////////////////////////////
  //                 Get the files                 //
  ///////////////////////////////////////////////////

  int year = 2017;
  if(year==2017){
    cout << "Computation of the ratio R(SF/OF) with the 2017 samples" << endl;
  }
  else if(year==2016){
    cout << "Computation of the ratio R(SF/OF) with the 2016 samples" << endl;
  }


  TChain* SF=new TChain("SF");
  if(year==2017){
    SF->Add("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_41p9ifb_incl_2017/zllControlRegion/ZllPurityTrees.root/Top/HT250toInf_j1toInf_b0toInf/tree_Top_HT250toInf_j1toInf_b0toInf");
    SF->Add("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_41p9ifb_incl_2017/zllControlRegion/ZllPurityTrees.root/DYJets/HT250toInf_j1toInf_b0toInf/tree_DYJets_HT250toInf_j1toInf_b0toInf");
  }
  else if(year==2016){
    SF->Add("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_35p9ifb_incl/zllControlRegion/ZllPurityTrees.root/Top/HT250toInf_j1toInf_b0toInf/tree_Top_HT250toInf_j1toInf_b0toInf");
    SF->Add("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_35p9ifb_incl/zllControlRegion/ZllPurityTrees.root/DYJets/HT250toInf_j1toInf_b0toInf/tree_DYJets_HT250toInf_j1toInf_b0toInf");
  }

  TChain* OF=new TChain("OF");
  if(year==2017){
    OF->Add("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_41p9ifb_incl_2017/zllControlRegion/ZllPurityTrees_of.root/Top/HT250toInf_j1toInf_b0toInf/tree_Top_HT250toInf_j1toInf_b0toInf");
    OF->Add("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_41p9ifb_incl_2017/zllControlRegion/ZllPurityTrees_of.root/DYJets/HT250toInf_j1toInf_b0toInf/tree_DYJets_HT250toInf_j1toInf_b0toInf");
  }
  else  if(year==2016){
    OF->Add("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_35p9ifb_incl/zllControlRegion/ZllPurityTrees_of.root/Top/HT250toInf_j1toInf_b0toInf/tree_Top_HT250toInf_j1toInf_b0toInf");
    OF->Add("/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_dataETH_SnTMC_35p9ifb_incl/zllControlRegion/ZllPurityTrees_of.root/DYJets/HT250toInf_j1toInf_b0toInf/tree_DYJets_HT250toInf_j1toInf_b0toInf");
  }


  gDirectory->cd("");
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  
  cout << "Number of SF entries: " << SF->GetEntries() << endl;
//  cout << SF_top->GetEntries() << endl;
//  cout << SF_dy->GetEntries() << endl;
  cout << "Number of OF entries: " << OF->GetEntries() << endl;




  ///////////////////////////////////////////////////
  //           Correlation of variables            //
  ///////////////////////////////////////////////////
  
  // we want to check here if there is a correlation between cuts and OF
  
  //total number of entries in the tree SF and OF:
  int nEntriesSF = SF->GetEntries();
  int nEntriesOF = OF->GetEntries();

  cout << "Number of entries in SF data tree: " << SF->GetEntries() << endl;
  cout << "Number of entries in OF data tree: " << OF->GetEntries() << endl;

  //we set access to some of the branches
  float Z_pT;
  float Z_mass;
  SF -> SetBranchAddress("Z_pt", &Z_pT);
  SF -> SetBranchAddress("Z_mass", &Z_mass);
  
  float Z_pT_OF;
  float Z_mass_OF;
  OF -> SetBranchAddress("Z_pt", &Z_pT_OF);
  OF -> SetBranchAddress("Z_mass", &Z_mass_OF);


  //we declare 2D histogram that will contain the four bins (OF<, OF>, SF<, SF>)
  TH2D *hist = new TH2D("hist", "hist", 2, 0, 1, 2, 0, 1);
  cout << "Total number of bins of the histogram: " << hist->GetNbinsX() + hist->GetNbinsY() << endl;
  Int_t bin01 = hist->GetBin(0, 1);
  Int_t bin10 = hist->GetBin(1, 0);
  Int_t bin11 = hist->GetBin(1, 1);
  Int_t bin00 = hist->GetBin(0, 0);

  cout << "bin01: " << bin01 << endl;
  cout << "bin10: " << bin10 << endl;
  cout << "bin11: " << bin11 << endl;
  cout << "bin00: " << bin00 << endl;

  //loop on SF
  for(int i=0; i<SF->GetEntries(); ++i){ 
    if(i % 30000 == 0){ 
      cout << "   Entry: " << i << " / " << nEntriesSF << endl;
    }
    
    SF -> GetEntry(i);
  
    if(Z_pT <= 200 && Z_mass >= 50){
      hist->AddBinContent(bin01);
    }
    else{
      hist->AddBinContent(bin11); 
    } 
  }
  
  cout << "bin01 content: " << hist->GetBinContent(bin01) << endl;
  cout << "total bin content SF: " <<  hist->GetBinContent(bin11) +  hist->GetBinContent(bin01) << endl;
  
  
  //loop on OF
  for(int iEntry(0); iEntry<nEntriesOF; ++iEntry){ 
    if(iEntry % 3000 == 0){ 
      cout << "Entry: " << iEntry << " / " << nEntriesOF << endl;
    }

    OF->GetEntry(iEntry);
 
    if(Z_pT_OF<=200 && Z_mass_OF>=50){
      hist->AddBinContent(bin00);
    }
    else{
      hist->AddBinContent(bin10);
    }
  }
   
  cout << "total bin content OF: " <<  hist->GetBinContent(bin00) +  hist->GetBinContent(bin10) << endl;
  

  double correlationFactor = hist->GetCorrelationFactor();
  cout << "Correlation Factor: " << correlationFactor << endl;

 
  

  //plot of normalized Z_pT in OF and SF


  TH1D* hist_ZpTSF = new TH1D("hist_ZpTSF", "hist_ZpTSF", 100, 0, 1500);
  TH1D* hist_ZpTOF = new TH1D("hist_ZpTOF", "hist_ZpTOF", 100, 0, 1500);

  hist_ZpTSF->Sumw2(); 
  hist_ZpTOF->Sumw2(); 

  hist_ZpTSF->SetLineColor(4);
  hist_ZpTSF->SetMarkerColor(4);
  hist_ZpTSF->SetMarkerStyle(7);
  hist_ZpTOF->SetLineColor(2);
  hist_ZpTOF->SetMarkerColor(2);
  hist_ZpTOF->SetMarkerStyle(7);

  hist_ZpTSF->GetXaxis()->SetTitle("Z pT [GeV]");
  hist_ZpTSF->GetYaxis()->SetTitle("Entries");
 
  //SF->Draw("Z_pt >> hist_ZpTSF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight/HLT_weight", "goff");
  //OF->Draw("Z_pt >> hist_ZpTOF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight", "goff");
  SF->Draw("Z_pt >> hist_ZpTSF", "(ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight/HLT_weight", "goff");
  OF->Draw("Z_pt >> hist_ZpTOF", "(ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight", "goff");
  //SF->Draw("Z_pt >> hist_ZpTSF", "", "goff");
  //OF->Draw("Z_pt >> hist_ZpTOF", "", "goff");

  // TH1D* ZpTSF = (TH1D*) hist_ZpTSF->Clone("ZpTSF");

  double integral_ZpTSF = hist_ZpTSF->Integral(1,-1);
  double integral_ZpTOF = hist_ZpTOF->Integral(1,-1);

  hist_ZpTSF->Scale(1/integral_ZpTSF);
  hist_ZpTOF->Scale(1/integral_ZpTOF);

  TLegend *leg = new TLegend(0.75, 0.7, 0.95, 0.9);
  leg -> AddEntry(hist_ZpTSF, "SF", "P");
  leg -> AddEntry(hist_ZpTOF, "OF", "P");
  leg -> SetTextSize(0.04);
  leg -> SetLineColor(0);
  leg -> SetFillColor(0);
  leg -> SetBorderSize(0);
  
  TCanvas *plotZpT = new TCanvas();
  plotZpT->cd();

  // hist_ZpTOF->Draw();
  hist_ZpTSF->Draw("same");
  hist_ZpTOF->Draw("same");
  leg -> Draw();
  plotZpT -> SaveAs("EventYields_dataETH_SnTMC_35p9ifb_incl/plotsZllEstimates/correlation/plotZpT_withCut.pdf");
  


  
  //we repeat the same with Z_mass

  TH1D* hist_ZMassSF = new TH1D("hist_ZMassSF", "hist_ZMassSF", 33, 0, 500);
  TH1D* hist_ZMassOF = new TH1D("hist_ZMassOF", "hist_ZMassOF", 33, 0, 500);

  hist_ZMassSF->Sumw2(); 
  hist_ZMassOF->Sumw2(); 

  hist_ZMassSF->SetLineColor(4);
  hist_ZMassSF->SetMarkerColor(4);
  hist_ZMassSF->SetMarkerStyle(7);
  hist_ZMassOF->SetLineColor(4);
  hist_ZMassOF->SetMarkerColor(2);
  hist_ZMassOF->SetMarkerStyle(7);

  hist_ZMassSF->GetXaxis()->SetTitle("Z Mass [GeV]");
  hist_ZMassSF->GetYaxis()->SetTitle("Entries");
 
  //SF->Draw("Z_mass >> hist_ZMassSF", "", "goff");
  //OF->Draw("Z_mass >> hist_ZMassOF", "", "goff");
  SF->Draw("Z_mass >> hist_ZMassSF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight/HLT_weight", "goff");
  OF->Draw("Z_mass >> hist_ZMassOF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight", "goff");
  //SF->Draw("Z_mass >> hist_ZMassSF", "(ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight/HLT_weight", "goff");
  //OF->Draw("Z_mass >> hist_ZMassOF", "(ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight", "goff");

  // TH1D* ZpTSF = (TH1D*) hist_ZpTSF->Clone("ZpTSF");

  double integral_ZMassSF = hist_ZMassSF->Integral(1,-1);
  double integral_ZMassOF = hist_ZMassOF->Integral(1,-1);

  hist_ZMassSF->Scale(1/integral_ZMassSF);
  hist_ZMassOF->Scale(1/integral_ZMassOF);

  TLegend *legMass = new TLegend(0.65, 0.7, 0.85, 0.9);
  legMass -> AddEntry(hist_ZMassSF, "SF", "P");
  legMass -> AddEntry(hist_ZMassOF, "OF", "P");
  legMass -> SetTextSize(0.04);
  legMass -> SetLineColor(0);
  legMass -> SetFillColor(0);
  legMass -> SetBorderSize(0);
  
  TCanvas *plotZMass = new TCanvas();
  plotZMass->cd();

  //hist_ZMassOF->Draw();
  hist_ZMassSF->Draw("same");
  hist_ZMassOF->Draw("same");
  legMass -> Draw();
  plotZMass -> SaveAs("EventYields_dataETH_SnTMC_35p9ifb_incl/plotsZllEstimates/correlation/plotZMass_withFullCut.pdf");







  bool computeRatio = false;
  if(computeRatio){
  
  ///////////////////////////////////////////////////
  //      ratio as a function of mll (Z mass)      //
  ///////////////////////////////////////////////////


  float bins_mll[]={50,60,71.20,80,90,100,111.20,120,130,140,150,160,170,180,200};
  TH1D* mllSF = new TH1D("mllSF", "", 14, bins_mll);
  mllSF->Sumw2();

  TH1D* mllOF = new TH1D("mllOF", "", 14, bins_mll);
  mllOF->Sumw2();

  mllSF->SetLineColor(4);
  mllSF->SetMarkerColor(4);
  mllSF->SetMarkerStyle(4);
  mllOF->SetLineColor(2);
  mllOF->SetMarkerColor(2);
  mllOF->SetMarkerStyle(20);

  mllSF->GetXaxis()->SetTitle("M_{ll} [GeV]");
  mllOF->GetXaxis()->SetTitle("M_{ll} [GeV]");
  mllSF->GetYaxis()->SetTitle("Entries");
  mllOF->GetYaxis()->SetTitle("Entries");


  SF->Draw("Z_mass>>mllSF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight/HLT_weight", "goff");
  OF->Draw("Z_mass>>mllOF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight", "goff");

  mllSF->Scale(41.9);
  mllOF->Scale(41.9);
  
  double integral_sf, error_sf;
  double integral_of, error_of;
  
  integral_sf = mllSF->IntegralAndError(1,-1, error_sf);
  integral_of = mllOF->IntegralAndError(1,-1, error_of);
//  error_sf = TMath::Sqrt(integral_sf);
//  error_of = TMath::Sqrt(integral_of);
  
  cout << "[mll] R(SF/OF) = ("<<integral_sf<<" +- "<<error_sf<<")/("<<integral_of<<" +- "<<error_of<<")"<<endl;
  double RSFOF = integral_sf/integral_of;
  double err_R = sqrt( (error_sf/integral_of)*(error_sf/integral_of) + (error_of*integral_sf/(integral_of*integral_of))*(error_of*integral_sf/(integral_of*integral_of)) );
  cout << "[mll] R(SF/OF) = "<< RSFOF << "+-" << err_R << endl;


  TCanvas* c1=new TCanvas("c1", "", 600, 600);
  c1->cd();
  mllSF->Draw("PE");
  mllOF->Draw("PE,same");
  

  TH1D* mllRSFOF = (TH1D*) mllSF->Clone("mllRSFOF");
  mllRSFOF->Divide(mllOF);

  mllRSFOF->SetMinimum(0.);
  mllRSFOF->SetMaximum(2.);
  
  mllRSFOF->GetYaxis()->SetRangeUser(1.,3.);
  mllRSFOF->GetYaxis()->SetTitle("R(SF/OF)");

  TCanvas* c1r=new TCanvas("c1r", "", 600, 600);
  c1r->cd();

  mllRSFOF->Draw("PE");
  mllRSFOF->Fit("pol0");
  
  TLine* lr = new TLine(50, (float) RSFOF, 200, (float)RSFOF);
  TLine* lrup = new TLine(50, (float)(RSFOF+err_R), 200, (float)(RSFOF+err_R));
  TLine* lrdn = new TLine(50, (float)(RSFOF-err_R), 200, (float)(RSFOF-err_R));

  lr->SetLineColor(1);
  lrup->SetLineColor(1);
  lrdn->SetLineColor(1);
  lrup->SetLineStyle(2);
  lrdn->SetLineStyle(2);
  
  lr->Draw("same");
  lrup->Draw("same");
  lrdn->Draw("same");


  ///////////////////////////////////////////////////
  //         ratio as a function of HT             //
  ///////////////////////////////////////////////////  

 
  float bins_ht[]={250,450,575,1000,2000};
  
  TH1D* htSF = new TH1D("htSF", "", 4, bins_ht);
  htSF->Sumw2();

  TH1D* htOF = new TH1D("htOF", "", 4, bins_ht);
  htOF->Sumw2();

  htSF->SetLineColor(4);
  htSF->SetMarkerColor(4);
  htSF->SetMarkerStyle(4);
  htOF->SetLineColor(2);
  htOF->SetMarkerColor(2);
  htOF->SetMarkerStyle(20);

  htSF->GetXaxis()->SetTitle("H_{T} [GeV]");
  htOF->GetXaxis()->SetTitle("H_{T} [GeV]");
  htSF->GetYaxis()->SetTitle("Entries");
  htOF->GetYaxis()->SetTitle("Entries");


  SF->Draw("ht>>htSF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. )*weight/HLT_weight", "goff");
  OF->Draw("ht>>htOF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. )*weight", "goff");
  
  htSF->Scale(41.9);
  htOF->Scale(41.9);
 

  integral_sf = htSF->IntegralAndError(1,-1, error_sf);
  integral_of = htOF->IntegralAndError(1,-1, error_of);
  
  cout << "[HT] R(SF/OF) = ("<<integral_sf<<" +- "<<error_sf<<")/("<<integral_of<<" +- "<<error_of<<")"<<endl;
  RSFOF = integral_sf/integral_of;
  err_R = sqrt( (error_sf/integral_of)*(error_sf/integral_of) + (error_of*integral_sf/(integral_of*integral_of))*(error_of*integral_sf/(integral_of*integral_of)) );
  cout << "[HT] R(SF/OF) = "<< RSFOF << "+-" << err_R << endl;

  TCanvas* c2=new TCanvas("c2", "", 600, 600);
  c2->cd();
  htSF->Draw("PE");
  htOF->Draw("PE,same");
  
  TH1D* htRSFOF = (TH1D*) htSF->Clone("htRSFOF");
  htRSFOF->Divide(htOF);

  htRSFOF->SetMinimum(0.);
  htRSFOF->SetMaximum(2.);
  
  htRSFOF->GetYaxis()->SetRangeUser(1.,3.);
  htRSFOF->GetYaxis()->SetTitle("R(SF/OF)");

  TCanvas* c2r=new TCanvas("c2r", "", 600, 600);
  c2r->cd();

  htRSFOF->Draw("PE");
  htRSFOF->Fit("pol0");

  TLine* lrht = new TLine(250, (float) RSFOF, 2000, (float)RSFOF);
  TLine* lrhtup = new TLine(250, (float)(RSFOF+err_R), 2000, (float)(RSFOF+err_R));
  TLine* lrhtdn = new TLine(250, (float)(RSFOF-err_R), 2000, (float)(RSFOF-err_R));

  lrht->SetLineColor(1);
  lrhtup->SetLineColor(1);
  lrhtdn->SetLineColor(1);
  lrhtup->SetLineStyle(2);
  lrhtdn->SetLineStyle(2);
  
  lrht->Draw("same");
  lrhtup->Draw("same");
  lrhtdn->Draw("same");


  ///////////////////////////////////////////////////
  //         ratio as a function of Nj             //
  ///////////////////////////////////////////////////  

  float bins_nj[]={1,2,4,7,10};
  
  TH1D* njSF = new TH1D("njSF", "", 4, bins_nj);
  njSF->Sumw2();

  TH1D* njOF = new TH1D("njOF", "", 4, bins_nj);
  njOF->Sumw2();

  njSF->SetLineColor(4);
  njSF->SetMarkerColor(4);
  njSF->SetMarkerStyle(4);
  njOF->SetLineColor(2);
  njOF->SetMarkerColor(2);
  njOF->SetMarkerStyle(20);

  njSF->GetXaxis()->SetTitle("N_{j}");
  njOF->GetXaxis()->SetTitle("N_{j}");
  njSF->GetYaxis()->SetTitle("Entries");
  njOF->GetYaxis()->SetTitle("Entries");


  SF->Draw("nJets>>njSF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. )*weight/HLT_weight", "goff");
  OF->Draw("nJets>>njOF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. )*weight", "goff");
  
  njSF->Scale(41.9);
  njOF->Scale(41.9);
  

  integral_sf = njSF->IntegralAndError(1,-1, error_sf);
  integral_of = njOF->IntegralAndError(1,-1, error_of);
  
  cout << "[Nj] R(SF/OF) = ("<<integral_sf<<" +- "<<error_sf<<")/("<<integral_of<<" +- "<<error_of<<")"<<endl;
  RSFOF = integral_sf/integral_of;
  err_R = sqrt( (error_sf/integral_of)*(error_sf/integral_of) + (error_of*integral_sf/(integral_of*integral_of))*(error_of*integral_sf/(integral_of*integral_of)) );
  cout << "[Nj] R(SF/OF) = "<< RSFOF << "+-" << err_R << endl;

  TCanvas* c3=new TCanvas("c3", "", 600, 600);
  c3->cd();
  njSF->Draw("PE");
  njOF->Draw("PE,same");
  
  TH1D* njRSFOF = (TH1D*) njSF->Clone("njRSFOF");
  njRSFOF->Divide(njOF);

  njRSFOF->SetMinimum(0.);
  njRSFOF->SetMaximum(2.);
  
  njRSFOF->GetYaxis()->SetRangeUser(1.,3.);
  njRSFOF->GetYaxis()->SetTitle("R(SF/OF)");

  TCanvas* c3r=new TCanvas("c3r", "", 600, 600);
  c3r->cd();

  njRSFOF->Draw("PE");
  njRSFOF->Fit("pol0");

  TLine* lrnj = new TLine(1, (float) RSFOF, 10, (float)RSFOF);
  TLine* lrnjup = new TLine(1, (float)(RSFOF+err_R), 10, (float)(RSFOF+err_R));
  TLine* lrnjdn = new TLine(1, (float)(RSFOF-err_R), 10, (float)(RSFOF-err_R));

  lrnj->SetLineColor(1);
  lrnjup->SetLineColor(1);
  lrnjdn->SetLineColor(1);
  lrnjup->SetLineStyle(2);
  lrnjdn->SetLineStyle(2);
  
  lrnj->Draw("same");
  lrnjup->Draw("same");
  lrnjdn->Draw("same");



  ///////////////////////////////////////////////////
  //         ratio as a function of Nb             //
  /////////////////////////////////////////////////// 
  
  float bins_nb[]={0,1,2,3,4};
  
  TH1D* nbSF = new TH1D("nbSF", "", 4, bins_nb);
  nbSF->Sumw2();

  TH1D* nbOF = new TH1D("nbOF", "", 4, bins_nb);
  nbOF->Sumw2();

  nbSF->SetLineColor(4);
  nbSF->SetMarkerColor(4);
  nbSF->SetMarkerStyle(4);
  nbOF->SetLineColor(2);
  nbOF->SetMarkerColor(2);
  nbOF->SetMarkerStyle(20);

  nbSF->GetXaxis()->SetTitle("N_{b}");
  nbOF->GetXaxis()->SetTitle("N_{b}");
  nbSF->GetYaxis()->SetTitle("Entries");
  nbOF->GetYaxis()->SetTitle("Entries");


  SF->Draw("nBJets>>nbSF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight/HLT_weight", "goff");
  OF->Draw("nBJets>>nbOF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight", "goff");
  
  nbSF->Scale(41.9);
  nbOF->Scale(41.9);
  

  integral_sf = nbSF->IntegralAndError(1,-1, error_sf);
  integral_of = nbOF->IntegralAndError(1,-1, error_of);
  
  cout << "[Nb] R(SF/OF) = ("<<integral_sf<<" +- "<<error_sf<<")/("<<integral_of<<" +- "<<error_of<<")"<<endl;
  RSFOF = integral_sf/integral_of;
  err_R = sqrt( (error_sf/integral_of)*(error_sf/integral_of) + (error_of*integral_sf/(integral_of*integral_of))*(error_of*integral_sf/(integral_of*integral_of)) );
  cout << "[Nb] R(SF/OF) = "<< RSFOF << "+-" << err_R << endl;

  TCanvas* c4=new TCanvas("c4", "", 600, 600);
  c4->cd();
  nbSF->Draw("PE");
  nbOF->Draw("PE,same");
  
  TH1D* nbRSFOF = (TH1D*) nbSF->Clone("nbRSFOF");
  nbRSFOF->Divide(nbOF);

  nbRSFOF->SetMinimum(0.);
  nbRSFOF->SetMaximum(2.);
  
  nbRSFOF->GetYaxis()->SetRangeUser(1.,3.);
  nbRSFOF->GetYaxis()->SetTitle("R(SF/OF)");

  TCanvas* c4r=new TCanvas("c4r", "", 600, 600);
  c4r->cd();

  nbRSFOF->Draw("PE");
  nbRSFOF->Fit("pol0");

  TLine* lrnb   = new TLine(0, (float) RSFOF, 4, (float)RSFOF);
//  TLine* lrnbup = new TLine(0, (float)(RSFOF+err_R), 4, (float)(RSFOF+err_R));
//  TLine* lrnbdn = new TLine(0, (float)(RSFOF-err_R), 4, (float)(RSFOF-err_R));

  TLine* lrnbup = new TLine(0, (float)(RSFOF+0.15), 4, (float)(RSFOF+0.15));
  TLine* lrnbdn = new TLine(0, (float)(RSFOF-0.15), 4, (float)(RSFOF-0.15));

  lrnb->SetLineColor(1);
  lrnbup->SetLineColor(2);
  lrnbdn->SetLineColor(2);
  lrnbup->SetLineStyle(2);
  lrnbdn->SetLineStyle(2);
  
  lrnb->Draw("same");
  lrnbup->Draw("same");
  lrnbdn->Draw("same");


  ///////////////////////////////////////////////////
  //         ratio as a function of MT2            //
  ///////////////////////////////////////////////////

  float bins_mt2[]={200,300,400,600,1500};
  
  TH1D* mt2SF = new TH1D("mt2SF", "", 4, bins_mt2);
  mt2SF->Sumw2();

  TH1D* mt2OF = new TH1D("mt2OF", "", 4, bins_mt2);
  mt2OF->Sumw2();

  mt2SF->SetLineColor(4);
  mt2SF->SetMarkerColor(4);
  mt2SF->SetMarkerStyle(4);
  mt2OF->SetLineColor(2);
  mt2OF->SetMarkerColor(2);
  mt2OF->SetMarkerStyle(20);

  mt2SF->GetXaxis()->SetTitle("M_{T2}");
  mt2OF->GetXaxis()->SetTitle("M_{T2}");
  mt2SF->GetYaxis()->SetTitle("Entries");
  mt2OF->GetYaxis()->SetTitle("Entries");


  SF->Draw("mt2>>mt2SF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. )*weight/HLT_weight", "goff");
  OF->Draw("mt2>>mt2OF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. )*weight", "goff");
  
  mt2SF->Scale(41.9);
  mt2OF->Scale(41.9);
  

  integral_sf = mt2SF->IntegralAndError(1,-1, error_sf);
  integral_of = mt2OF->IntegralAndError(1,-1, error_of);
  
  cout << "[MT2] R(SF/OF) = ("<<integral_sf<<" +- "<<error_sf<<")/("<<integral_of<<" +- "<<error_of<<")"<<endl;
  RSFOF = integral_sf/integral_of;
  err_R = sqrt( (error_sf/integral_of)*(error_sf/integral_of) + (error_of*integral_sf/(integral_of*integral_of))*(error_of*integral_sf/(integral_of*integral_of)) );
  cout << "[MT2] R(SF/OF) = "<< RSFOF << "+-" << err_R << endl;

  TCanvas* c5=new TCanvas("c5", "", 600, 600);
  c5->cd();
  mt2SF->Draw("PE");
  mt2OF->Draw("PE,same");
  
  TH1D* mt2RSFOF = (TH1D*) mt2SF->Clone("mt2RSFOF");
  mt2RSFOF->Divide(mt2OF);

  mt2RSFOF->SetMinimum(0.);
  mt2RSFOF->SetMaximum(2.);
  
  mt2RSFOF->GetYaxis()->SetRangeUser(1.,3.);
  mt2RSFOF->GetYaxis()->SetTitle("R(SF/OF)");

  TCanvas* c5r=new TCanvas("c5r", "", 600, 600);
  c5r->cd();

  mt2RSFOF->Draw("PE");
  mt2RSFOF->Fit("pol0");

  TLine* lrmt2   = new TLine(200, (float) RSFOF,        1500., (float)RSFOF);
  TLine* lrmt2up = new TLine(200, (float)(RSFOF+err_R), 1500., (float)(RSFOF+err_R));
  TLine* lrmt2dn = new TLine(200, (float)(RSFOF-err_R), 1500., (float)(RSFOF-err_R));

  lrmt2->SetLineColor(1);
  lrmt2up->SetLineColor(1);
  lrmt2dn->SetLineColor(1);
  lrmt2up->SetLineStyle(2);
  lrmt2dn->SetLineStyle(2);
  
  lrmt2->Draw("same");
  lrmt2up->Draw("same");
  lrmt2dn->Draw("same");

  /*
  // dR(l1,l2)
  TH1D* drllSF = new TH1D("drllSF", "", 25,0,5);
  drllSF->Sumw2();

  TH1D* drllOF = new TH1D("drllOF", "", 25,0,5);
  drllOF->Sumw2();

  drllSF->SetLineColor(4);
  drllSF->SetMarkerColor(4);
  drllSF->SetMarkerStyle(4);
  drllOF->SetLineColor(2);
  drllOF->SetMarkerColor(2);
  drllOF->SetMarkerStyle(20);

  drllSF->GetXaxis()->SetTitle("#Delta R(l_{1}, l_{2})");
  drllOF->GetXaxis()->SetTitle("#Delta R(l_{1}, l_{2})");
  drllSF->GetYaxis()->SetTitle("Entries");
  drllOF->GetYaxis()->SetTitle("Entries");

  //  SF->Draw("TMath::Sqrt( (lep_eta0-lep_eta1)*(lep_eta0-lep_eta1) + ((lep_phi0-lep_phi1)>TMath::Pi() ? lep_phi0-lep_phi1-2*TMath::Pi() : ((lep_phi0-lep_phi1)<=-TMath::Pi() ? lep_phi0-lep_phi1+2*TMath::Pi() : lep_phi0-lep_phi1) )*((lep_phi0-lep_phi1)>TMath::Pi() ? lep_phi0-lep_phi1-2*TMath::Pi() : ((lep_phi0-lep_phi1)<=-TMath::Pi() ? lep_phi0-lep_phi1+2*TMath::Pi() : lep_phi0-lep_phi1) ) )>>drllSF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100.  && (lep_eta0<1.4 || lep_eta0>1.6) && (lep_eta1<1.4 || lep_eta1>1.6))*weight/HLT_weight", "goff");
  //  OF->Draw("TMath::Sqrt( (lep_eta0-lep_eta1)*(lep_eta0-lep_eta1) + ((lep_phi0-lep_phi1)>TMath::Pi() ? lep_phi0-lep_phi1-2*TMath::Pi() : ((lep_phi0-lep_phi1)<=-TMath::Pi() ? lep_phi0-lep_phi1+2*TMath::Pi() : lep_phi0-lep_phi1) )*((lep_phi0-lep_phi1)>TMath::Pi() ? lep_phi0-lep_phi1-2*TMath::Pi() : ((lep_phi0-lep_phi1)<=-TMath::Pi() ? lep_phi0-lep_phi1+2*TMath::Pi() : lep_phi0-lep_phi1) ) )>>drllOF", "(Z_pt<=200 && Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100.  && (lep_eta0<1.4 || lep_eta0>1.6) && (lep_eta1<1.4 || lep_eta1>1.6))*weight", "goff");

  SF->Draw("TMath::Sqrt( (lep_eta0-lep_eta1)*(lep_eta0-lep_eta1) + ((lep_phi0-lep_phi1)>TMath::Pi() ? lep_phi0-lep_phi1-2*TMath::Pi() : ((lep_phi0-lep_phi1)<=-TMath::Pi() ? lep_phi0-lep_phi1+2*TMath::Pi() : lep_phi0-lep_phi1) )*((lep_phi0-lep_phi1)>TMath::Pi() ? lep_phi0-lep_phi1-2*TMath::Pi() : ((lep_phi0-lep_phi1)<=-TMath::Pi() ? lep_phi0-lep_phi1+2*TMath::Pi() : lep_phi0-lep_phi1) ) )>>drllSF", "(Z_mass>50. && (Z_mass<=71.19 || Z_mass>111.19) && Z_pt<=200)*weight/HLT_weight", "goff");
  OF->Draw("TMath::Sqrt( (lep_eta0-lep_eta1)*(lep_eta0-lep_eta1) + ((lep_phi0-lep_phi1)>TMath::Pi() ? lep_phi0-lep_phi1-2*TMath::Pi() : ((lep_phi0-lep_phi1)<=-TMath::Pi() ? lep_phi0-lep_phi1+2*TMath::Pi() : lep_phi0-lep_phi1) )*((lep_phi0-lep_phi1)>TMath::Pi() ? lep_phi0-lep_phi1-2*TMath::Pi() : ((lep_phi0-lep_phi1)<=-TMath::Pi() ? lep_phi0-lep_phi1+2*TMath::Pi() : lep_phi0-lep_phi1) ) )>>drllOF", "(Z_mass>50. && (Z_mass<=71.19 || Z_mass>111.19) && Z_pt<=200)*weight", "goff");

  drllSF->Scale(41.9);
  drllOF->Scale(41.9);


  TCanvas* c6=new TCanvas("c6", "", 600, 600);
  c6->cd();
  drllSF->Draw("PE");
  drllOF->Draw("PE,same");

  TH1D* drllRSFOF = (TH1D*) drllSF->Clone("drllRSFOF");
  drllRSFOF->Divide(drllOF);

  drllRSFOF->SetMinimum(0.);
  drllRSFOF->SetMaximum(2.);

  drllRSFOF->GetYaxis()->SetRangeUser(0.,2.);
  drllRSFOF->GetYaxis()->SetTitle("R(SF/OF)");

  TCanvas* c6r=new TCanvas("c6r", "", 600, 600);
  c6r->cd();

  drllRSFOF->Draw("PE");

  TLine* lrdrll   = new TLine(0, (float) RSFOF,        5, (float)RSFOF);
  TLine* lrdrllup = new TLine(0, (float)(RSFOF+err_R), 5, (float)(RSFOF+err_R));
  TLine* lrdrlldn = new TLine(0, (float)(RSFOF-err_R), 5, (float)(RSFOF-err_R));

  lrdrll->SetLineColor(1);
  lrdrllup->SetLineColor(1);
  lrdrlldn->SetLineColor(1);
  lrdrllup->SetLineStyle(2);
  lrdrlldn->SetLineStyle(2);

  lrdrll->Draw("same");
  lrdrllup->Draw("same");
  lrdrlldn->Draw("same");


  //ZpT
  //  float bins_zpt[]={0,100,200,300,400,1500};
  float bins_zpt[]={0,200,1500};
  
  TH1D* zptSF = new TH1D("zptSF", "", 2, bins_zpt);
  zptSF->Sumw2();

  TH1D* zptOF = new TH1D("zptOF", "", 2, bins_zpt);
  zptOF->Sumw2();

  zptSF->SetLineColor(4);
  zptSF->SetMarkerColor(4);
  zptSF->SetMarkerStyle(4);
  zptOF->SetLineColor(2);
  zptOF->SetMarkerColor(2);
  zptOF->SetMarkerStyle(20);

  zptSF->GetXaxis()->SetTitle("p_{T}(ll)");
  zptOF->GetXaxis()->SetTitle("p_{T}(ll)");
  zptSF->GetYaxis()->SetTitle("Entries");
  zptOF->GetYaxis()->SetTitle("Entries");


  SF->Draw("Z_pt>>zptSF", "(Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight/HLT_weight", "goff");
  OF->Draw("Z_pt>>zptOF", "(Z_mass>=50 && (Z_mass<=71.19 || Z_mass>111.19) && ht>=250 && mt2>200 && lep_pt0>100. && lep_pt1>30.)*weight", "goff");
  
  zptSF->Scale(41.9);
  zptOF->Scale(41.9);
  

  integral_sf = zptSF->IntegralAndError(1,-1, error_sf);
  integral_of = zptOF->IntegralAndError(1,-1, error_of);
  
  cout << "[ZpT] R(SF/OF) = ("<<integral_sf<<" +- "<<error_sf<<")/("<<integral_of<<" +- "<<error_of<<")"<<endl;
  RSFOF = integral_sf/integral_of;
  err_R = sqrt((error_sf/integral_of)*(error_sf/integral_of) + (error_of*integral_sf/(integral_of*integral_of))*(error_of*integral_sf/(integral_of*integral_of)) );
  cout << "[ZpT] R(SF/OF) = "<< RSFOF << "+-" << err_R << endl;

  TCanvas* c7=new TCanvas("c7", "", 600, 600);
  c7->cd();
  zptSF->Draw("PE");
  zptOF->Draw("PE,same");
  
  TH1D* zptRSFOF = (TH1D*) zptSF->Clone("zptRSFOF");
  zptRSFOF->Divide(zptOF);

  zptRSFOF->SetMinimum(0.);
  zptRSFOF->SetMaximum(2.);
  
  zptRSFOF->GetYaxis()->SetRangeUser(0.,2.);
  zptRSFOF->GetYaxis()->SetTitle("R(SF/OF)");

  TCanvas* c7r=new TCanvas("c7r", "", 600, 600);
  c7r->cd();

  zptRSFOF->Draw("PE");
  //  zptRSFOF->Fit("pol0");

  TLine* lrzpt   = new TLine(0, (float) RSFOF,        1500., (float)RSFOF);
//  TLine* lrzptup = new TLine(0, (float)(RSFOF+err_R), 1500., (float)(RSFOF+err_R));
//  TLine* lrzptdn = new TLine(0, (float)(RSFOF-err_R), 1500., (float)(RSFOF-err_R));

  TLine* lrzptup = new TLine(0, (float)(RSFOF+0.15), 1500., (float)(RSFOF+0.15));
  TLine* lrzptdn = new TLine(0, (float)(RSFOF-0.15), 1500., (float)(RSFOF-0.15));

  lrzpt->SetLineColor(1);
//  lrzptup->SetLineColor(1);
//  lrzptdn->SetLineColor(1);
  lrzptup->SetLineColor(2);
  lrzptdn->SetLineColor(2);
  lrzptup->SetLineStyle(2);
  lrzptdn->SetLineStyle(2);
  
  lrzpt->Draw("same");
  lrzptup->Draw("same");
  lrzptdn->Draw("same");
  */


  // Save As:
  TString directoryName = "EventYields_dataETH_SnTMC_41p9ifb_incl_2017/zllControlRegion/RSFOF_mc/";

  c1 ->SaveAs(directoryName + "SFandOF_mll.pdf");
  c1r->SaveAs(directoryName + "RSFOF_mll.pdf");
  c1 ->SaveAs(directoryName + "SFandOF_mll.png");
  c1r->SaveAs(directoryName + "RSFOF_mll.png");

  c2 ->SaveAs(directoryName + "SFandOF_ht.pdf");
  c2r->SaveAs(directoryName + "RSFOF_ht.pdf");
  c2 ->SaveAs(directoryName + "SFandOF_ht.png");
  c2r->SaveAs(directoryName + "RSFOF_ht.png");

  c3 ->SaveAs(directoryName + "SFandOF_nj.pdf");
  c3r->SaveAs(directoryName + "RSFOF_nj.pdf");
  c3 ->SaveAs(directoryName + "SFandOF_nj.png");
  c3r->SaveAs(directoryName + "RSFOF_nj.png");

  c4 ->SaveAs(directoryName + "SFandOF_nb.pdf");
  c4r->SaveAs(directoryName + "RSFOF_nb.pdf");
  c4 ->SaveAs(directoryName + "SFandOF_nb.png");
  c4r->SaveAs(directoryName + "RSFOF_nb.png");

  c5 ->SaveAs(directoryName + "SFandOF_mt2.pdf");
  c5r->SaveAs(directoryName + "RSFOF_mt2.pdf");
  c5 ->SaveAs(directoryName + "SFandOF_mt2.png");
  c5r->SaveAs(directoryName + "RSFOF_mt2.png");
  }

  return 0;
}
