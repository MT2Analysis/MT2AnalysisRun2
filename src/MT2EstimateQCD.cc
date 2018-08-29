#include "../interface/MT2EstimateQCD.h"
#include "../interface/MT2EstimateTree.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include "TRandom3.h"
#include "TMath.h"


MT2EstimateQCD::MT2EstimateQCD( const std::string& aname, const MT2Region& aregion ) : MT2EstimateTree( aname, aregion ) {

  int nBins;
  double* bins;
  region->getBins_qcdCR(nBins, bins);

  lDphi = new TH1D( this->getHistoName("lDphi").c_str(), "", nBins, bins); lDphi->Sumw2();
  hDphi = new TH1D( this->getHistoName("hDphi").c_str(), "", nBins, bins); hDphi->Sumw2();

  dphi_low = 0.3;

}




void MT2EstimateQCD::projectFromTree( const MT2EstimateTree* treeEst, const std::string& selectionTree, const std::string& selectionDphi ) {

  std::string selection_tree = "deltaPhiMin<0.3 && mt2>40.";
  if( selectionTree!="" ) selection_tree = selection_tree + " && " + selectionTree;
  MT2EstimateTree::projectFromTree( treeEst, selection_tree );

  std::string fullSelection = region->getRegionCuts();
  //if( selection    !="" ) fullSelection = fullSelection + " && " + selection;
  if( selectionDphi!="" ) fullSelection = fullSelection + " && " + selectionDphi;

  treeEst->tree->Project( lDphi->GetName(), "mt2", Form("weight*(deltaPhiMin<0.3 && %s)", fullSelection.c_str()) );
  treeEst->tree->Project( hDphi->GetName(), "mt2", Form("weight*(deltaPhiMin>0.3 && %s)", fullSelection.c_str()) );

}



// not sure this method works:
MT2Analysis<MT2EstimateQCD>* MT2EstimateQCD::makeAnalysisFromTree( const std::string& aname, MT2Analysis<MT2EstimateTree>* estimate, const std::string& selectionTree, const std::string& selectionDphi ) {

  std::set<MT2Region> regions = estimate->getRegions();

  std::set<MT2EstimateQCD*> data;

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    MT2EstimateTree*  thisEstimate = estimate->get( *iR );
    MT2EstimateQCD* thisEstimateQCD = new MT2EstimateQCD( aname, *iR );
    thisEstimateQCD->projectFromTree( thisEstimate, selectionTree, selectionDphi );
    data.insert( thisEstimateQCD );

  } // for regions


  MT2Analysis<MT2EstimateQCD>* analysis = new MT2Analysis<MT2EstimateQCD>( aname, data );

  return analysis;

}




MT2Analysis<MT2EstimateQCD>* MT2EstimateQCD::makeAnalysisFromInclusiveTree( const std::string& aname, const std::string& regionsSet, MT2Analysis<MT2EstimateTree>* estimate, const std::string& selectionTree, const std::string& selectionDphi ) {

  std::set<MT2Region> regions = estimate->getRegions();

  if( regions.size()!=1 ) {
  //if( treeInclusive==0 ) {
    std::cout << "[MT2EstimateQCD::makeAnalysisFromEstimateTreeInclusive] ERROR!! You need to pass an inclusive MT2EstimateTree Analysis to use this function!" << std::endl;
    exit(19191);
  }

  //MT2EstimateTree* treeInclusive = estimate->get( MT2Region("HT450toInf_j2toInf_b0toInf") );
  MT2EstimateTree* treeInclusive = estimate->get( *(regions.begin()) );

  // will create a new analysis with custom regions from inclusive tree:
  MT2Analysis<MT2EstimateQCD>* analysis = new MT2Analysis<MT2EstimateQCD>( aname, regionsSet );
  std::set<MT2Region> newRegions = analysis->getRegions();


  for( std::set<MT2Region>::iterator iR=newRegions.begin(); iR!=newRegions.end(); ++iR ) {

    MT2EstimateQCD* thisEstimateQCD = analysis->get( *iR );
    thisEstimateQCD->projectFromTree( treeInclusive, selectionTree, selectionDphi );

  } // for newRegions


  return analysis;

}




