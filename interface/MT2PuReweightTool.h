#ifndef MT2PuReweightTool_h
#define MT2PuReweightTool_h

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

#include <string>
#include <iostream>

#include "TFile.h"
#include "TString.h"
#include "TH1.h"



class MT2PuReweightTool {

  public:

    MT2PuReweightTool(); // constructor

    // configuration method
    bool setPuWeightHist(int year=2016);

    // get weight method
    double getPuWeight(int evt_nTrueInt);



  private:

    // member histograms with weights
		TH1D* h_nTrueInt_weights = 0;



};



#endif
