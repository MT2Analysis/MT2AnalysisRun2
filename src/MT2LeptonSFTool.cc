#include "../interface/MT2LeptonSFTool.h"
using namespace std;
// Tool to apply the lepton scale factors for the MT2 specific selections

// This MT2LeptonSFTool has a hard time to be elegant, and in fact it's not
// that's simply because the recommendations are far from being symmetric across channels and eras
// so it doesn't hurt too much to have everything hard-coded

// FIXME: current implementation is ok only for 2017 94X MC

MT2LeptonSFTool::MT2LeptonSFTool(){
  // doing nothing for the moment
}


void MT2LeptonSFTool::test(){
  std::cout << "je suis dans lepton scale factor et je fais un test" << std::endl;
}

bool MT2LeptonSFTool::setElHist(TString sel){

  // get the files 
  TString fname_ID = "../data/ElectronScaleFactors_Run2017.root";
  //pT > 20GeV
  TString fname_RECO = "../data/egammaEffi.txt_EGM2D_runBCDEF_passingRECO.root";
  //pT < 20GeV
  TString fname_RECO_LOWPT = "../data/egammaEffi.txt_EGM2D_runBCDEF_passingRECO_lowEt.root";
  //old files:
  //fname_ID = "/shome/mratti/mt2_workarea/CMSSW_8_0_12/src/myMT2Analysis/data/eleSF/egammaEffi.txt_EGM2D_runBCDEF_passingLoose94X.root";

  // https://twiki.cern.ch/twiki/bin/view/CMS/EgammaIDRecipesRun2 There we can find the recommandation for Moriond2019, with 2017 and 2018 samples. There is also a file with pT > 10GeV (which is our cut) --> should we consider this?

  TFile* f_ID = new TFile(fname_ID);
  TFile* f_RECO = new TFile(fname_RECO);
  TFile* f_RECO_LOWPT = new TFile(fname_RECO_LOWPT); 

  if (!f_ID->IsOpen() || !f_RECO->IsOpen() || !f_RECO_LOWPT->IsOpen())
    std::cout << " ERROR: Could not find scale factor file for ELECTRONS " << std::endl;
  
  // get the histograms from the file
  TH2D* h_ID_noIso = 0;
  if(sel == "SR" || sel == "llep" || sel == ""){
    //cout << "I enter in electron hist" << endl;
     h_ID_noIso  = (TH2D*) f_ID->Get("Run2017_CutBasedVetoNoIso94X");
  }
  else if(sel == "zll"){
    //cout << "You are not supposed to see me" << endl;
     h_ID_noIso  = (TH2D*) f_ID->Get("Run2017_CutBasedLooseNoIso94X");
  }
  TH2D* h_ID_Iso = (TH2D*) f_ID->Get("Run2017_MVAVLooseTightIP2DMini");
  TH2D* h_RECO = (TH2D*) f_RECO->Get("EGamma_SF2D");
  TH2D* h_RECO_LOWPT = (TH2D*) f_RECO_LOWPT->Get("EGamma_SF2D");
 
  if (!h_ID_noIso || !h_ID_Iso || !h_RECO || !h_RECO_LOWPT){
    std::cout << std::endl << std::endl << "ERROR: Could not find at least one of the scale factor histograms"<< std::endl << std::endl;
  }
  
  //  cout << "Number of bins of No iso: " << h_ID_noIso -> GetSize() << endl;
  //cout << "Number of bins of iso: " << h_ID_Iso -> GetSize() << endl;
  
  // set the member histograms

  h_EL_ID = (TH2D*) h_ID_noIso->Clone("h_EL_ID");
  h_EL_ID->SetDirectory(0); // the histogram doesn't belong to any directory anymore
  h_EL_ID->Multiply(h_ID_Iso); // we multiply the scale factors with and without isolation
  
  // we cannot multiply with the reco scale factors, since it will depend on the particular electron pt
  h_EL_RECO = (TH2D*) h_RECO->Clone("h_RECO");
  h_EL_RECO->SetDirectory(0);

  h_EL_RECO_LOWPT = (TH2D*) h_RECO_LOWPT->Clone("h_RECO_LOWPT");
  h_EL_RECO_LOWPT->SetDirectory(0);

  f_ID->Close();
  f_RECO->Close();
  f_RECO_LOWPT->Close();

  delete f_ID;
  delete f_RECO;
  delete f_RECO_LOWPT;

  return true;
}


