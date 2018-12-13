
#include "../interface/MT2BTagSFHelper.h"

using namespace std;




MT2BTagSFHelper::MT2BTagSFHelper(){

  //CSVv2 scale factors from 94X BTV POG recommendation
  calib = new BTagCalibrationStandalone("csvv2", "/t3home/anlyon/CMSSW_8_0_12/src/myMT2Analysis/data/b_tagging/CSVv2_94XSF_V2_B_F.csv");
  //calib = new BTagCalibrationStandalone("csvv2", "/shome/mschoene/btagSF/CSVv2Moriond17_2017_1_26_BtoH.csv"); 
  //calib = new BTagCalibrationStandalone("csvv2", "/shome/mschoene/btagSF/CSVv2_ichep.csv");
  reader_fullSim_heavy    = new BTagCalibrationStandaloneReader(BTagEntryStandalone::OP_MEDIUM, "central",{"up","down"}); 
  reader_fullSim_light    = new BTagCalibrationStandaloneReader(BTagEntryStandalone::OP_MEDIUM, "central",{"up","down"}); 

  reader_fullSim_heavy->load(*calib,BTagEntryStandalone::FLAV_UDSG,"comb");
  reader_fullSim_heavy->load(*calib,BTagEntryStandalone::FLAV_B,"comb");
  reader_fullSim_heavy->load(*calib,BTagEntryStandalone::FLAV_C,"comb");
  reader_fullSim_light->load(*calib,BTagEntryStandalone::FLAV_UDSG,"incl");
  reader_fullSim_light->load(*calib,BTagEntryStandalone::FLAV_B,"incl");
  reader_fullSim_light->load(*calib,BTagEntryStandalone::FLAV_C,"incl");

  //FIXME: for the efficiency, I guess that we will have to recompute it with our kinematics
  f_btag_eff = new TFile("/shome/mschoene/btagSF/btageff__ttbar_powheg_pythia8_25ns_Moriond17.root"); // Dominick's b-tagging efficiencies
  //f_btag_eff = new TFile("/shome/mschoene/btagSF/btageff__ttbar_powheg_pythia8_25ns.root"); // Dominick's b-tagging efficiencies
  h_btag_eff_b    = (TH2D*) f_btag_eff->Get("h2_BTaggingEff_csv_med_Eff_b"   );
  h_btag_eff_c    = (TH2D*) f_btag_eff->Get("h2_BTaggingEff_csv_med_Eff_c"   );
  h_btag_eff_udsg = (TH2D*) f_btag_eff->Get("h2_BTaggingEff_csv_med_Eff_udsg");

  //fastSimulation files: not needed anymore, kept the old ones
  calib_fast = new BTagCalibrationStandalone("csvv2", "/shome/mschoene/btagSF/fastsim_csvv2_ttbar_26_1_2017.csv"); // <- this is the fixed version, the one on the twiki had a bug in the format (too many "")
  //calib_fast = new BTagCalibrationStandalone("csvv2", "/shome/mschoene/btagSF/CSV_13TEV_Combined_14_7_2016.csv"); // 25 ns official version of SFs
 
  reader_fastSim    = new BTagCalibrationStandaloneReader(BTagEntryStandalone::OP_MEDIUM, "central", {"up","down"}); 

  reader_fastSim->load(*calib_fast,BTagEntryStandalone::FLAV_UDSG,"fastsim");
  reader_fastSim->load(*calib_fast,BTagEntryStandalone::FLAV_B,"fastsim");
  reader_fastSim->load(*calib_fast,BTagEntryStandalone::FLAV_C,"fastsim");

  f_btag_fast_eff = new TFile("/shome/mschoene/btagSF/btageff__SMS-T1bbbb-T1qqqq_25ns_Moriond17.root"); // Dominick's fast sim b-tagging efficiencies
  //f_btag_fast_eff = new TFile("/shome/mschoene/btagSF/btageff__SMS-T1bbbb-T1qqqq_fastsim_2016.root"); // Dominick's fast sim b-tagging efficiencies
  h_btag_fast_eff_b    = (TH2D*) f_btag_fast_eff->Get("h2_BTaggingEff_csv_med_Eff_b"   );
  h_btag_fast_eff_c    = (TH2D*) f_btag_fast_eff->Get("h2_BTaggingEff_csv_med_Eff_c"   );
  h_btag_fast_eff_udsg = (TH2D*) f_btag_fast_eff->Get("h2_BTaggingEff_csv_med_Eff_udsg");
}

