// Script to test the QCD estimate as converted from SnT to ETH format
// example usage:
// ./checkQCDEstimate /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/mg-branch/scripts/qcdEstimate.root nominal

// other directories:
// syst_jer
// syst_nbjetshape
// syst_njetshape
// syst_sigmasoft
// syst_tail

#include <fstream>

#include "TString.h"

#include "interface/MT2Analysis.h"
#include "interface/MT2Estimate.h"
#include "interface/MT2EstimateSyst.h"


int main( int argc, char* argv[] ) {

  if( argc != 3 ) {
    std::cout << "USAGE: ./checkQCDEstimate [QCDEstimateFile] [dirName]" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(11);
  }

  // new file /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/2016_compatible/analysis/EventYields_dataETH_SnTMC_35p9ifb/llepControlRegion/data.root
  // old file /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/old_mt2analysis/analysis/EventYields_dataETH_SnTMC_35p9ifb/llepControlRegion/data.root

  std::string fileName = argv[1];
  std::string dirName = argv[2];

  MT2Analysis<MT2Estimate>* newEst  = MT2Analysis<MT2Estimate>::readFromFile( fileName, dirName ); // second argument is the name of the directory
  //std::vector<MT2Analysis<MT2EstimateSyst>*> newEsts = MT2Analysis<MT2EstimateSigSyst>::readAllSystFromFile( dirName, "qcdEstimate", "jer" );
  //MT2Analysis<MT2EstimateSyst>* newEst  = MT2Analysis<MT2EstimateSyst>::readAllSystFromFile(  dirName, "qcdEstimate", "jer"); // jer_UP_qcdEstimate

  std::cout << "Read in all the inputs" << std::endl;

  // got the estimates
  std::set<MT2Region> regions = newEst->getRegions();
  
  std::cout << "Defined Regions " << std::endl;

  // loop over topological regions
  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    std::cout << "Working on topo region " << iR->getName()  << std::endl;

    TH1D* this_new_TopoYield = newEst->get(*iR)->yield;

    for( int iBin=1; iBin<this_new_TopoYield->GetNbinsX()+1; ++iBin ) {

      float this_new_yield = this_new_TopoYield->GetBinContent(iBin);

      
      std::cout << "iBin=[" << std::setprecision(1) << this_new_TopoYield->GetBinLowEdge(iBin) << ","
                            << std::setprecision(1) << this_new_TopoYield->GetBinLowEdge(iBin+1) << "]" << std::endl;

      std::cout << "Yield=" << std::setprecision(3) << this_new_yield << std::endl;
      
    }
    std::cout << std::endl;
  }

  delete newEst;

  std::cout << "finished" << std::endl;

}