bool MT2LeptonSFTool::setMuHist(){
 
  // get the files
  TString filenameID = "../data/RunBCDEF_SF_ID.root";
  TString filenameISO = "../data/SF.root";

  //old files
  // filenameID =   "/mnt/t3nfs01/data01/shome/mmasciov/lepSF/TnP_NUM_LooseID_DENOM_generalTracks_VAR_map_pt_eta.root";
  // filenameISO =  "/mnt/t3nfs01/data01/shome/mmasciov/lepSF/TnP_NUM_MiniIsoTight_DENOM_LooseID_VAR_map_pt_eta.root"; 
  // filenamedxyz = "/mnt/t3nfs01/data01/shome/mmasciov/lepSF/TnP_NUM_MediumIP2D_DENOM_LooseID_VAR_map_pt_eta.root";

  TFile * f1 = new TFile(filenameID);
  TFile * f2 = new TFile(filenameISO);
  //TFile * f3 = new TFile(filenamedxyz.c_str() );

  if (!f1->IsOpen()) { std::cout<<" ERROR: Could not find ID scale factor file "<<filenameID<<std::endl; return 0;}
  if (!f2->IsOpen()) { std::cout<<"ERROR: Could not find ISO scale factor file "<<filenameISO<<std::endl; return 0;}
  //if (!f3->IsOpen()) { std::cout<<"ERROR: Could not find dxy dz scale factor file "<<filenamedxyz<<std::endl; return 0;}

  TH2D* h_id_mu   = (TH2D*) f1->Get("NUM_LooseID_DEN_genTracks_pt_abseta");
  TH2D* h_iso_mu  = (TH2D*) f2->Get("TnP_MC_NUM_MiniIso02Cut_DEN_LooseID_PAR_pt_eta");
  //TH2D* h_dxyz_mu = (TH2D*) f3->Get("SF");

  //cout << "I enter muon hist" << endl;
  //cout << "Number of bins 1: " << h_id_mu->GetSize() << endl;
  //cout << "Number of bins 2: " << h_iso_mu->GetSize() << endl;

  if (!h_id_mu || !h_iso_mu) { std::cout<<"ERROR: Could not find scale factor histogram"<<std::endl; return 0;}
  

  h_muSF_ID = (TH2D*) h_id_mu->Clone("h_muSF_ID");
  h_muSF_ID->SetDirectory(0);
  // h_muSF->Multiply(h_iso_mu);//this causes a problem since the histograms don't have the same number of bins
  // h_muSF->Multiply(h_dxyz_mu);

  h_muSF_ISO = (TH2D*) h_iso_mu->Clone("h_muSF_ISO");
  h_muSF_ISO->SetDirectory(0);

  f1->Close();
  f2->Close();

  delete f1;
  delete f2;

  return true;
}

// TODO: add version for with muon track corr
// TODO: add version for fast sim




lepSF MT2LeptonSFTool::getElSF(float pt, float eta){

  lepSF weight;

  Float_t central = 1;
  Float_t tot_err = 0;
  Float_t err = 0;

  // auxiliary variables to access right bin
  float pt_cutoff = -9999;
  Int_t binx = -9999;
  Int_t biny = -9999;

  if( !h_EL_ID || !h_EL_RECO || !h_EL_RECO_LOWPT ){
    std::cout << "There are not scale factor histograms for me to read from " << std::endl;
    return weight;
  }

  // for h_EL_ID, histogram goes from 10 GeV to 500 GeV in pt (y axis)
  //if(pt>500) cout << "Je suis un electron avec un trop grand pT" << endl;
  pt_cutoff = std::max( 10., std::min( 499., double(pt)) );//select electron pT, or 10GeV or 500GeV if pT doesnt belong to that range
  // the 2D histograms are basically eta vs pT. So we pick up the scale factor corresponding to our electron features
  binx = h_EL_ID->GetXaxis()->FindBin(eta);
  biny = h_EL_ID->GetYaxis()->FindBin(pt_cutoff);
  central *= h_EL_ID->GetBinContent(binx,biny);
  err  = h_EL_ID->GetBinError(binx,biny);
  tot_err += err*err;

  // for RECO, you need to access to a different histogram depending on the pt
  if(pt>=20.){
    pt_cutoff = std::max( 20., std::min( 499., double(pt)) );//Note: SF at pT==500 is null, so we set the limit to 499
    //if(pt!=pt_cutoff) cout << "le cut marche, pT_cutoff= " << pt_cutoff << endl;
    binx = h_EL_RECO->GetXaxis()->FindBin(eta);
    biny = h_EL_RECO->GetYaxis()->FindBin(pt_cutoff);
    central *= h_EL_RECO->GetBinContent(binx,biny);
    //if(pt>500) cout << "et SF vaut " << h_EL_RECO->GetBinContent(binx,biny)<< endl;
    err = h_EL_RECO->GetBinError(binx,biny);
    tot_err += err*err;
  }else{
    // if(pt>500) cout << "est ce que je rentre systematiquement la?" << endl;
    binx = h_EL_RECO_LOWPT->GetXaxis()->FindBin(eta);
    central *= h_EL_RECO_LOWPT->GetBinContent(binx,1);//we set biny to 1 since eta is constant along pT axis
    err = h_EL_RECO_LOWPT->GetBinContent(binx,1);
    tot_err += err*err;
  }

  if (central > 1.3 || central < 0.3) std::cout<<"STRANGE SF: Electron with pT/eta of "<< pt <<"/"<< eta <<". SF is "<< central <<std::endl;

  // now finalize the calculation
  weight.sf = central;
  weight.up = central + sqrt(tot_err);
  weight.dn = central - sqrt(tot_err);

  return weight;

}