MT2EstimateQCD::MT2EstimateQCD( const MT2EstimateQCD& rhs ) : MT2EstimateTree(rhs) {

  this->lDphi = new TH1D(*(rhs.lDphi));
  this->hDphi = new TH1D(*(rhs.hDphi));

  this->dphi_low = rhs.dphi_low;

}

// destructor
MT2EstimateQCD::~MT2EstimateQCD() {

  delete lDphi;
  delete hDphi;

}




void MT2EstimateQCD::setName( const std::string& newName ) {

  MT2EstimateTree::setName(newName);

  lDphi->SetName( this->getHistoName("lDphi").c_str() );
  hDphi->SetName( this->getHistoName("hDphi").c_str() );

}




void MT2EstimateQCD::fillDphi(float dphi, float weight, float mt2) {

  if (dphi > 0.3)
    this->hDphi->Fill( mt2, weight );
  else if (dphi < dphi_low)
    this->lDphi->Fill( mt2, weight );
      
}


TH1D* MT2EstimateQCD::getRatio() const {

  TH1D* ratio = new TH1D( *hDphi );
  ratio->SetName( this->getHistoName("ratio").c_str() );
  ratio->Divide(lDphi);

  return ratio;

}


TF1* MT2EstimateQCD::getFit( const std::string& functionName, float xMin_fit, float xMax_fit, float par0, float par1 ) {

  TH1D* ratio = getRatio();

  float xMin = hDphi->GetXaxis()->GetXmin();
  float xMax = hDphi->GetXaxis()->GetXmax();

  TF1* f1;
  if( functionName=="pow" ) {
    f1 = new TF1(this->getHistoName("power").c_str(), "[0]*TMath::Power(x,[1])", xMin, xMax);
  } else if( functionName=="exp" ) {      
    f1 = new TF1(this->getHistoName("exp").c_str(), "expo(0)", xMin, xMax);
  } else {
    std::cout << "[MT2EstimateQCD::getFit] ERROR!! Unkown function name: '" << functionName << "'" << std::endl;
    std::cout << "Exiting." << std::endl;
    exit(191111);
  }

  if ( par0!=0 && par1!=0 ) {
    f1->SetParameter(0, par0);
    f1->SetParameter(1, par1);
  }

  f1->SetRange( xMin_fit, xMax_fit );
  ratio->Fit( f1, "QR0" ); 
  f1->SetRange( xMin, xMax);

  return f1;

}



void MT2EstimateQCD::finalize() {

  MT2Estimate::finalize();

}




void MT2EstimateQCD::getShit( TFile* file, const std::string& path ) {

  MT2EstimateTree::getShit(file, path);

  lDphi = (TH1D*)file->Get(Form("%s/%s", path.c_str(), lDphi->GetName()));
  hDphi = (TH1D*)file->Get(Form("%s/%s", path.c_str(), hDphi->GetName()));

}



void MT2EstimateQCD::print(const std::string& ofs){

  MT2Estimate::print( ofs );

}

void MT2EstimateQCD::randomizePoisson( float scale, int seed ){

  MT2Estimate::randomizePoisson( scale, seed );

  TRandom3 rand(seed);
  
  for( int ibin=1; ibin<lDphi->GetXaxis()->GetNbins()+1; ++ibin ) {
    
    int poisson_data = rand.Poisson(scale * lDphi->GetBinContent(ibin));
    lDphi->SetBinContent(ibin, poisson_data);
    //lDphi->SetBinError( ibin, 0. );
    lDphi->SetBinError( ibin, TMath::Sqrt(poisson_data) ); // here i want an approximation of the Poisson error
    
  } 

  for( int ibin=1; ibin<hDphi->GetXaxis()->GetNbins()+1; ++ibin ) {
    
    int poisson_data = rand.Poisson(scale * hDphi->GetBinContent(ibin));
    hDphi->SetBinContent(ibin, poisson_data);
    //hDphi->SetBinError( ibin, 0. );
    hDphi->SetBinError( ibin, TMath::Sqrt(poisson_data) ); // here i want an approximation of the Poisson error
    
  } 
  
}

