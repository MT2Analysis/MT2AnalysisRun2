#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
 //#include <cmath>

#include "TMath.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TPaveText.h"
#include "TGraphAsymmErrors.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TString.h"
using namespace std;
int preselection_forML_reducefeature_llcontrol(){

    TRandom *rand= new TRandom(1234);
    string files2016mcbkg_dyjetsll[7]={"/scratch/mratti/NEWSnTtrees/2016/dyjetsll_ht100to200.root",
    "/scratch/mratti/NEWSnTtrees/2016/dyjetsll_ht1200to2500.root",
    "/scratch/mratti/NEWSnTtrees/2016/dyjetsll_ht200to400.root",
    "/scratch/mratti/NEWSnTtrees/2016/dyjetsll_ht2500toInf.root",
    "/scratch/mratti/NEWSnTtrees/2016/dyjetsll_ht400to600.root",
    "/scratch/mratti/NEWSnTtrees/2016/dyjetsll_ht600to800.root",
    "/scratch/mratti/NEWSnTtrees/2016/dyjetsll_ht800to1200.root"};

    string files2016mcbkg_gjets[5]={"/scratch/mratti/NEWSnTtrees/2016/gjets_dr0p05_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2016/gjets_dr0p05_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2016/gjets_dr0p05_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2016/gjets_dr0p05_ht40to100.root",
"/scratch/mratti/NEWSnTtrees/2016/gjets_dr0p05_ht600toInf.root"};

    string files2016mcbkg_qcd[6]={"/scratch/mratti/NEWSnTtrees/2016/qcd_ht1000to1500.root",
"/scratch/mratti/NEWSnTtrees/2016/qcd_ht1500to2000.root",
"/scratch/mratti/NEWSnTtrees/2016/qcd_ht2000toInf.root",
"/scratch/mratti/NEWSnTtrees/2016/qcd_ht300to500.root",
"/scratch/mratti/NEWSnTtrees/2016/qcd_ht500to700.root",
"/scratch/mratti/NEWSnTtrees/2016/qcd_ht700to1000.root"};

    string files2016mcbkg_singletop[5]={"/scratch/mratti/NEWSnTtrees/2016/singletop_tW_tbar.root",
"/scratch/mratti/NEWSnTtrees/2016/singletop_tW_top.root",
"/scratch/mratti/NEWSnTtrees/2016/singletop_tchan_tbar.root",
"/scratch/mratti/NEWSnTtrees/2016/singletop_tchan_top.root",
"/scratch/mratti/NEWSnTtrees/2016/singletop_schan.root"};

    string files2016mcbkg_tt[6]={"/scratch/mratti/NEWSnTtrees/2016/ttdl.root",
"/scratch/mratti/NEWSnTtrees/2016/ttsl_tbar.root",
"/scratch/mratti/NEWSnTtrees/2016/ttsl_top.root",
"/scratch/mratti/NEWSnTtrees/2016/ttz_mg_lo.root",
"/scratch/mratti/NEWSnTtrees/2016/ttw_lnu_amcatnlo.root",
"/scratch/mratti/NEWSnTtrees/2016/ttw_qq_amcatnlo.root"};

    string files2016mcbkg_tt_negligible[2]={"/scratch/mratti/NEWSnTtrees/2016/ttg_amcatnlo.root",
"/scratch/mratti/NEWSnTtrees/2016/tttt.root"};

    string files2016mcbkg_wjets[7]={"/scratch/mratti/NEWSnTtrees/2016/wjets_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2016/wjets_ht1200to2500.root",
"/scratch/mratti/NEWSnTtrees/2016/wjets_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2016/wjets_ht2500toInf.root",
"/scratch/mratti/NEWSnTtrees/2016/wjets_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2016/wjets_ht600to800.root",
"/scratch/mratti/NEWSnTtrees/2016/wjets_ht800to1200.root"};


    string files2016mcbkg_ww_wz[2]={"/scratch/mratti/NEWSnTtrees/2016/ww.root",
"/scratch/mratti/NEWSnTtrees/2016/wz.root"};

    string files2016mcbkg_zinv[7]={"/scratch/mratti/NEWSnTtrees/2016/zinv_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2016/zinv_ht1200to2500.root",
"/scratch/mratti/NEWSnTtrees/2016/zinv_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2016/zinv_ht2500toInf.root",
"/scratch/mratti/NEWSnTtrees/2016/zinv_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2016/zinv_ht600to800.root",
"/scratch/mratti/NEWSnTtrees/2016/zinv_ht800to1200.root"};

    string files2017mcbkg_dyjetsll[7]={"/scratch/mratti/NEWSnTtrees/2017/dyjetsll_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2017/dyjetsll_ht1200to2500.root",
"/scratch/mratti/NEWSnTtrees/2017/dyjetsll_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2017/dyjetsll_ht2500toInf.root",
"/scratch/mratti/NEWSnTtrees/2017/dyjetsll_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2017/dyjetsll_ht600to800.root",
"/scratch/mratti/NEWSnTtrees/2017/dyjetsll_ht800to1200.root"};

    string files2017mcbkg_gjets[5]={"/scratch/mratti/NEWSnTtrees/2017/gjets_dr0p05_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2017/gjets_dr0p05_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2017/gjets_dr0p05_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2017/gjets_dr0p05_ht40to100.root",
"/scratch/mratti/NEWSnTtrees/2017/gjets_dr0p05_ht600toInf.root"};

    string files2017mcbkg_qcd[6]={"/scratch/mratti/NEWSnTtrees/2017/qcd_ht1000to1500.root",
"/scratch/mratti/NEWSnTtrees/2017/qcd_ht1500to2000.root",
"/scratch/mratti/NEWSnTtrees/2017/qcd_ht2000toInf.root",
"/scratch/mratti/NEWSnTtrees/2017/qcd_ht300to500.root",
"/scratch/mratti/NEWSnTtrees/2017/qcd_ht500to700.root",
"/scratch/mratti/NEWSnTtrees/2017/qcd_ht700to1000.root"};

    string files2017mcbkg_singletop[5]={"/scratch/mratti/NEWSnTtrees/2017/singletop_schan.root",
"/scratch/mratti/NEWSnTtrees/2017/singletop_tW_tbar.root",
"/scratch/mratti/NEWSnTtrees/2017/singletop_tW_top.root",
"/scratch/mratti/NEWSnTtrees/2017/singletop_tchan_tbar.root",
"/scratch/mratti/NEWSnTtrees/2017/singletop_tchan_top.root"};

    string files2017mcbkg_tt[5]={"/scratch/mratti/NEWSnTtrees/2017/ttdl_mg.root",
"/scratch/mratti/NEWSnTtrees/2017/ttsl_tbar_mg.root",
"/scratch/mratti/NEWSnTtrees/2017/ttsl_top_mg.root",
"/scratch/mratti/NEWSnTtrees/2017/ttz_mg.root",
"/scratch/mratti/NEWSnTtrees/2017/ttw_mg.root"};

    string files2017mcbkg_tt_negligible[2]={"/scratch/mratti/NEWSnTtrees/2017/ttg_amcatnlo.root",
"/scratch/mratti/NEWSnTtrees/2017/tttt.root"};

    string files2017mcbkg_wjets[9]={"/scratch/mratti/NEWSnTtrees/2017/wjets_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2017/wjets_ht1200to2500.root",
"/scratch/mratti/NEWSnTtrees/2017/wjets_ht1200to2500_1.root",
"/scratch/mratti/NEWSnTtrees/2017/wjets_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2017/wjets_ht2500toInf.root",
"/scratch/mratti/NEWSnTtrees/2017/wjets_ht2500toInf_1.root",
"/scratch/mratti/NEWSnTtrees/2017/wjets_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2017/wjets_ht600to800.root",
"/scratch/mratti/NEWSnTtrees/2017/wjets_ht800to1200.root"};

    string files2017mcbkg_ww_wz[2]={"/scratch/mratti/NEWSnTtrees/2017/ww.root",
"/scratch/mratti/NEWSnTtrees/2017/wz.root"};

    string files2017mcbkg_zinv[7]={"/scratch/mratti/NEWSnTtrees/2017/zinv_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2017/zinv_ht1200to2500.root",
"/scratch/mratti/NEWSnTtrees/2017/zinv_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2017/zinv_ht2500toInf.root",
"/scratch/mratti/NEWSnTtrees/2017/zinv_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2017/zinv_ht600to800.root",
"/scratch/mratti/NEWSnTtrees/2017/zinv_ht800to1200.root"};

    string files2018mcbkg_dyjetsll[7]={"/scratch/mratti/NEWSnTtrees/2018/dyjetsll_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2018/dyjetsll_ht1200to2500.root",
"/scratch/mratti/NEWSnTtrees/2018/dyjetsll_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2018/dyjetsll_ht2500toInf.root",
"/scratch/mratti/NEWSnTtrees/2018/dyjetsll_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2018/dyjetsll_ht600to800.root",
"/scratch/mratti/NEWSnTtrees/2018/dyjetsll_ht800to1200.root"};

    string files2018mcbkg_gjets[4]={"/scratch/mratti/NEWSnTtrees/2018/gjets_dr0p05_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2018/gjets_dr0p05_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2018/gjets_dr0p05_ht40to100.root",
"/scratch/mratti/NEWSnTtrees/2018/gjets_dr0p05_ht600toInf_ext1.root"};

    string files2018mcbkg_qcd[6]={"/scratch/mratti/NEWSnTtrees/2018/qcd_ht1000to1500.root",
"/scratch/mratti/NEWSnTtrees/2018/qcd_ht1500to2000.root",
"/scratch/mratti/NEWSnTtrees/2018/qcd_ht2000toInf.root",
"/scratch/mratti/NEWSnTtrees/2018/qcd_ht300to500.root",
"/scratch/mratti/NEWSnTtrees/2018/qcd_ht500to700.root",
"/scratch/mratti/NEWSnTtrees/2018/qcd_ht700to1000.root"};

    string files2018mcbkg_singletop[5]={"/scratch/mratti/NEWSnTtrees/2018/singletop_schan.root",
"/scratch/mratti/NEWSnTtrees/2018/singletop_tW_tbar.root",
"/scratch/mratti/NEWSnTtrees/2018/singletop_tW_top.root",
"/scratch/mratti/NEWSnTtrees/2018/singletop_tchan_tbar.root",
"/scratch/mratti/NEWSnTtrees/2018/singletop_tchan_top.root"};

    string files2018mcbkg_tt[5]={"/scratch/mratti/NEWSnTtrees/2018/ttdl_mg.root",
"/scratch/mratti/NEWSnTtrees/2018/ttsl_tbar_mg.root",
"/scratch/mratti/NEWSnTtrees/2018/ttsl_top_mg.root",
"/scratch/mratti/NEWSnTtrees/2018/ttz_mg.root",
"/scratch/mratti/NEWSnTtrees/2018/ttw_mg.root"};

    string files2018mcbkg_tt_negligible[1]={"/scratch/mratti/NEWSnTtrees/2018/tttt.root"};

    string files2018mcbkg_wjets[7]={"/scratch/mratti/NEWSnTtrees/2018/wjets_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2018/wjets_ht1200to2500.root",
"/scratch/mratti/NEWSnTtrees/2018/wjets_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2018/wjets_ht2500toInf.root",
"/scratch/mratti/NEWSnTtrees/2018/wjets_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2018/wjets_ht600to800.root",
"/scratch/mratti/NEWSnTtrees/2018/wjets_ht800to1200.root"};

    string files2018mcbkg_ww_wz[2]={"/scratch/mratti/NEWSnTtrees/2018/ww.root",
"/scratch/mratti/NEWSnTtrees/2018/wz.root"};

    string files2018mcbkg_zinv[7]={"/scratch/mratti/NEWSnTtrees/2018/zinv_ht100to200.root",
"/scratch/mratti/NEWSnTtrees/2018/zinv_ht1200to2500.root",
"/scratch/mratti/NEWSnTtrees/2018/zinv_ht200to400.root",
"/scratch/mratti/NEWSnTtrees/2018/zinv_ht2500toInf.root",
"/scratch/mratti/NEWSnTtrees/2018/zinv_ht400to600.root",
"/scratch/mratti/NEWSnTtrees/2018/zinv_ht600to800.root",
"/scratch/mratti/NEWSnTtrees/2018/zinv_ht800to1200.root"};

    string files2016signalT1bbbb[3]={"/scratch/mratti/NEWSnTtrees/2016/signal/T1bbbb_94x_1.root",
"/scratch/mratti/NEWSnTtrees/2016/signal/T1bbbb_94x_2.root",
"/scratch/mratti/NEWSnTtrees/2016/signal/T1bbbb_94x.root"};

    string files2016signalT1qqqq[3]={"/scratch/mratti/NEWSnTtrees/2016/signal/T1qqqq_94x_1.root",
"/scratch/mratti/NEWSnTtrees/2016/signal/T1qqqq_94x_2.root",
"/scratch/mratti/NEWSnTtrees/2016/signal/T1qqqq_94x.root"};

    string files2016signalT2bb[3]={"/scratch/mratti/NEWSnTtrees/2016/signal/T2bb_1.root",
"/scratch/mratti/NEWSnTtrees/2016/signal/T2bb_mSbot1650to2600.root",
"/scratch/mratti/NEWSnTtrees/2016/signal/T2bb.root"};

    string files2016signalT2qq[3]={"/scratch/mratti/NEWSnTtrees/2016/signal/T2qq_1.root",
"/scratch/mratti/NEWSnTtrees/2016/signal/T2qq_mSq1850to2600.root",
"/scratch/mratti/NEWSnTtrees/2016/signal/T2qq.root"};


    string files2017signalT1bbbb[8]={"/scratch/mratti/NEWSnTtrees/2017/signal/T1bbbb_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1bbbb_2.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1bbbb_ext1_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1bbbb_ext1_2.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1bbbb_ext1_3.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1bbbb_ext1_4.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1bbbb_ext1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1bbbb.root"};

    string files2017signalT1qqqq[8]={"/scratch/mratti/NEWSnTtrees/2017/signal/T1qqqq_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1qqqq_2.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1qqqq_ext1_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1qqqq_ext1_2.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1qqqq_ext1_3.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1qqqq_ext1_4.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1qqqq_ext1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T1qqqq.root"};

    string files2017signalT2bb[7]={"/scratch/mratti/NEWSnTtrees/2017/signal/T2bb_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2bb_ext1_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2bb_ext1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2bb_mSbot1650to2600_ext1_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2bb_mSbot1650to2600_ext1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2bb_mSbot1650to2600.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2bb.root"};

    string files2017signalT2qq[9]={"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq_2.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq_ext1_1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq_ext1_2.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq_ext1_3.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq_ext1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq_mSq_1850to2600_ext1.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq_mSq_1850to2600.root",
"/scratch/mratti/NEWSnTtrees/2017/signal/T2qq.root"};

    vector<string *> bkgfile;
    vector<string *> sigfile;
    int bkgfilenum[27]={7,5,6,5,6,2,7,2,7,7,5,6,5,5,2,9,2,7,7,4,6,5,5,1,7,2,7};
    int sigfilenum[8]={3,3,3,3,8,8,7,9};
    bkgfile.push_back(files2016mcbkg_dyjetsll);
    bkgfile.push_back(files2016mcbkg_gjets);
    bkgfile.push_back(files2016mcbkg_qcd);
    bkgfile.push_back(files2016mcbkg_singletop);
    bkgfile.push_back(files2016mcbkg_tt);
    bkgfile.push_back(files2016mcbkg_tt_negligible);
    bkgfile.push_back(files2016mcbkg_wjets);
    bkgfile.push_back(files2016mcbkg_ww_wz);
    bkgfile.push_back(files2016mcbkg_zinv);
    bkgfile.push_back(files2017mcbkg_dyjetsll);
    bkgfile.push_back(files2017mcbkg_gjets);
    bkgfile.push_back(files2017mcbkg_qcd);
    bkgfile.push_back(files2017mcbkg_singletop);
    bkgfile.push_back(files2017mcbkg_tt);
    bkgfile.push_back(files2017mcbkg_tt_negligible);
    bkgfile.push_back(files2017mcbkg_wjets);
    bkgfile.push_back(files2017mcbkg_ww_wz);
    bkgfile.push_back(files2017mcbkg_zinv);
    bkgfile.push_back(files2018mcbkg_dyjetsll);
    bkgfile.push_back(files2018mcbkg_gjets);
    bkgfile.push_back(files2018mcbkg_qcd);
    bkgfile.push_back(files2018mcbkg_singletop);
    bkgfile.push_back(files2018mcbkg_tt);
    bkgfile.push_back(files2018mcbkg_tt_negligible);
    bkgfile.push_back(files2018mcbkg_wjets);
    bkgfile.push_back(files2018mcbkg_ww_wz);
    bkgfile.push_back(files2018mcbkg_zinv);
  //  string bkgfilename[27]={"preselection_2016_dyjetsll","preselection_2016_gjets",
//"preselection_2016_qcd","preselection_2016_singletop","preselection_2016_tt",
//"preselection_2016_tt_negligible","preselection_2016_wjets","preselection_2016_ww_wz",
//"preselection_2016_zinv","preselection_2017_dyjetsll","preselection_2017_gjets",
//"preselection_2017_qcd","preselection_2017_singletop","preselection_2017_tt",
//"preselection_2017_tt_negligible","preselection_2017_wjets","preselection_2017_ww_wz",
//"preselection_2017_zinv","preselection_2018_dyjetsll","preselection_2018_gjets",
//"preselection_2018_qcd","preselection_2018_singletop","preselection_2018_tt",
//"preselection_2018_tt_negligible","preselection_2018_wjets","preselection_2018_ww_wz",
//"preselection_2018_zinv"};

    sigfile.push_back(files2016signalT1bbbb);
    sigfile.push_back(files2016signalT1qqqq);
    sigfile.push_back(files2016signalT2bb);
    sigfile.push_back(files2016signalT2qq);
    sigfile.push_back(files2017signalT1bbbb);
    sigfile.push_back(files2017signalT1qqqq);
    sigfile.push_back(files2017signalT2bb);
    sigfile.push_back(files2017signalT2qq);
//    string sigfilename[8]={"preselection_2016_T1bbbb","preselection_2016_T1qqqq","preselection_2016_T2bb","preselection_2016_T2qq","preselection_2017_T1bbbb","preselection_2017_T1qqqq","preselection_2017_T2bb","preselection_2017_T2qq"};

    string pathbkg="/work/wjin/featurereduced4/rootfiles_llcontrol";
    string pathsig="/work/wjin/featurereduced4/rootfiles_llcontrol";
    Int_t nJet30; Int_t nJet40; Int_t nBJet20; Int_t nBJet30; Int_t nBJet40;
    Float_t deltaPhiMin; Float_t diffMetMht; Float_t ht; Float_t mt2;Int_t nJet30FailId;
    Float_t mht_pt; Float_t mht_phi;Float_t met_pt; Float_t met_phi;
    Int_t GenSusyMScan1; Int_t GenSusyMScan2;
    Int_t njet;
    Float_t jet_pt[100]; Float_t jet_eta[100]; Float_t jet_phi[100]; Float_t jet_mass[100];
    Float_t jet_btagDeepCSV[100];Float_t evt_scale1fb;
//    Int_t nMuons10; Int_t nElectrons10; Int_t nPFLep5LowMT; Int_t nPFHad10LowMT;
//    Int_t nlep; Float_t lep_pt[10];
    Int_t nLepLowMT; Int_t nLepHighMT;Bool_t is2016=0;
//    Int_t MLtag;
    for(int i=0;i<bkgfile.size();i++){
  //  for(int i=0;i<1;i++){
        for(int count=0;count<bkgfilenum[i];count++){
            TFile *sourcefile=new TFile(bkgfile[i][count].c_str(),"read");
            TTree *sourcetree=(TTree *)sourcefile->Get("mt2");
            TString sourcefilename(bkgfile[i][count]);
            if(sourcefilename.Contains("2016")) is2016=1;
            cout<<"loading source file "<<bkgfile[i][count]<<endl;
            TFile *bkg=new TFile((pathbkg+bkgfile[i][count].substr(27)).c_str(),"recreate");
            TTree *tree=sourcetree->CloneTree(0);
            cout<<"start creating backgroud file "<<pathbkg<<bkgfile[i][count].substr(27)<<endl;
//            tree->Branch("MLtag",&MLtag,"MLtag/I");
            for(int j=0;j<15;j++){
                tree->Branch(("jet"+to_string(j+1)+"_pt").c_str(),jet_pt+j,("jet"+to_string(j+1)+"_pt/F").c_str());
                tree->Branch(("jet"+to_string(j+1)+"_eta").c_str(),jet_eta+j,("jet"+to_string(j+1)+"_eta/F").c_str());
                tree->Branch(("jet"+to_string(j+1)+"_phi").c_str(),jet_phi+j,("jet"+to_string(j+1)+"_phi/F").c_str());
                tree->Branch(("jet"+to_string(j+1)+"_mass").c_str(),jet_mass+j,("jet"+to_string(j+1)+"_mass/F").c_str());
                tree->Branch(("jet"+to_string(j+1)+"_btagDeepCSV").c_str(),jet_btagDeepCSV+j,("jet"+to_string(j+1)+"_btagDeepCSV/F").c_str());
            }
            sourcetree->SetBranchAddress("nJet30",&nJet30);
            sourcetree->SetBranchAddress("nJet40",&nJet40);
            sourcetree->SetBranchAddress("nBJet20",&nBJet20);
            sourcetree->SetBranchAddress("nBJet30",&nBJet30);
            sourcetree->SetBranchAddress("nBJet40",&nBJet40);
            sourcetree->SetBranchAddress("nJet30FailId",&nJet30FailId);
            sourcetree->SetBranchAddress("deltaPhiMin",&deltaPhiMin);
            sourcetree->SetBranchAddress("diffMetMht",&diffMetMht);
            sourcetree->SetBranchAddress("ht",&ht);
            sourcetree->SetBranchAddress("mt2",&mt2);
            sourcetree->SetBranchAddress("mht_pt",&mht_pt);
            sourcetree->SetBranchAddress("mht_phi",&mht_phi);
            sourcetree->SetBranchAddress("met_pt",&met_pt);
            sourcetree->SetBranchAddress("met_phi",&met_phi);
            sourcetree->SetBranchAddress("GenSusyMScan1",&GenSusyMScan1);
            sourcetree->SetBranchAddress("GenSusyMScan2",&GenSusyMScan2);
            sourcetree->SetBranchAddress("njet",&njet);
            sourcetree->SetBranchAddress("evt_scale1fb",&evt_scale1fb);
            sourcetree->SetBranchAddress("jet_pt",jet_pt);
            sourcetree->SetBranchAddress("jet_eta",jet_eta);
            sourcetree->SetBranchAddress("jet_phi",jet_phi);
            sourcetree->SetBranchAddress("jet_mass",jet_mass);
            sourcetree->SetBranchAddress("jet_btagDeepCSV",jet_btagDeepCSV);
//            sourcetree->SetBranchAddress("nMuons10",&nMuons10);
//            sourcetree->SetBranchAddress("nElectrons10",&nElectrons10);
            sourcetree->SetBranchAddress("nLepLowMT",&nLepLowMT);
            if(!is2016) sourcetree->SetBranchAddress("nLepHighMT",&nLepHighMT);
//            sourcetree->SetBranchAddress("nPFHad10LowMT",&nPFHad10LowMT);
//            sourcetree->SetBranchAddress("nlep",&nlep);
//            sourcetree->SetBranchAddress("lep_pt",lep_pt);
               for(int entry=0;entry<sourcetree->GetEntries();entry++){
                    for(int j=0;j<15;j++){
                        jet_pt[j]=0;
                        jet_phi[j]=0;
                        jet_eta[j]=0;
                        jet_mass[j]=0;
                        jet_btagDeepCSV[0]=0;
                    }
                    sourcetree->GetEntry(entry);
                    if(nJet30>=1&&nJet30FailId==0&&ht>250.&&((nJet30>1&&ht<1200.&&met_pt>250.)||(nJet30>1&&ht>=1200.&&met_pt>30.)||(nJet30==1 && met_pt>250.))&&deltaPhiMin>0.3&&(diffMetMht < 0.5*met_pt)&&(nLepLowMT==1&&(is2016||nLepHighMT==0))){
  //                      MLtag=rand->Integer(20);
                        tree->Fill();
                    }
                }
            tree->AutoSave();
            cout<<"saved file "<<pathbkg<<bkgfile[i][count].substr(27)<<endl;
            delete tree;
            delete bkg;
            delete sourcetree;
            delete sourcefile;
        }
    }


    for(int i=0;i<sigfile.size();i++){
        for(int count=0;count<sigfilenum[i];count++){
            TFile *sourcefile=new TFile(sigfile[i][count].c_str(),"read");
            TTree *sourcetree=(TTree *)sourcefile->Get("mt2");
            TString sourcefilename(sigfile[i][count]);
            if(sourcefilename.Contains("2016")) is2016=1;
            cout<<"loading source file "<<sigfile[i][count]<<endl;
            TFile *sig=new TFile((pathsig+sigfile[i][count].substr(27)).c_str(),"recreate");
            cout<<"start creating signal file "<<pathsig<<sigfile[i][count].substr(27)<<endl;
            TTree *tree=sourcetree->CloneTree(0);
//            tree->Branch("MLtag",&MLtag,"MLtag/I");
            for(int j=0;j<15;j++){
                tree->Branch(("jet"+to_string(j+1)+"_pt").c_str(),jet_pt+j,("jet"+to_string(j+1)+"_pt/F").c_str());
                tree->Branch(("jet"+to_string(j+1)+"_eta").c_str(),jet_eta+j,("jet"+to_string(j+1)+"_eta/F").c_str());
                tree->Branch(("jet"+to_string(j+1)+"_phi").c_str(),jet_phi+j,("jet"+to_string(j+1)+"_phi/F").c_str());
                tree->Branch(("jet"+to_string(j+1)+"_mass").c_str(),jet_mass+j,("jet"+to_string(j+1)+"_mass/F").c_str());
                tree->Branch(("jet"+to_string(j+1)+"_btagDeepCSV").c_str(),jet_btagDeepCSV+j,("jet"+to_string(j+1)+"_btagDeepCSV/F").c_str());
            }
            sourcetree->SetBranchAddress("nJet30",&nJet30);
            sourcetree->SetBranchAddress("nJet40",&nJet40);
            sourcetree->SetBranchAddress("nBJet20",&nBJet20);
            sourcetree->SetBranchAddress("nBJet30",&nBJet30);
            sourcetree->SetBranchAddress("nBJet40",&nBJet40);
            sourcetree->SetBranchAddress("nJet30FailId",&nJet30FailId);
            sourcetree->SetBranchAddress("deltaPhiMin",&deltaPhiMin);
            sourcetree->SetBranchAddress("diffMetMht",&diffMetMht);
            sourcetree->SetBranchAddress("ht",&ht);
            sourcetree->SetBranchAddress("mt2",&mt2);
            sourcetree->SetBranchAddress("mht_pt",&mht_pt);
            sourcetree->SetBranchAddress("mht_phi",&mht_phi);
            sourcetree->SetBranchAddress("met_pt",&met_pt);
            sourcetree->SetBranchAddress("met_phi",&met_phi);
            sourcetree->SetBranchAddress("GenSusyMScan1",&GenSusyMScan1);
            sourcetree->SetBranchAddress("GenSusyMScan2",&GenSusyMScan2);
            sourcetree->SetBranchAddress("njet",&njet);
            sourcetree->SetBranchAddress("evt_scale1fb",&evt_scale1fb);
            sourcetree->SetBranchAddress("jet_pt",jet_pt);
            sourcetree->SetBranchAddress("jet_eta",jet_eta);
            sourcetree->SetBranchAddress("jet_phi",jet_phi);
            sourcetree->SetBranchAddress("jet_mass",jet_mass);
            sourcetree->SetBranchAddress("jet_btagDeepCSV",jet_btagDeepCSV);
            sourcetree->SetBranchAddress("nLepLowMT",&nLepLowMT);
            if(!is2016) sourcetree->SetBranchAddress("nLepHighMT",&nLepHighMT);
//            sourcetree->SetBranchAddress("nMuons10",&nMuons10);
 //           sourcetree->SetBranchAddress("nElectrons10",&nElectrons10);
 //           sourcetree->SetBranchAddress("nPFLep5LowMT",&nPFLep5LowMT);
 //           sourcetree->SetBranchAddress("nPFHad10LowMT",&nPFHad10LowMT);
//            sourcetree->SetBranchAddress("nlep",&nlep);
//            sourcetree->SetBranchAddress("lep_pt",lep_pt);
            for(int entry=0;entry<sourcetree->GetEntries();entry++){
                for(int j=0;j<15;j++){
                    jet_pt[j]=0;
                    jet_phi[j]=0;
                    jet_eta[j]=0;
                    jet_mass[j]=0;
                    jet_btagDeepCSV[j]=0;
                }
                sourcetree->GetEntry(entry);
                if(nJet30>=1&&nJet30FailId==0&&ht>250.&&((nJet30>1&&ht<1200.&&met_pt>250.)||(nJet30>1&&ht>=1200.&&met_pt>30.)||(nJet30==1 && met_pt>250.))&&deltaPhiMin>0.3&&(diffMetMht < 0.5*met_pt)&&(nLepLowMT==1&&(is2016||nLepHighMT==0))){
 //                   MLtag=rand->Integer(20);
                    tree->Fill();
//                    selectedentry++;
 //                   if((selectedentry+1)%20000==0) tree->AutoSave();
                    }
            }
//           delete sourcetree;
            tree->AutoSave();
            cout<<"saved file "<<pathsig<<sigfile[i][count].substr(27)<<endl;
            delete tree;
            delete sig;
            delete sourcefile;
        }
    }
    return 1;


}
