// Script to check different yields as obtained from two different versions of the code
// Relative yields difference above specified threshold will be printed to screen
// example usage:
// ./checkYields /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/2016_compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/llepControlRegion/data.root /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/old_mt2analysis/analysis/EventYields_dataETH_SnTMC_35p9ifb/llepControlRegion/data.root llepCR 0.0001

#include <fstream>

#include "TString.h"
#include "TCanvas.h"
#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"

int main( int argc, char* argv[] ) {

  if( argc != 6 ) {
    std::cout << "USAGE: ./checkYields [newYieldsFile] [oldYieldsFile] [newDirName] [oldDirName] [max allowed difference]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

  // new file /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/2016_compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/llepControlRegion/data.root
  // old file /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/old_mt2analysis/analysis/EventYields_dataETH_SnTMC_35p9ifb/llepControlRegion/data.root

  std::string newFileName = argv[1];
  std::string oldFileName = argv[2];
  std::string newDirName = argv[3];
  std::string oldDirName = argv[4];
  float thr = std::stod(argv[5]);

  MT2Analysis<MT2Estimate>* newEst  = MT2Analysis<MT2Estimate>::readFromFile( newFileName, newDirName ); // second argument is the name of the directory
  MT2Analysis<MT2Estimate>* oldEst  = MT2Analysis<MT2Estimate>::readFromFile( oldFileName, oldDirName );
  std::cout << "Read in all the inputs" << std::endl;

  // got the estimates
  std::set<MT2Region> regions = newEst->getRegions();
  // TODO: check that the regions are the same bw new and old
  std::cout << "Defined Regions " << std::endl;

  // a plot to look overall at the differences
  TH1D* hDiff = new TH1D("hDiff", "hDiff", 50,-100., 100.);
  TH1D* hAvDiff = new TH1D("hAvDiff", "hAvDiff", 50,-100., 100.);
  double sum_yield = 0.;
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
      hAvDiff->Fill(diff*100*this_new_yield); // higher weight if yield is higher
      sum_yield += this_new_yield;
      //if( isnan(this_old_yield) || isnan(this_new_yield)) std::cout << "WARNING, found nan " << std::endl;

      if (abs(diff) >= thr){
      //  std::cout << "Found difference in iBin=["  << this_new_TopoYield->GetBinLowEdge(iBin) << ","
      //                                             << this_new_TopoYield->GetBinLowEdge(iBin+1) << "]" << std::endl;

      //  std::cout << "New="  << this_new_yield << " Old="  << this_old_yield << std::endl;
      //  std::cout << "  Diff=" << diff*100 << "%" << std::endl;
      //}

        std::cout << "  iBin=["  << this_new_TopoYield->GetBinLowEdge(iBin) << ","
                               << this_new_TopoYield->GetBinLowEdge(iBin+1) << "]" 
        
                  << "  New="  << this_new_yield << " Old="  << this_old_yield 
                  << "  Diff=" << diff*100 << "%" << std::endl;
      } else {

        std::cout << "  iBin=["  << this_new_TopoYield->GetBinLowEdge(iBin) << ","
                               << this_new_TopoYield->GetBinLowEdge(iBin+1) << "]"  << std::endl;

      }

    }
    std::cout << std::endl;
  }

  TCanvas *c = new TCanvas("c","c");
  hDiff->Draw("hist");
  c->SaveAs("diff.pdf");
  TCanvas *c1 = new TCanvas("c1","c1");
  hAvDiff->Scale(1./sum_yield);
  hAvDiff->Draw("hist");
  c1->SaveAs("Avdiff.pdf");

  return 0;

}