lepSF MT2LeptonSFTool::getMuSF(float pt, float eta){ 
  lepSF weight;

  if(!h_muSF_ID && !h_muSF_ISO){
    std::cout << "There are not scale factor histograms for me to read from " << std::endl;
    return weight;
  }

  Float_t central = 1;
  Float_t tot_err = 0;
  Float_t err = 0;

  float pt_cutoff = -9999;
  Int_t binx_ID = -9999;
  Int_t biny_ID = -9999;
  Int_t binx_ISO = -9999;
  Int_t biny_ISO = -9999;

  pt_cutoff = std::max(20., std::min(119., double(pt))); 
   
  binx_ID = h_muSF_ID->GetXaxis()->FindBin(pt_cutoff);
  biny_ID = h_muSF_ID->GetYaxis()->FindBin(fabs(eta));

    //	  float central_trk = 1;
    //	  Int_t binx_trk = h_muTrk_hi->GetXaxis()->FindBin(  lep_eta[o] );
    //	  if( binx_trk>10 ) binx_trk = 10;
    //	  else if( binx_trk<1 ) binx_trk = 1;
    //	  central_trk = h_muTrk_hi->GetBinContent( binx_trk );
    //
    //
    //	  if ( binx >7 ) binx = 7; //overflow bin empty for the muons...
    //	  central = h_muSF->GetBinContent(binx,biny);
    //
    //	  central *= central_trk;


    central *= h_muSF_ID->GetBinContent(binx_ID,biny_ID);
    err = h_muSF_ID->GetBinError(binx_ID,biny_ID);
    tot_err += err*err;

    binx_ISO = h_muSF_ISO->GetXaxis()->FindBin(pt_cutoff);
    biny_ISO = h_muSF_ISO->GetYaxis()->FindBin(fabs(eta));

    central *= h_muSF_ISO->GetBinContent(binx_ISO,biny_ISO);
    err = h_muSF_ISO->GetBinError(binx_ISO,biny_ISO);
    tot_err += err*err;

    //err  = 0.03; //current recomendation is 3% //   err  = 0.014; // adding in quadrature 1% unc. on ID and 1% unc. on ISO
    if (central > 1.2 || central < 0.8){
      std::cout<<"STRANGE: Muon with pT/eta of "<<pt<<"/"<< fabs(eta) <<". SF is "<< central <<std::endl;
    }else{
      //cout << "Good muon" << endl;
    }

  weight.sf = central;
  weight.up = central + sqrt(tot_err);
  weight.dn = central - sqrt(tot_err);

  return weight;
}

















/*
bool MT2LeptonSFTool::setVetoEffHistos( std::string filename){
// currently not supported

  if( filename == "")
    filename = "/mnt/t3nfs01/data01/shome/mmasciov/lepSF/vetoeff_emu_etapt_lostlep.root";

  TFile* f_eff_full = new TFile(filename.c_str() );

  if (!f_eff_full->IsOpen()) std::cout << " ERROR: Could not find scale factor file for veto eff" << filename << std::endl;

  h_eff_full_mu = (TH2D*) f_eff_full->Get("h_mu_comb_eff");
  h_eff_full_el = (TH2D*) f_eff_full->Get("h_ele_comb_eff");
  if(!h_eff_full_mu || !h_eff_full_el ) {std::cout << " ERROR: Could not find the 2D histogram in your files " << std::endl; return 0;}
  h_eff_full_mu->SetDirectory(0);
  h_eff_full_el->SetDirectory(0);

  f_eff_full->Close();

  return true;
}
*/


