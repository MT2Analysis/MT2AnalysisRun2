#include "../interface/MT2EstimateTree.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include "TCanvas.h"

using namespace std;


MT2EstimateTree::MT2EstimateTree( const std::string& aname, const MT2Region& aregion ) : MT2Estimate( aname, aregion ) {

  //TH1::AddDirectory(kFALSE);

  tree = new TTree(this->getHistoName("tree").c_str(), "" );
  tree->SetDirectory(0);
  
  this->initTree();

}




MT2EstimateTree::MT2EstimateTree( const MT2EstimateTree& rhs ) : MT2Estimate( rhs ) {
 
  //TH1::AddDirectory(kFALSE);

  tree = rhs.tree->CloneTree(-1);

  tree->SetDirectory(0);

  this->initTree();

}



MT2EstimateTree::~MT2EstimateTree() {

  delete tree;

}






void MT2EstimateTree::initTree( ) {

  tree->Branch( "run", &run, "run/I");
  tree->Branch( "lumi", &lumi, "lumi/I");
  tree->Branch( "evt", &evt, "evt/L");
  tree->Branch( "weight", &weight, "weight/F");
  tree->Branch( "puWeight", &puWeight, "puWeight/F");
  tree->Branch( "id", &id, "id/I");

  tree->Branch( "mt2", &mt2, "mt2/F");
  tree->Branch( "ht", &ht, "ht/F");

  tree->Branch( "met", &met, "met/F");
  tree->Branch( "nJets", &nJets, "nJets/I");
  tree->Branch( "nJet30HEMFail", &nJet30HEMFail, "nJet30HEMFail/I");
  tree->Branch( "nBJets", &nBJets, "nBJets/I");
  
  tree->Branch( "nJetHF", &nJetHF, "nJetHF/I");
  
  tree->Branch( "deltaPhiMin", &deltaPhiMin, "deltaPhiMin/F");
  tree->Branch( "diffMetMht", &diffMetMht, "diffMetMht/F");
  tree->Branch( "nVert", &nVert, "nVert/I");
  tree->Branch( "rho", &rho, "rho/F");
  tree->Branch( "met_pt", &met_pt, "met_pt/F");
  tree->Branch( "met_phi", &met_phi, "met_phi/F");
  tree->Branch( "mht_pt", &mht_pt, "mht_pt/F");
  tree->Branch( "mht_phi", &mht_phi, "mht_phi/F");

  tree->Branch( "nElectrons", &nElectrons, "nElectrons/I");
  tree->Branch( "nMuons", &nMuons, "nMuons/I");
  tree->Branch( "nPFLep", &nPFLep, "nPFLep/I");
  tree->Branch( "nPFHad", &nPFHad, "nPFHad/I");

  tree->Branch( "lepPt", &lepPt, "lepPt/F");
  tree->Branch( "lepEta", &lepEta, "lepEta/F");
  tree->Branch( "lepPhi", &lepPhi, "lepPhi/F");
  tree->Branch( "lepMass", &lepMass, "lepMass/F");
  tree->Branch( "lepID", &lepID, "lepID/F");
  tree->Branch( "lepMiniRelIso", &lepMiniRelIso, "lepMiniRelISo/F");
  tree->Branch( "isotrackPt", &isotrackPt, "isotrackPt/F");
  tree->Branch( "isotrackEta", &isotrackEta, "isotrackEta/F");
  tree->Branch( "isotrackPhi", &isotrackPhi, "isotrackPhi/F");
  tree->Branch( "isotrack_pdgId", &isotrack_pdgId, "isotrack_pdgId/I");

  //tree->Branch( "GenSusyMScan1", &GenSusyMScan1, "GenSusyMScan1/I");
  //tree->Branch( "GenSusyMScan2", &GenSusyMScan2, "GenSusyMScan2/I");

  //  tree->Branch( "weight_isr", &weight_isr, "weight_isr/F" );
  
  //  tree->Branch( "weight_scales", &weight_scales, "weight_scales[111]/F" );

//  tree->Branch( "LHEweight_original", &LHEweight_original, "LHEweight_original/F" );
//  tree->Branch( "LHEweight_id", &LHEweight_id, "LHEweight_id[111]/I" );
//  tree->Branch( "LHEweight_wgt", &LHEweight_wgt, "LHEweight_wgt[111]/F" );

  tree->SetDirectory(0);
}

