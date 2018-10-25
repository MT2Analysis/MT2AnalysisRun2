#ifndef MT2BTagSFHelper_h
#define MT2BTagSFHelper_h

#include <sstream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "TFile.h"
#include "TFileMerger.h"
#include "TTree.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TBranch.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2F.h"
#include "TLorentzVector.h"

#include "../interface/BTagCalibrationStandalone.h"

using namespace std;

//Working point
//float btag_wp = 0.8484;

// setup calibration readers 
// https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation74X  --  official SFs
// https://twiki.cern.ch/twiki/bin/view/CMS/BTagCalibration  --  calibration reader documentations

class MT2BTagSFHelper{

public:
  MT2BTagSFHelper();

  ~MT2BTagSFHelper();
  
  void get_SF_btag(float pt, float eta, int mcFlavour, float &SF, float &SFup, float &SFdown, bool isFastSim);

  float getBtagEffFromFile(float pt, float eta, int mcFlavour, bool isFastSim);
  
  void get_weight_btag(int nobj, float* obj_pt, float* obj_eta, int* obj_mcFlavour, float* obj_btagCSV, float &wtbtag, float &wtbtagUp_heavy, float &wtbtagDown_heavy, float &wtbtagUp_light, float &wtbtagDown_light, bool isFastSim);

private:
  BTagCalibrationStandalone *calib;
  BTagCalibrationStandaloneReader *reader_fullSim_heavy;
  BTagCalibrationStandaloneReader *reader_fullSim_light;

  BTagCalibrationStandalone *calib_fast;
  BTagCalibrationStandaloneReader *reader_fastSim;


  TFile *f_btag_eff;
  TH2D* h_btag_eff_b;
  TH2D* h_btag_eff_c;
  TH2D* h_btag_eff_udsg;


  TFile *f_btag_fast_eff;
  TH2D* h_btag_fast_eff_b;
  TH2D* h_btag_fast_eff_c;
  TH2D* h_btag_fast_eff_udsg;

};


#endif