/*
lepSF MT2LeptonSFTool::getVetoEffLepSF( float pt, float eta, int pdgId){

  lepSF weights;

  if( !h_muSF || !h_elSF || !h_elSF_trk ){
    std::cout << "There are not scale factor histograms for me to read from" << std::endl;
      return weights;
  }


  float central=1.;
  float err=0;
  float uncert=0;
  float fast_central=1.;
  float fast_err=0.;
  float uncert_fast=0;

  float pt_cutoff = std::max( 10.1, std::min( 100., double(pt) ) );
  float pt_cutoff_eff = std::max( 5.1, std::min( 100., double(pt) ) );
  float veto_eff = 1.; //just need one veto eff, not one for full and fast sim


  if ( abs( pdgId ) == 11) {
    Int_t binx = h_elSF->GetXaxis()->FindBin(pt_cutoff);
    Int_t biny = h_elSF->GetYaxis()->FindBin( fabs(eta) );
    central = h_elSF->GetBinContent(binx,biny);
    uncert  = h_elSF->GetBinError(binx,biny);

    Int_t binx_eff = h_eff_full_el->GetXaxis()->FindBin(pt_cutoff_eff);
    Int_t biny_eff = h_eff_full_el->GetYaxis()->FindBin( fabs(eta) );
    veto_eff = h_eff_full_el->GetBinContent(binx_eff,biny_eff);

  } else if( abs( pdgId ) == 13) {
    Int_t binx = h_muSF->GetXaxis()->FindBin(pt_cutoff);
    Int_t biny = h_muSF->GetYaxis()->FindBin( fabs(eta) );
    central = h_muSF->GetBinContent(binx,biny);
    uncert = 0.03;// uncert  = 0.014;   // adding in quadrature 1% unc. on ID and 1% unc. on ISO

    //	  float central_trk = 1;
    //	  if( pt >= 10.){
    //	    Int_t binx_trk = h_muTrk_hi->GetXaxis()->FindBin(  lep_eta[o] );
    //	    if( binx_trk>10 ) binx_trk = 10;
    //	    else if( binx_trk<1 ) binx_trk = 1;
    //	    central_trk = h_muTrk_hi->GetBinContent( binx_trk );
    //	  }else if( pt < 10 ){
    //	    Int_t binx_trk = h_muTrk_lo->GetXaxis()->FindBin(  lep_eta[o] );
    //	    if( binx_trk>10 ) binx_trk = 10;
    //	    else if( binx_trk<1 ) binx_trk = 1;
    //	    central_trk = h_muTrk_lo->GetBinContent( binx_trk );
    //	  }
    //	  central *= central_trk;

    Int_t binx_eff = h_eff_full_mu->GetXaxis()->FindBin( pt_cutoff_eff );
    Int_t biny_eff = h_eff_full_mu->GetYaxis()->FindBin( fabs(eta) );
    veto_eff = h_eff_full_mu->GetBinContent(binx_eff,biny_eff);
  }

  //Only full sim correction, only uncertainty, not weight

  float sf = central;
  // float veto_eff_corr = veto_eff* sf; //corrected veto eff with the

  float unc = uncert;
  float veto_eff_unc_UP = veto_eff* (1. + unc);

  float unc_UP_0l = (( 1. - veto_eff_unc_UP) / (1. - veto_eff))  -1.;
  // weight_lepsf_0l_UP *= ( 1. + unc_UP_0l);
  // weight_lepsf_0l_DN *= ( 1. - unc_UP_0l);


  weights.sf = central;
  weights.up = ( 1. + unc_UP_0l);
  weights.dn = ( 1. - unc_UP_0l);


  return weights;
}


*/