void MT2EstimateTree::initTree4read( ) {

  tree->SetBranchAddress( "run"          , &run          );
  tree->SetBranchAddress( "lumi"         , &lumi         );
  tree->SetBranchAddress( "evt"          , &evt          );
  tree->SetBranchAddress( "weight"       , &weight       );
  tree->SetBranchAddress( "puWeight"     , &puWeight     );
  tree->SetBranchAddress( "id"           , &id           );
  tree->SetBranchAddress( "mt2"          , &mt2          );
  tree->SetBranchAddress( "ht"           , &ht           );
  tree->SetBranchAddress( "met"          , &met          );
  tree->SetBranchAddress( "nJets"        , &nJets        );
  tree->SetBranchAddress( "nJet30HEMFail"        , &nJet30HEMFail        );
  tree->SetBranchAddress( "nBJets"       , &nBJets       );
  tree->SetBranchAddress( "nJetHF"       , &nJetHF       );
  tree->SetBranchAddress( "nJet30HEMFail", &nJet30HEMFail);
  tree->SetBranchAddress( "deltaPhiMin"  , &deltaPhiMin  );
  tree->SetBranchAddress( "diffMetMht"   , &diffMetMht   );
  tree->SetBranchAddress( "nVert"        , &nVert        );
  tree->SetBranchAddress( "rho"          , &rho          );
  tree->SetBranchAddress( "met_pt"       , &met_pt       );
  tree->SetBranchAddress( "met_phi"      , &met_phi      );
  tree->SetBranchAddress( "mht_pt"       , &mht_pt       );
  tree->SetBranchAddress( "mht_phi"      , &mht_phi      );
  tree->SetBranchAddress( "nElectrons"   , &nElectrons   );
  tree->SetBranchAddress( "nMuons"       , &nMuons       );
  tree->SetBranchAddress( "nPFLep"       , &nPFLep       );
  tree->SetBranchAddress( "nPFHad"       , &nPFHad       );
  tree->SetBranchAddress( "GenSusyMScan1", &GenSusyMScan1);
  tree->SetBranchAddress( "GenSusyMScan2", &GenSusyMScan2);
  tree->SetBranchAddress( "lepPt"        , &lepPt        );
  tree->SetBranchAddress( "lepEta"       , &lepEta       );
  tree->SetBranchAddress( "lepPhi"       , &lepPhi       );
  tree->SetBranchAddress( "lepMass"      , &lepMass      );
  tree->SetBranchAddress( "lepID"        , &lepID        );
  tree->SetBranchAddress( "lepMiniRelIso", &lepMiniRelIso); 
  tree->SetBranchAddress( "isotrackPt"   , &isotrackPt   );
  tree->SetBranchAddress( "isotrackEta"  , &isotrackEta  );
  tree->SetBranchAddress( "isotrackPhi"  , &isotrackPhi  );
  tree->SetBranchAddress( "isotrack_pdgId", &isotrack_pdgId);

  for (std::map< std::string, float* >::iterator i= extraVars.begin(); i!=extraVars.end(); i++)
    tree->SetBranchAddress(i->first.c_str(), (i->second));

  for (std::map< std::string, std::vector<float>* >::iterator i= extraVectors.begin(); i!=extraVectors.end(); i++)
    tree->SetBranchAddress(i->first.c_str(), &(i->second));

}




void MT2EstimateTree::setName( const std::string& newName ) {

  MT2Estimate::setName(newName);

  tree->SetName( this->getHistoName("tree").c_str() );

}



//void MT2EstimateTree::projectFromTree( const MT2EstimateTree* treeEst, const std::string& selection ) {
//
//  TDirectory* dir = TDirectory::CurrentDirectory();
//
//  std::string fullSelection = region->getRegionCuts();
//  if( selection!="" ) fullSelection = fullSelection + " && " + selection;
//
//  treeEst->tree->Project( yield->GetName(), "mt2", Form("weight*(%s)", fullSelection.c_str()) );
//
//  gROOT->cd();
//
//  this->tree = treeEst->tree->CopyTree( Form("%s", fullSelection.c_str()) );
//  this->tree->SetDirectory(0);
//  this->tree->SetName( this->getHistoName("tree").c_str() );
//
//  dir->cd();
//
//}



