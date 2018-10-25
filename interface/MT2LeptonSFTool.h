#ifndef MT2LeptonSFTool_h
#define MT2LeptonSFTool_h

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>

#include <string>
#include <iostream>

#include "TFile.h"
#include "TString.h"
#include "TH2.h"



struct lepSF{

  float sf = 1.0;
  float up = 1.0;
  float dn = 1.0;

};



class MT2LeptonSFTool {

  public:

    MT2LeptonSFTool(); // constructor
   
    // configuration methods
    bool setElHist(TString); //the TString is needed since the electron is not selected following the same POG working point (Veto for SR and llep, Loose for zll)
    bool setMuHist();
    //bool setVetoEffHistos( std::string filename);

    // get SF methods
    lepSF getElSF(float pt, float eta);
    lepSF getMuSF(float pt, float eta);

    void test();
    //lepSF getLepSF( float pt, float eta, int pdgId);
    //lepSF getVetoEffLepSF( float pt, float eta, int pdgId);
    //lepSF getLepSF_fast(float pt, float eta, int pdgId);


  private:

    // member histograms containing the product scale factors
    TH2D* h_EL_ID        = 0;
    TH2D* h_EL_RECO      = 0;
    TH2D* h_EL_RECO_LOWPT      = 0;

    TH2D* h_muSF_ID        = 0;
    TH2D* h_muSF_ISO       = 0;

    TH2D* h_eff_full_mu = 0;
    TH2D* h_eff_full_el = 0;

    TH2D* h_fast_elSF   = 0;
    TH2D* h_fast_muSF   = 0;

  
 // nothing really matters

};



#endif