/*
lepSF MT2LeptonSFTool::getLepSF_fast( float pt, float eta, int pdgId){

  lepSF weights;

  if( !h_muSF || !h_elSF || !h_elSF_trk ){
    std::cout << "There are not scale factor histograms for me to read from " << std::endl;
    return weights;
  }

  Float_t uncert = 1; //Place holder for total uncertainty
  Float_t central = 1;
  Float_t err = 0;
  Float_t uncert_UP = 0; 	Float_t uncert_DN = 0;

  Float_t fast_central = 1;
  Float_t fast_err = 0;


  float pt_cutoff = std::max( 10.1, std::min( 100., double(pt) ) );

  //Electrons
  if( pdgId == 11) {

   // Int_t binx = h_elSF->GetXaxis()->FindBin(pt_cutoff);
   //  Int_t biny = h_elSF->GetYaxis()->FindBin(eta);
   //  central = h_elSF->GetBinContent(binx,biny);
   //  err  = h_elSF->GetBinError(binx,biny);

   //  int binx_trk = h_elSF_trk->GetXaxis()->FindBin(eta);
   //  int biny_trk = 1; // hardcoding for now - only one bin in pt (hist starts at 20)
   //  central *= h_elSF_trk->GetBinContent(binx_trk,biny_trk);
   //  float trk_err = h_elSF_trk->GetBinError(binx_trk,biny_trk);
   //  if (pt_cutoff < 20. || pt_cutoff > 80.) err = sqrt(err*err + trk_err*trk_err + 0.01*0.01);
   //  else err = sqrt(err*err + trk_err*trk_err);

   //  if (central > 1.3 || central < 0.3)
   //    std::cout<<"STRANGE: Electron with pT/eta of "<< pt <<"/"<< eta <<". SF is "<< central <<std::endl;
   //  uncert_UP = central + err;
   //  uncert_DN = central - err;

    Int_t fast_binx = h_fast_elSF->GetXaxis()->FindBin(pt_cutoff);
    Int_t fast_biny = h_fast_elSF->GetYaxis()->FindBin(eta);
    fast_central = h_fast_elSF->GetBinContent(fast_binx,fast_biny);
    fast_err = h_fast_elSF->GetBinError(fast_binx,fast_biny);
    fast_err = sqrt(fast_err*fast_err + 0.05*0.05); // 5% systematic uncertainty

    if( fast_central > 1.3 || fast_central < 0.7 )
      std::cout << "Strange FastSim Electron with pT/eta of" << pt <<"/"<< eta <<". SF is "<< fast_central <<std::endl;

    central   = fast_central;
    uncert_UP = ( fast_central + fast_err );
    uncert_DN = ( fast_central - fast_err );
    // uncert_UP += fast_err ;
    //uncert_DN -= fast_err ;



  } //else Muons
  else if( pdgId == 13) {

    // Int_t binx = h_muSF->GetXaxis()->FindBin(pt_cutoff);
    // Int_t biny = h_muSF->GetYaxis()->FindBin(fabs(eta));

    // //	  float central_trk = 1;
    // //	  Int_t binx_trk = h_muTrk_hi->GetXaxis()->FindBin(  lep_eta[o] );
    // //	  if( binx_trk>10 ) binx_trk = 10;
    // //	  else if( binx_trk<1 ) binx_trk = 1;
    // //	  central_trk = h_muTrk_hi->GetBinContent( binx_trk );
    // //
    // //
    // //	  if ( binx >7 ) binx = 7; //overflow bin empty for the muons...
    // //	  central = h_muSF->GetBinContent(binx,biny);
    // //
    // //	  central *= central_trk;


    // central = h_muSF->GetBinContent(binx,biny);
    // err  = 0.03; //current recomendation is 3% //   err  = 0.014; // adding in quadrature 1% unc. on ID and 1% unc. on ISO
    // if (central > 1.2 || central < 0.8)
    //   std::cout<<"STRANGE: Muon with pT/eta of "<<pt<<"/"<< fabs(eta) <<". SF is "<< central <<std::endl;
    // uncert_UP = central + err;
    // uncert_DN = central - err;

    Int_t fast_binx = h_fast_muSF->GetXaxis()->FindBin(pt);
    Int_t fast_biny = h_fast_muSF->GetYaxis()->FindBin(fabs(eta));
    fast_central = h_fast_muSF->GetBinContent(fast_binx,fast_biny);
    fast_err  = h_fast_muSF->GetBinError(fast_binx,fast_biny);
    if( pt < 20)
      fast_err= sqrt(fast_err*fast_err+ 0.03*0.03); // 3% systematic uncertainty
    else
      fast_err= sqrt(fast_err*fast_err+ 0.01*0.01); // 1% systematic uncertainty

    if( fast_central > 1.3 || fast_central < 0.7 )
      std::cout << "Strange FastSim Muon with pT/eta of" <<pt<<"/"<<eta<<". SF is "<< fast_central <<std::endl;

    central   = fast_central;

    uncert_UP = ( fast_central + fast_err );
    uncert_DN = ( fast_central - fast_err );

    //uncert_UP += fast_err ;
    //uncert_DN -= fast_err ;

  }//done with one  electron/muon

  weights.sf = central;
  weights.up = uncert_UP;
  weights.dn = uncert_DN;

  return weights;
}


*/