//MT2Analysis<MT2EstimateTree>* MT2EstimateTree::makeRebinnedAnalysisFromInclusiveTree( const std::string& aname, const std::string& regionsSet, MT2Analysis<MT2EstimateTree>* estimate, const std::string& selection, int nBins, float xMin, float xMax ) {
//
//
//  std::set<MT2Region> regions = estimate->getRegions();
//
//  //  MT2EstimateTree* treeInclusive = estimate->get( MT2Region("HT450toInf_j2toInf_b0toInf") );
//  if( regions.size()!=1 ) {
//  //if( treeInclusive==0 ) {
//    std::cout << "[MT2EstimateTree::makeAnalysisFromEstimateTreeInclusive] ERROR!! You need to pass an inclusive MT2EstimateTree Analysis to use this function!" << std::endl;
//    exit(19191);
//  }
//
//  MT2EstimateTree* treeInclusive = estimate->get( *(regions.begin()) );
//
//  // will create a new analysis with custom regions from inclusive tree:
//  MT2Analysis<MT2EstimateTree>* analysis = new MT2Analysis<MT2EstimateTree>( aname, regionsSet );
//  std::set<MT2Region> newRegions = analysis->getRegions();
//
//  if ( nBins!=0 )
//    MT2Estimate::rebinYields( (MT2Analysis<MT2Estimate>*)analysis, nBins, xMin, xMax );
//
//  for( std::set<MT2Region>::iterator iR=newRegions.begin(); iR!=newRegions.end(); ++iR ) {
//
//    MT2EstimateTree* thisEstimateTree = analysis->get( *iR );
//    thisEstimateTree->projectFromTree( treeInclusive, selection );
//
//  } // for regions
//
//
//  return analysis;
//
//}



void MT2EstimateTree::projectFromTree( const MT2EstimateTree* treeEst, const std::string& selection, const std::string& variable ) {

  TDirectory* dir = TDirectory::CurrentDirectory();

  std::string fullSelection = region->getRegionCuts();
 
  if( selection!="" ) fullSelection = fullSelection + " && " + selection;
  
  treeEst->tree->Project( yield->GetName(), variable.c_str(), Form("weight*(%s)", fullSelection.c_str()) );
  
  //can be useful in case of debugging, simply plot the MT2 shape in a given region. Create dir /test beforehand
  //TCanvas* c = new TCanvas();
  //c->cd();
  //yield->Draw();
  //TString regionName = region->getName();
  //TString yieldName = yield->GetName();
  //c->SaveAs("test/" + yieldName + "_yield.pdf");
     
  gROOT->cd();

  this->tree = treeEst->tree->CopyTree( Form("%s", fullSelection.c_str()) );
  this->tree->SetDirectory(0);
  this->tree->SetName( this->getHistoName("tree").c_str() );
  
  dir->cd();
  
}

// //Analysis with the bins defined by the region set, and also applying a selection
// MT2Analysis<MT2EstimateTree>* MT2EstimateTree::makeAnalysisFromInclusiveTree( const std::string& aname, const std::string& regionsSet, MT2Analysis<MT2EstimateTree>* estimate, const std::string& selection, const std::string& variable ) {
//
//   std::set<MT2Region> regions = estimate->getRegions();
//
//   if( regions.size()!=1 ) {
//     std::cout << "[MT2EstimateTree::makeAnalysisFromEstimateTreeInclusive] ERROR!! You need to pass an inclusive MT2EstimateTree Analysis to use this function!" << std::endl;
//     exit(19191);
//   }
//
//   MT2EstimateTree* treeInclusive = estimate->get( *(regions.begin()) );
//
//   // will create a new analysis with custom regions from inclusive tree:
//   MT2Analysis<MT2EstimateTree>* analysis = new MT2Analysis<MT2EstimateTree>( aname, regionsSet );
//   std::set<MT2Region> newRegions = analysis->getRegions();
//
//   for( std::set<MT2Region>::iterator iR=newRegions.begin(); iR!=newRegions.end(); ++iR ) {
// 
//     MT2EstimateTree* thisEstimateTree = analysis->get( *iR );
//     thisEstimateTree->projectFromTree( treeInclusive, selection, variable );
//
//   } // for regions
//
//   return analysis;
//
// }


