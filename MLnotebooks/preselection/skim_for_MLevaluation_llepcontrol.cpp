#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include "TMath.h"
 //#include "TH1D.h"
 //#include "TH2D.h"
 //#include "TH3D.h"
 //#include "THStack.h"
 //#include "TCanvas.h"
 //#include "TLegend.h"
 //#include "TPaveText.h"
 //#include "TGraphAsymmErrors.h"
 //#include "TLorentzVector.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
using namespace std;
int skim_for_MLevaluation_llepcontrol(const char *inputfile,const char *outputfile,const char* treename,bool ML){
    TRandom *rand= new TRandom(1234);
    Float_t deltaPhiMin; Float_t ht; Float_t mt2;
    Float_t mht_pt; Float_t met_pt;Float_t diffMetMht;
    Int_t nJet30; Int_t nJet30FailId;
    Float_t jet_pt[100]; Float_t jet_eta[100]; Float_t jet_phi[100];// Float_t jet_mass[100];
    Float_t jet_btagDeepCSV[100];
//    Int_t nMuons10; Int_t nElectrons10; Int_t nPFLep5LowMT; Int_t nPFHad10LowMT;
    Int_t nLepLowMT; Int_t nLepHighMT;Bool_t is2016=0;
    Int_t MLtag;
    TFile *sourcefile=new TFile(inputfile,"read");
    TTree *sourcetree=(TTree *)sourcefile->Get(treename);
    cout<<"loading source file "<<inputfile<<endl;
    TString sourcefilename(inputfile);
    if(sourcefilename.Contains("2016")) is2016=1;
    TFile *bkg=new TFile(outputfile,"recreate");
    TTree *tree=sourcetree->CloneTree(0);
    cout<<"start creating skimmed file "<<outputfile<<endl;
    if(ML) tree->Branch("MLtag",&MLtag,"MLtag/I");
    for(int j=0;j<15;j++){
        tree->Branch(("jet"+to_string(j+1)+"_pt").c_str(),jet_pt+j,("jet"+to_string(j+1)+"_pt/F").c_str());
        tree->Branch(("jet"+to_string(j+1)+"_eta").c_str(),jet_eta+j,("jet"+to_string(j+1)+"_eta/F").c_str());
        tree->Branch(("jet"+to_string(j+1)+"_phi").c_str(),jet_phi+j,("jet"+to_string(j+1)+"_phi/F").c_str());
//                tree->Branch(("jet"+to_string(j+1)+"_mass").c_str(),jet_mass+j,("jet"+to_string(j+1)+"_mass/F").c_str());
        tree->Branch(("jet"+to_string(j+1)+"_btagDeepCSV").c_str(),jet_btagDeepCSV+j,("jet"+to_string(j+1)+"_btagDeepCSV/F").c_str());
    }        
    sourcetree->SetBranchAddress("deltaPhiMin",&deltaPhiMin);
    sourcetree->SetBranchAddress("ht",&ht);
    sourcetree->SetBranchAddress("mt2",&mt2);
    sourcetree->SetBranchAddress("mht_pt",&mht_pt);
    sourcetree->SetBranchAddress("met_pt",&met_pt);
    sourcetree->SetBranchAddress("nJet30",&nJet30);
    sourcetree->SetBranchAddress("nJet30FailId",&nJet30FailId);
    sourcetree->SetBranchAddress("diffMetMht",&diffMetMht);
    sourcetree->SetBranchAddress("jet_pt",jet_pt);
    sourcetree->SetBranchAddress("jet_eta",jet_eta);
    sourcetree->SetBranchAddress("jet_phi",jet_phi);
 //           sourcetree->SetBranchAddress("jet_mass",jet_mass);
    sourcetree->SetBranchAddress("jet_btagDeepCSV",jet_btagDeepCSV);
//    sourcetree->SetBranchAddress("nMuons10",&nMuons10);
//    sourcetree->SetBranchAddress("nElectrons10",&nElectrons10);
//    sourcetree->SetBranchAddress("nPFLep5LowMT",&nPFLep5LowMT);
//    sourcetree->SetBranchAddress("nPFHad10LowMT",&nPFHad10LowMT);
    sourcetree->SetBranchAddress("nLepLowMT",&nLepLowMT);
    if(!is2016) sourcetree->SetBranchAddress("nLepHighMT",&nLepHighMT);
    int skimnum=0;
    for(int entry=0;entry<sourcetree->GetEntries();entry++){
        for(int j=0;j<15;j++){
            jet_pt[j]=0;
            jet_phi[j]=0;
            jet_eta[j]=0;
 //                       jet_mass[j]=0;
            jet_btagDeepCSV[0]=0;
        }
        sourcetree->GetEntry(entry);
        if(nJet30>=1&&nJet30FailId==0&&ht>250.&&((nJet30>1&&ht<1200.&&met_pt>250.)||(nJet30>1&&ht>=1200.&&met_pt>30.)||(nJet30==1 && met_pt>250.))&&deltaPhiMin>0.3&&(diffMetMht < 0.5*met_pt)&&(nLepLowMT==1&&(is2016||nLepHighMT==0))){
           if(ML) MLtag=rand->Integer(20);
           tree->Fill();
           skimnum++;
        }
        if(!(entry%100000)) cout<<"skimmed "<<entry<<"events, get "<<skimnum<<"events"<<endl;
    }
    tree->AutoSave();
    cout<<"saved file "<<outputfile<<endl;
    delete tree;
    delete bkg;
    delete sourcetree;
    delete sourcefile;
    return 1;    
    
}