MT2BTagSFHelper::~MT2BTagSFHelper(){
  delete calib;
  delete reader_fullSim_heavy;
  delete reader_fullSim_light;

  delete calib_fast;
  delete reader_fastSim;


  delete f_btag_eff;
  delete h_btag_eff_b;
  delete h_btag_eff_c;
  delete h_btag_eff_udsg;

  delete f_btag_fast_eff;
  delete h_btag_fast_eff_b;
  delete h_btag_fast_eff_c;
  delete h_btag_fast_eff_udsg;
}





void MT2BTagSFHelper::get_SF_btag(float pt, float eta, int mcFlavour, float &SF, float &SFup, float &SFdown, bool isFastSim){

  BTagEntryStandalone::JetFlavor flavour = BTagEntryStandalone::FLAV_UDSG;
  if      ( abs(mcFlavour)==5 ) flavour = BTagEntryStandalone::FLAV_B;
  else if ( abs(mcFlavour)==4 ) flavour = BTagEntryStandalone::FLAV_C;
  
  //BM: these two lines are probably unnecessary now with "eval_auto_bounds" method. To be checked
  //Checked and it is still needed:
  float pt_cutoff  = std::max(29.999 ,std::min(669., double(pt)));
  //  float pt_cutoff  = std::max(30. ,std::min(669., double(pt)));
  float eta_cutoff = std::min(2.39,fabs(double(eta)));
  //FIXME: update the values 

  if ( flavour==BTagEntryStandalone::FLAV_UDSG ){
    SF     = reader_fullSim_light->eval_auto_bounds("central",flavour,eta_cutoff, pt_cutoff);
    SFup   = reader_fullSim_light->eval_auto_bounds("up",flavour,eta_cutoff, pt_cutoff);
    SFdown = reader_fullSim_light->eval_auto_bounds("down",flavour,eta_cutoff, pt_cutoff);
  }
  else {
    SF     = reader_fullSim_heavy->eval_auto_bounds("central",flavour,eta_cutoff, pt_cutoff);
    SFup   = reader_fullSim_heavy->eval_auto_bounds("up",flavour,eta_cutoff, pt_cutoff);
    SFdown = reader_fullSim_heavy->eval_auto_bounds("down",flavour,eta_cutoff, pt_cutoff);
  }

  if( isFastSim ){
    SF     *= reader_fastSim->eval_auto_bounds("central",flavour,eta_cutoff, pt_cutoff);
    SFup   *= reader_fastSim->eval_auto_bounds("up",flavour,eta_cutoff, pt_cutoff);
    SFdown *= reader_fastSim->eval_auto_bounds("down",flavour,eta_cutoff, pt_cutoff);
  }

}


float MT2BTagSFHelper::getBtagEffFromFile(float pt, float eta, int mcFlavour, bool isFastSim){
  if(!h_btag_eff_b || !h_btag_eff_c || !h_btag_eff_udsg) {
    std::cout << "ERROR: missing input hists" << std::endl;
    return 1.;
  }

  if( isFastSim && (!h_btag_fast_eff_b || !h_btag_fast_eff_c || !h_btag_fast_eff_udsg) ) {
    std::cout << "ERROR: missing fastsim input hists" << std::endl;
    return 1.;
  }
  
  // only use pt bins up to 400 GeV for charm and udsg
  //FIXME: update the values
  float pt_cutoff = std::max(20.,std::min(399.,double(pt)));
  TH2D* h(0);
  if (abs(mcFlavour) == 5) {
    h = isFastSim ? h_btag_fast_eff_b : h_btag_eff_b; 
    // use pt bins up to 600 GeV for b
    pt_cutoff = std::max(20.,std::min(599.,double(pt)));
  }
  else if (abs(mcFlavour) == 4) {
    h = isFastSim ? h_btag_fast_eff_c : h_btag_eff_c;
  }
  else {
    h = isFastSim ? h_btag_fast_eff_udsg : h_btag_eff_udsg;
  }
  
  int binx = h->GetXaxis()->FindBin(pt_cutoff);
  int biny = h->GetYaxis()->FindBin(fabs(eta));
  return h->GetBinContent(binx,biny);
}