MT2Analysis<MT2EstimateTree>* MT2EstimateTree::makeRebinnedAnalysisFromInclusiveTree( const std::string& aname, const std::string& regionsSet, MT2Analysis<MT2EstimateTree>* estimate, int nBins, float xMin, float xMax, const std::string& selection, const std::string& variable ) {

  std::set<MT2Region> regions = estimate->getRegions();

  if( regions.size()!=1 ) {
    std::cout << "[MT2EstimateTree::makeAnalysisFromEstimateTreeInclusive] ERROR!! You need to pass an inclusive MT2EstimateTree Analysis to use this function!" << std::endl;
    exit(19191);
  }
  
  MT2EstimateTree* treeInclusive = estimate->get( *(regions.begin()) );

  // will create a new analysis with custom regions from inclusive tree:
  MT2Analysis<MT2EstimateTree>* analysis = new MT2Analysis<MT2EstimateTree>( aname, regionsSet );
  std::set<MT2Region> newRegions = analysis->getRegions();
  
  //this will only rebin if the new number of bins has been set, otherwise binning of analysis remains
  if ( nBins > 0 )
    MT2Estimate::rebinYields( (MT2Analysis<MT2Estimate>*) analysis, nBins, xMin, xMax );
  
  for( std::set<MT2Region>::iterator iR=newRegions.begin(); iR!=newRegions.end(); ++iR ) {
   
    MT2EstimateTree* thisEstimateTree = analysis->get( *iR );
    thisEstimateTree->projectFromTree( treeInclusive, selection, variable );

  } // for regions

  return analysis;

}



MT2Analysis<MT2EstimateTree>* MT2EstimateTree::makeRebinnedAnalysisFromInclusiveTree( const std::string& aname, const std::string& regionsSet, MT2Analysis<MT2EstimateTree>* estimate, const std::string& selection, int nBins, double* bins, const std::string& variable ) {

  //cout << "Rebinned function 2" << endl;

  std::set<MT2Region> regions = estimate->getRegions();

  if( regions.size()!=1 ) {
    std::cout << "[MT2EstimateTree::makeAnalysisFromEstimateTreeInclusive] ERROR!! You need to pass an inclusive MT2EstimateTree Analysis to use this function!" << std::endl;
    exit(19191);
  }
  
  MT2EstimateTree* treeInclusive = estimate->get( *(regions.begin()) );

  // will create a new analysis with custom regions from inclusive tree:
  MT2Analysis<MT2EstimateTree>* analysis = new MT2Analysis<MT2EstimateTree>( aname, regionsSet );
  std::set<MT2Region> newRegions = analysis->getRegions();

  if ( nBins!=0 ){
    MT2Estimate::rebinYields( (MT2Analysis<MT2Estimate>*)analysis, nBins, bins );
  }

  for( std::set<MT2Region>::iterator iR=newRegions.begin(); iR!=newRegions.end(); ++iR ) {
    MT2Region* thisRegion = new MT2Region(*iR);
    //cout << "Region: " << thisRegion->getName() << endl;

    MT2EstimateTree* thisEstimateTree = analysis->get( *iR );
    thisEstimateTree->projectFromTree( treeInclusive, selection, variable );
  } // for regions

  return analysis;

}


void MT2EstimateTree::addVar( MT2Analysis<MT2EstimateTree>* analysis, const std::string& name ) {


  for( std::set<MT2EstimateTree*>::iterator iD=analysis->data.begin(); iD!=analysis->data.end(); ++iD ) {

    float* x = new float();
    (*iD)->extraVars[name] = x;

    (*iD)->tree->Branch( name.c_str(), x, Form("%s/F", name.c_str()) );

  }

}

void MT2EstimateTree::addVector( MT2Analysis<MT2EstimateTree>* analysis, const std::string& name ) {


  for( std::set<MT2EstimateTree*>::iterator iD=analysis->data.begin(); iD!=analysis->data.end(); ++iD ) {

    std::vector<float> *x = new std::vector<float>();
    (*iD)->extraVectors[name] = x;

    (*iD)->tree->Branch( name.c_str(), x );

  }

}


//void MT2EstimateTree::addVarFloat( MT2Analysis<MT2EstimateTree>* analysis, const std::string& name ) {
//
//
//  for( std::set<MT2EstimateTree*>::iterator iD=analysis->data.begin(); iD!=analysis->data.end(); ++iD ) {
//
//    float* x = new float();
//    (*iD)->extraVars[name] = (size_t)x;
//
//    (*iD)->tree->Branch( name.c_str(), x, Form("%s/F", name.c_str()) );
//
//  }
//
//}



//void MT2EstimateTree::addVarInt( MT2Analysis<MT2EstimateTree>* analysis, const std::string& name ) {
//
//
//  for( std::set<MT2EstimateTree*>::iterator iD=analysis->data.begin(); iD!=analysis->data.end(); ++iD ) {
//
//    int* x = new int();
//    (*iD)->extraVars[name] = (size_t)x;
//
//    (*iD)->tree->Branch( name.c_str(), x, Form("%s/I", name.c_str()) );
//
//  }
//
//}