void MT2EstimateQCD::sqrtErrors( float scale ){

  for( int ibin=1; ibin<lDphi->GetXaxis()->GetNbins()+1; ++ibin ) {
    
    float data = scale * lDphi->GetBinContent(ibin);
    lDphi->SetBinContent(ibin, data);
    lDphi->SetBinError( ibin, TMath::Sqrt(data) );
    
  } 

  for( int ibin=1; ibin<hDphi->GetXaxis()->GetNbins()+1; ++ibin ) {
    
    float data = scale * hDphi->GetBinContent(ibin);
    hDphi->SetBinContent(ibin, data);
    hDphi->SetBinError( ibin, TMath::Sqrt(data) );
    
  } 
  
}


void MT2EstimateQCD::write() const {

  MT2EstimateTree::write();

  lDphi->Write();
  hDphi->Write();

}

const MT2EstimateQCD& MT2EstimateQCD::operator=( const MT2EstimateQCD& rhs ) {

  this->region = new MT2Region(*(rhs.region));

  this->yield = new TH1D(*(rhs.yield));

  this->lDphi = new TH1D(*(rhs.lDphi));
  this->hDphi = new TH1D(*(rhs.hDphi));

  this->dphi_low = rhs.dphi_low;

  this->setName(this->getName());

  return *this;

}

MT2EstimateQCD MT2EstimateQCD::operator+( const MT2EstimateQCD& rhs ) const{

  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateQCD::operator+] ERROR! Can't add MT2EstimateQCD with different MT2Regions!" << std::endl;
    exit(113);
  }

  MT2EstimateQCD result(*this);
  result.yield->Add(rhs.yield);

  result.lDphi->Add(rhs.lDphi);
  result.hDphi->Add(rhs.hDphi);
  
  return result;

}

MT2EstimateQCD MT2EstimateQCD::operator-( const MT2EstimateQCD& rhs ) const{

  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2EstimateQCD::operator-] ERROR! Can't add MT2EstimateQCD with different MT2Regions!" << std::endl;
    exit(113);
  }

  MT2EstimateQCD result(*this);

  result.yield->Add(rhs.yield, -1.);
  result.lDphi->Add(rhs.lDphi, -1.);
  result.hDphi->Add(rhs.hDphi, -1.);
 
  return result;

}

const MT2EstimateQCD& MT2EstimateQCD::operator+=( const MT2EstimateQCD& rhs ) {

  this->yield->Add(rhs.yield);

  this->lDphi->Add(rhs.lDphi);
  this->hDphi->Add(rhs.hDphi);

  return (*this);

}

const MT2EstimateQCD& MT2EstimateQCD::operator-=( const MT2EstimateQCD& rhs ) {

  this->yield->Add(rhs.yield, -1.);

  this->lDphi->Add(rhs.lDphi, -1.);
  this->hDphi->Add(rhs.hDphi, -1.);

  return (*this);

}

MT2EstimateQCD MT2EstimateQCD::operator*( float k ) const{

  MT2EstimateQCD result(*this);
  result.yield->Scale(k);
  result.lDphi->Scale(k);
  result.hDphi->Scale(k);

  return result;

}

const MT2EstimateQCD& MT2EstimateQCD::operator*=( float k ) {

  this->yield->Scale(k);
  this->lDphi->Scale(k);
  this->hDphi->Scale(k);

  return (*this);

}

MT2EstimateQCD MT2EstimateQCD::operator/( float k ) const{


  MT2EstimateQCD result(*this);
  result.yield->Scale(1./k);
  result.lDphi->Scale(1./k);
  result.hDphi->Scale(1./k);

  return result;

}

const MT2EstimateQCD& MT2EstimateQCD::operator/=( float k ) {

  this->yield->Scale(1./k);
  this->lDphi->Scale(1./k);
  this->hDphi->Scale(1./k);

  return (*this);

}

  
// friend functions:

MT2EstimateQCD operator*( float k, const MT2EstimateQCD& rhs ) {

  return rhs*k;

}

MT2EstimateQCD operator/( float k, const MT2EstimateQCD& rhs ) {

  return rhs/k;

}