void MT2BTagSFHelper::get_weight_btag(int nobj, float* obj_pt, float* obj_eta, int* obj_mcFlavour, float* obj_btagCSV, float &wtbtag, float &wtbtagUp_heavy, float &wtbtagDown_heavy, float &wtbtagUp_light, float &wtbtagDown_light, bool isFastSim){


  float mcTag = 1.;
  float mcNoTag = 1.;
  float dataTag = 1.;
  float dataNoTag = 1.;

  float errHup   = 1.;
  float errHdown = 1.;
  float errLup   = 1.;
  float errLdown = 1.;

  // float errHup   = 0;
  // float errHdown = 0;
  // float errLup   = 0;
  // float errLdown = 0;

  for(int indj=0; indj < nobj; ++indj){ //Here we loop over all selected jets ( for our case, pt>30, PF loose ID, etc )
    
    float csv = obj_btagCSV[indj]; ////here we get the CSV btag value
    int   mcFlavour = abs(obj_mcFlavour[indj]);
    float eta = fabs(obj_eta[indj]);
    float pt  = obj_pt[indj];

    //cout << "nobj: " << indj << " eta: " << eta << " pt: " << pt << " mcFlavour: " << mcFlavour << endl;

  
    if(eta > 2.4) continue;
    if(pt  < 20 ) continue;
    //if(mcFlavour==0) continue; //for jets with flavour 0, we ignore.

    
    // get efficiency
    float eff = getBtagEffFromFile(pt, eta, mcFlavour, isFastSim);

  
    bool istag = (csv >= 0.8838 && eta <= 2.4 && pt>=20);
    float SF = 0.;
    float SFup = 0.;
    float SFdown = 0.;

    // get SF
    get_SF_btag(pt, eta, mcFlavour, SF, SFup, SFdown, isFastSim);

    if(istag){
      mcTag   *= eff;
      dataTag *= eff*SF;

      if(mcFlavour==5 || mcFlavour ==4) {//b or c
	errHup   *= (SFup * eff);
	errHdown *= (SFdown * eff);
	errLup   *= (SF * eff);
	errLdown *= (SF * eff);
      }
      else {
	errHup   *= (SF * eff);
	errHdown *= (SF * eff);
	errLup   *= (SFup * eff);
	errLdown *= (SFdown * eff);
      }

    }
    else{ //NOT TAGGED
      mcNoTag   *= (1 - eff);
      dataNoTag *= (1 - eff*SF);

      if( mcFlavour==5 || mcFlavour==4 ) {//b or c
	errHup   *= (1. - SFup * eff );
	errHdown *= (1. - SFdown * eff );
	errLup   *= (1. - SF * eff );
	errLdown *= (1. - SF * eff );
      }
      else {
	errHup   *= (1. - SF * eff );
	errHdown *= (1. - SF * eff );
	errLup   *= (1. - SFup * eff );
	errLdown *= (1. - SFdown * eff );
      }
    }

  }

  wtbtag = (dataNoTag * dataTag ) / ( mcNoTag * mcTag );

  wtbtagUp_heavy   = errHup   / ( mcNoTag * mcTag );
  wtbtagUp_light   = errLup   / ( mcNoTag * mcTag );
  wtbtagDown_heavy = errHdown / ( mcNoTag * mcTag );
  wtbtagDown_light = errLdown / ( mcNoTag * mcTag );
 
}