void MT2EstimateTree::assignVar( const std::string& name, float value ) {

  //&(extraVars[name]) = value;
  float* x = (float*)extraVars[name];
  *x = value;

}

void MT2EstimateTree::assignVector( const std::string& name, std::vector<float> vec ) {

  std::vector<float>* x = (std::vector<float>*)extraVectors[name];
  *x = vec;

}





void MT2EstimateTree::fillTree( const MT2Tree& mt2tree, float w  ) {
  this->assignTree( mt2tree, w  );
  tree->Fill();
}



void MT2EstimateTree::fillTree_zll( const MT2Tree& mt2tree, float w ) {
  this->assignTree_zll( mt2tree, w );
  tree->Fill();
}




void MT2EstimateTree::assignTree( const MT2Tree& mt2tree, float w  ) {

  run    = mt2tree.run;
  lumi   = mt2tree.luminosityBlock;
  evt    = mt2tree.evt;
  weight = w;
//  puWeight = mt2tree.puWeight;
  id     = mt2tree.evt_id;

  nVert  = mt2tree.PV_npvsGood;
  
//  rho  = mt2tree.rho;
 

  if(mt2tree.nJet30>1)
    mt2    = mt2tree.mt2;
  else if(mt2tree.nJet30==1)
    mt2    = mt2tree.ht;

  ht      = mt2tree.ht;
  met     = mt2tree.met_pt;
  met_pt  = mt2tree.met_pt;
  met_phi = mt2tree.met_phi;
  mht_pt  = mt2tree.mht_pt;
  mht_phi = mt2tree.mht_phi;
  

  nJets  = mt2tree.nJet30;
  nJet30HEMFail = mt2tree.nJet30HEMFail;
  nBJets = mt2tree.nBJet20;

  deltaPhiMin   = mt2tree.deltaPhiMin;
  diffMetMht    = mt2tree.diffMetMht;
  nElectrons    = mt2tree.nElectrons10;
  nMuons        = mt2tree.nMuons10;
  nPFLep        = mt2tree.nPFLep5LowMT;
  nPFHad        = mt2tree.nPFHad10LowMT;

//  nJetHF = mt2tree.get_nJetHF();

  lepPt    = mt2tree.lep_pt[0];
  lepEta   = mt2tree.lep_eta[0];
  lepPhi   = mt2tree.lep_phi[0];
  lepMass  = mt2tree.lep_mass[0];
  lepID    = mt2tree.lep_pdgId[0];
  lepMiniRelIso = mt2tree.lep_miniRelIso[0];
  isotrackPt    = mt2tree.isoTrack_pt[0];
  isotrackEta   = mt2tree.isoTrack_eta[0];
  isotrackPhi   = mt2tree.isoTrack_phi[0];
  isotrack_pdgId = mt2tree.isoTrack_pdgId[0];
    
  //GenSusyMScan1 = mt2tree.GenSusyMGluino;
  //GenSusyMScan2 = mt2tree.GenSusyMNeutralino;
 
  //  weight_isr = mt2tree.weight_isr;
//  for (int i=0; i < 111; ++i){
//    weight_scales[i] = mt2tree.weight_scales[i];
//  }

//  LHEweight_original = mt2tree.LHEweight_original;
//  for (int i=0; i < 111; ++i){
//    LHEweight_id[i] = mt2tree.LHEweight_id[i];
//    LHEweight_wgt[i] = mt2tree.LHEweight_wgt[i];
//  }

}
  

void MT2EstimateTree::assignTree_zll( const MT2Tree& mt2tree, float w ) {

  run    = mt2tree.run;
  lumi   = mt2tree.luminosityBlock;
  evt    = mt2tree.event;
  weight = w;
//  puWeight = mt2tree.puWeight;
  id     = mt2tree.evt_id;

  nVert  = mt2tree.PV_npvsGood;


  if(mt2tree.nJet30>1)
    mt2    = mt2tree.zll_mt2;
  else if(mt2tree.nJet30==1)
    mt2           = mt2tree.zll_ht;
 
  ht            = mt2tree.zll_ht;
  met           = mt2tree.zll_met_pt;

  deltaPhiMin   = mt2tree.zll_deltaPhiMin;
  diffMetMht    = mt2tree.zll_diffMetMht;


  nJets         = mt2tree.nJet30;
  nJet30HEMFail = mt2tree.nJet30HEMFail;
  nBJets        = mt2tree.nBJet20;

  nElectrons    = mt2tree.nElectrons10;
  nMuons        = mt2tree.nMuons10;
  nPFLep        = mt2tree.nPFLep5LowMT;
  nPFHad        = mt2tree.nPFHad10LowMT;

  //nJetHF = mt2tree.get_nJetHF();

  //GenSusyMScan1 = mt2tree.GenSusyMGluino;
  //GenSusyMScan2 = mt2tree.GenSusyMNeutralino;

//  LHEweight_original = mt2tree.LHEweight_original;
//  for (int i=0; i < 446; ++i){
//    LHEweight_id[i] = mt2tree.LHEweight_id[i];
//    LHEweight_wgt[i] = mt2tree.LHEweight_wgt[i];
//  }

}


