// Script to check estimates for full run2 stats with two methods
// old method, all separate
// new method, all together
// ./checkLlepEstimates

#include <fstream>

#include "TString.h"
#include "TCanvas.h"
#include "TF1.h"

#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"

int main( ) {

  // new file /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/2016_compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/llepControlRegion/data.root
  // old file /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/old_mt2analysis/analysis/EventYields_dataETH_SnTMC_35p9ifb/llepControlRegion/data.root
  float thr = 0.05; // 5 % -> 0.05

  std::string dir16 = "./EventYields_moriond2019_zurich2016_35p9ifb/";
  std::string dir17 = "./EventYields_moriond2019_zurich2016_41p9ifb_2017/";
  std::string dir18 = "./EventYields_moriond2019_zurich2016_59p9ifb_2018/";

  // new way of calculating bkg
  MT2Analysis<MT2Estimate>* newEst    = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "llepEstimateCombined.root", "llepEstimate16" );
  MT2Analysis<MT2Estimate>* newEst17  = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "llepEstimateCombined.root", "llepEstimate17" );
  MT2Analysis<MT2Estimate>* newEst18  = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "llepEstimateCombined.root", "llepEstimate18" );
  (*newEst) += (*(newEst17));
  (*newEst) += (*(newEst18));

  // old way of calculating bkg
  MT2Analysis<MT2Estimate>* oldEst  = MT2Analysis<MT2Estimate>::readFromFile( dir16 + "llepEstimate.root", "llepEstimate" );
  MT2Analysis<MT2Estimate>* oldEst17  = MT2Analysis<MT2Estimate>::readFromFile( dir17 + "llepEstimate.root", "llepEstimate" );
  MT2Analysis<MT2Estimate>* oldEst18  = MT2Analysis<MT2Estimate>::readFromFile( dir18 + "llepEstimate.root", "llepEstimate" );
  (*oldEst) += (*(oldEst17));
  (*oldEst) += (*(oldEst18));

  // got the estimates
  std::set<MT2Region> regions = newEst->getRegions();
  // TODO: check that the regions are the same bw new and old
  std::cout << "Defined Regions " << std::endl;

  // a plot to look overall at the differences
  TH1D* hDiff = new TH1D("hDiff", "hDiff", 100,-50., 50.);

  // loop over topological regions
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    std::cout << "Working on topo region " << iR->getName() << std::endl;
  
    TH1D* this_new_TopoYield = newEst->get(*iR)->yield;
    TH1D* this_old_TopoYield = oldEst->get(*iR)->yield;

    for( int iBin=1; iBin<this_new_TopoYield->GetNbinsX()+1; ++iBin ) {

      double this_new_yield = this_new_TopoYield->GetBinContent(iBin);
      double this_old_yield = this_old_TopoYield->GetBinContent(iBin);
      //std::cout << "debug new=" << this_new_yield << " old=" << this_old_yield << std::endl;
      double diff = -999;
      if (this_old_yield == 0 && this_new_yield==0) diff=0;
      else if(this_old_yield ==0 && this_new_yield!=0) diff=999.99;
      else diff=this_new_yield/this_old_yield - 1;
      hDiff->Fill(diff*100);
      //if( isnan(this_old_yield) || isnan(this_new_yield)) std::cout << "WARNING, found nan " << std::endl;

      //if (abs(diff) >= thr){
      //  std::cout << "Found difference in iBin=["  << this_new_TopoYield->GetBinLowEdge(iBin) << ","
      //                                             << this_new_TopoYield->GetBinLowEdge(iBin+1) << "]" << std::endl;

      //  std::cout << "New="  << this_new_yield << " Old="  << this_old_yield << std::endl;
      //  std::cout << "  Diff=" << diff*100 << "%" << std::endl;
      //}
      if (abs(diff) >= thr) {
        std::cout << "  iBin=["  << this_new_TopoYield->GetBinLowEdge(iBin) << ","
                               << this_new_TopoYield->GetBinLowEdge(iBin+1) << "]" 
        
                  << "  New="  << this_new_yield << " Old="  << this_old_yield 
                  << "  Diff=" << diff*100 << "%" << std::endl;
      }

    }
    std::cout << std::endl;
  }

  TCanvas *c = new TCanvas("c","c");
  hDiff->Draw("hist");
  TF1 *f1 = new TF1("f1", "gaus", -50, 50);
  hDiff->Fit("f1", "SRM");
  f1->Draw("same");
  c->SaveAs("diffLlepEst.pdf");

  return 0;

}