void MT2EstimateTree::assignVars( float aht, int anJets, int anBJets, float amet, float amt2 ) {

  mt2    = amt2;
  ht     = aht;
  met    = amet;
  nJets  = anJets;
  nBJets = anBJets;

}
  




void MT2EstimateTree::getShit( TFile* file, const std::string& path ) {

  MT2Estimate::getShit(file, path);

  tree = (TTree*)file->Get(Form("%s/%s", path.c_str(), tree->GetName()));

  //this->initTree();

}



void MT2EstimateTree::write() const {

  MT2Estimate::write();
  tree->Write();

}


void MT2EstimateTree::print(const std::string& ofs){

  MT2Estimate::print( ofs );

}


const MT2EstimateTree& MT2EstimateTree::operator=( const MT2EstimateTree& rhs ) {

  this->region = new MT2Region(*(rhs.region));

  this->yield3d = new TH3D(*(rhs.yield3d));
  this->yield = new TH1D(*(rhs.yield));

  this->tree = rhs.tree->CloneTree(-1);

  this->setName( this->getName() );

  this->initTree();

  return *this;

}




MT2EstimateTree MT2EstimateTree::operator+( const MT2EstimateTree& rhs ) const{


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateTree::operator+] ERROR! Can't add MT2EstimateTree with different MT2Regions!" << std::endl;
    exit(113);
  }


  MT2EstimateTree result(*this);
  result.yield->Add(rhs.yield);
  result.yield3d->Add(rhs.yield3d);

  TList* list = new TList();
  list->Add(result.tree);
  list->Add(rhs.tree);
  result.tree = TTree::MergeTrees( list );

  result.initTree();


  return result;
  
}





MT2EstimateTree MT2EstimateTree::operator*( float k ) const{

  MT2EstimateTree result( this->getName(), *(this->region) );
  result.yield3d = new TH3D(*(this->yield3d));
  result.yield3d->Scale(k);
  
  result.yield = new TH1D(*(this->yield));
  result.yield->Scale(k);
  
  // loop on entries and multiply weight

  return result;


}


MT2EstimateTree MT2EstimateTree::operator/( float k ) const{

  MT2EstimateTree result( this->getName(), *(this->region) );
  result.yield3d = new TH3D(*(this->yield3d));
  result.yield3d->Scale(1./k);
  
  result.yield = new TH1D(*(this->yield));
  result.yield->Scale(1./k);

  
  // loop on entries and multiply weight

  return result;


}



const MT2EstimateTree& MT2EstimateTree::operator+=( const MT2EstimateTree& rhs ) {


  if( rhs.tree->GetEntries()>0 ) {

    this->yield->Add(rhs.yield);
    this->yield3d->Add(rhs.yield3d);

    std::string oldName(this->tree->GetName());

    TList* list = new TList();
    list->Add(this->tree);
    list->Add(rhs.tree);
    this->tree = TTree::MergeTrees( list );
    this->tree->SetName( oldName.c_str() );

    //    this->initTree();

    delete list;

  }

  return (*this);

}





const MT2EstimateTree& MT2EstimateTree::operator*=( float k ) {

  this->yield->Scale(k);
  this->yield3d->Scale(k);
  // loop on entries and multiply weight
  return (*this);

}


const MT2EstimateTree& MT2EstimateTree::operator/=( float k ) {

  this->yield->Scale(1./k);
  this->yield3d->Scale(1./k);
  // loop on entries and multiply weight
  return (*this);

}



// friend functions:

MT2EstimateTree operator*( float k, const MT2EstimateTree& rhs ) {

  return rhs*k;

}



MT2EstimateTree operator/( float k, const MT2EstimateTree& rhs ) {

  return rhs/k;

}

