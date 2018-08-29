#include "../interface/MT2Estimate.h"
#include "../interface/MT2Region.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include "TRandom3.h"



MT2Estimate::MT2Estimate( const std::string& aname, const MT2Region& aregion ) {

  name = aname;

  region = new MT2Region(aregion);

  int nBins;
  double* bins;
  region->getBins(nBins, bins);
  
  int nBinsM=93;
  double binWidthM=25.;
  double binsM[nBinsM+1];
  for (int b=0; b<=nBinsM; ++b)
    binsM[b]=b*binWidthM;
 
  //int nBinsMY=161;
  //double binWidthMY=5.;
  // double binsMY[nBinsMY+1];
  // for (int b=0; b<=nBinsMY; ++b)
  //   binsMY[b]=b*binWidthMY;
 
  yield3d = new TH3D(this->getHistoName("yield3d").c_str(), "", nBins, bins, nBinsM, binsM, nBinsM, binsM);
  //yield3d = new TH3D(this->getHistoName("yield3d").c_str(), "", nBins, bins, nBinsM, binsM, nBinsMY, binsMY);
  yield3d->Sumw2();

  yield = new TH1D(this->getHistoName("yield").c_str(), "", nBins, bins);
  yield->Sumw2();

}


MT2Estimate::MT2Estimate( const MT2Estimate& rhs ) {

  name = rhs.getName();

  region = new MT2Region(*(rhs.region));

  yield3d = new TH3D(*(rhs.yield3d));
  yield = new TH1D(*(rhs.yield));

}


MT2Estimate::~MT2Estimate() {

  delete region;
  delete yield;
  delete yield3d;

  
}



std::string MT2Estimate::getHistoName( const std::string& prefix ) const {

  std::string returnName = prefix + "_" + name + "_" + region->getName();
  
  return returnName;

}



void MT2Estimate::setName( const std::string& newName ) {

  name = newName;
  yield3d->SetName( this->getHistoName("yield3d").c_str() );
  yield->SetName( this->getHistoName("yield").c_str() );

}



void MT2Estimate::getYieldBins( int& nBins, double*& bins ) const {
  nBins = yield->GetNbinsX();
  bins = new double[nBins+1];
  for (int iBin = 0; iBin <= nBins; iBin++)
    bins[iBin] = yield->GetXaxis()->GetBinLowEdge(iBin+1);
}



void MT2Estimate::rebinYields( MT2Analysis<MT2Estimate>* analysis, int nBins, float xMin, float xMax ) {

  double bins[nBins+1];
  double step = (xMax-xMin)/(double)nBins;

  for(int i=0; i < nBins+1; i++)
    bins[i] = xMin + ((double)i )* step;
  

  MT2Estimate::rebinYields( analysis, nBins, bins);

}


void MT2Estimate::rebinYields( MT2Analysis<MT2Estimate>* analysis, int nBins, double* bins) {

  std::set<MT2Region> regions = analysis->getRegions();

  for( std::set<MT2Region>::iterator iR = regions.begin(); iR!=regions.end(); ++iR ) {

    MT2Estimate* estimate = analysis->get(*iR);
    TH1D* thisYield = estimate->yield;

    std::string oldName(thisYield->GetName());

    if( thisYield!=0 )
      delete thisYield; 

    estimate->yield = new TH1D( oldName.c_str(), "", nBins, bins );

  }

}



const MT2Estimate& MT2Estimate::operator=( const MT2Estimate& rhs ) {


  this->region = new MT2Region(*(rhs.region));

  this->yield3d = new TH3D(*(rhs.yield3d));
  this->yield = new TH1D(*(rhs.yield));
  
  this->setName(this->getName());

  return *this;

}



MT2Estimate MT2Estimate::operator+( const MT2Estimate& rhs ) const {


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2Estimate::operator+] ERROR! Can't add MT2Estimate with different MT2Regions!" << std::endl;
    exit(113);
  }

  MT2Estimate result(*this);
  result.yield3d->Add(rhs.yield3d);
  result.yield->Add(rhs.yield);

  return result;

}



MT2Estimate MT2Estimate::operator-( const MT2Estimate& rhs ) const {


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2Estimate::operator-] ERROR! Can't add MT2Estimate with different MT2Regions!" << std::endl;
    exit(113);
  }

  MT2Estimate result(*this);
  result.yield3d->Add(rhs.yield3d, -1.);
  result.yield->Add(rhs.yield, -1.);

  return result;

}




MT2Estimate MT2Estimate::operator/( const MT2Estimate& rhs ) const {


  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2Estimate::operator/] ERROR! Can't divide MT2Estimate with different MT2Regions!" << std::endl;
    exit(113);
  }
//  //////
//  MT2Estimate result(*this);
//  
//  for( int iBin=1; iBin<result.yield->GetNbinsX()+1; ++iBin ) {
//  
//    float thisBin  = result.yield->GetBinContent(iBin);
//    float otherBin = rhs.yield->Integral();
//  
//    float newBin = thisBin/otherBin;
//  
//    result.yield         ->SetBinContent( iBin, newBin );
// 
//    for( int iBinY=1; iBinY<result.yield3d->GetNbinsY()+1; ++iBinY)
//      for( int iBinZ=1; iBinZ<result.yield3d->GetNbinsZ()+1; ++iBinZ)
//        result.yield3d   ->SetBinContent( iBin, iBinY, iBinZ, newBin );
//
//  }
//
//  return result;
//  //////

  MT2Estimate result(*this);
  result.yield3d->Divide(rhs.yield3d);
  result.yield->Divide(rhs.yield);
  //MT2Estimate result(name, *(this->region) );
  //result.yield = new TH1D(*(this->yield));
  //result.yield->Divide(rhs.yield);
  
  return result;

}


MT2Estimate MT2Estimate::operator*( const MT2Estimate& rhs ) const {

  if( *(this->region) != *(rhs.region) ) {
    std::cout << "[MT2Estimate::operator*] ERROR! Can't multiply MT2Estimate with different MT2Regions!" << std::endl;
    exit(113);
  }

  MT2Estimate result(*this);
  result.yield3d->Multiply(rhs.yield3d);
  result.yield->Multiply(rhs.yield);
  //MT2Estimate result(name, *(this->region) );
  //result.yield = new TH1D(*(this->yield));
  //result.yield->Multiply(rhs.yield);

  return result;

}



MT2Estimate MT2Estimate::operator/( float k ) const {

  MT2Estimate result(*this);
  result.yield3d->Scale(1./k);
  result.yield->Scale(1./k);
  //MT2Estimate result(name, *(this->region) );
  //result.yield = new TH1D(*(this->yield));
  //result.yield->Scale(1./k);

  return result;

}


MT2Estimate MT2Estimate::operator*( float k ) const {

  MT2Estimate result(*this);
  result.yield3d->Scale(k);
  result.yield->Scale(k);
  //MT2Estimate result(name, *(this->region) );
  //result.yield = new TH1D(*(this->yield));
  //result.yield->Scale(k);

  return result;

}




const MT2Estimate& MT2Estimate::operator/=( const MT2Estimate& rhs ) {

  this->yield3d->Divide(rhs.yield3d);
  this->yield->Divide(rhs.yield);
  return (*this);

}



const MT2Estimate& MT2Estimate::operator+=( const MT2Estimate& rhs ) {

  this->yield3d->Add(rhs.yield3d);
  this->yield->Add(rhs.yield);
  return (*this);

}


const MT2Estimate& MT2Estimate::operator-=( const MT2Estimate& rhs ) {

  this->yield3d->Add(rhs.yield3d, -1.);
  this->yield->Add(rhs.yield, -1.);
  return (*this);

}


const MT2Estimate& MT2Estimate::operator*=( const MT2Estimate& rhs ) {

  this->yield3d->Multiply(rhs.yield3d);
  this->yield->Multiply(rhs.yield);
  return (*this);

}


const MT2Estimate& MT2Estimate::operator*=( float k ) {

  this->yield3d->Scale(k);
  this->yield->Scale(k);
  return (*this);

}


const MT2Estimate& MT2Estimate::operator/=( float k ) {

  this->yield3d->Scale(1./k);
  this->yield->Scale(1./k);
  return (*this);

}


const MT2Estimate& MT2Estimate::getMassPoint( const MT2Estimate& rhs, int mParent, int mLSP ){

  this->region = new MT2Region(*(rhs.region));

  this->yield3d = new TH3D(*(rhs.yield3d));

  TH1D* this_mParent = this->yield3d->ProjectionY("mParent");
  int iBinY = this_mParent->FindBin(mParent);

  TH1D* this_LSP = this->yield3d->ProjectionZ("mLSP", 0, -1, iBinY, iBinY);
  int iBinZ = this_LSP->FindBin(mLSP);

  TH1D* this_mt2 = this->yield3d->ProjectionX("mt2", iBinY, iBinY, iBinZ, iBinZ);

  this->yield = (TH1D*) this_mt2->Clone();
  this->yield ->Sumw2();
  
  this->yield3d ->Reset();
  for( int iBin = 1; iBin < this->yield->GetNbinsX()+1; ++iBin )
    this->yield3d ->Fill( this->yield->GetBinContent( iBin ), mParent, mLSP );
  this->yield3d->Sumw2();

  std::string massPointName( Form("%s_mParent%d_mLSP%d", this->getName().c_str(), mParent, mLSP ) );
  this->setName(massPointName);

  return *this;

}


MT2Analysis<MT2Estimate>* MT2Estimate::makeIntegralAnalysisFromEstimate( const std::string& aname, const std::string& regionsSet, MT2Analysis<MT2Estimate>* estimate ) {

  std::set<MT2Region> regions = estimate->getRegions();

  std::set<MT2Estimate*> data;

  for( std::set<MT2Region>::iterator iR=regions.begin(); iR!=regions.end(); ++iR ) {

    MT2Estimate*  thisEstimate = estimate->get( *iR );

    double error;
    double integral = thisEstimate->yield->IntegralAndError(1, -1, error);

    int nBins;
    double* bins;
    iR->getBins(nBins,bins);

    TH1D* thisYield = new TH1D("yield", "", nBins, bins);
    thisYield->Sumw2();

    //    for( int iBin = 1; iBin < thisEstimate->yield->GetNbinsX()+1; ++iBin ){
    for( int iBin = 1; iBin <= nBins+1; ++iBin ){

      thisYield->SetBinContent(iBin, integral);
      thisYield->SetBinError(iBin, error);
      thisEstimate->yield = (TH1D*) thisYield->Clone((thisEstimate->yield->GetName()));
      
      //      thisEstimate->yield->SetBinContent(iBin, integral);
      //      thisEstimate->yield->SetBinError(iBin, error);

      for( int jBin = 1; jBin < thisEstimate->yield3d->GetNbinsY()+1; ++jBin )
	for( int kBin = 1; kBin < thisEstimate->yield3d->GetNbinsZ()+1; ++kBin ){
	  thisEstimate->yield3d->SetBinContent(iBin, jBin, kBin, integral);
	  thisEstimate->yield3d->SetBinError(iBin, jBin, kBin, error);
	}
    }

    data.insert( thisEstimate );

  } // for regions                                                                                                                                                                                                               

  MT2Analysis<MT2Estimate>* analysis = new MT2Analysis<MT2Estimate>( aname, data );

  return analysis;

}


void MT2Estimate::getShit( TFile* file, const std::string& path ) {

  yield3d = (TH3D*)file->Get(Form("%s/%s", path.c_str(), yield3d->GetName()));
  yield = (TH1D*)file->Get(Form("%s/%s", path.c_str(), yield->GetName()));

}



void MT2Estimate::print(const std::string& ofs){

  Int_t binXmin=1;
  Int_t binXmax=-1;

  Double_t error;
  Double_t integral = yield->IntegralAndError(binXmin, binXmax, error);

  std::ofstream ofs_file;
  ofs_file.open( ofs, std::ofstream::app );
  if(integral >= 10)
    ofs_file << std::fixed << std::setprecision(1) << " & " << integral << " $\\pm$ " << error;
  else if(integral < 10)
    ofs_file << std::fixed << std::setprecision(2) << " & " << integral << " $\\pm$ " << error;

}

void MT2Estimate::print(std::ofstream& ofs_file){

  Int_t binXmin=1;
  Int_t binXmax=-1;

  Double_t error;
  Double_t integral = yield->IntegralAndError(binXmin, binXmax, error);

  if(integral >= 10)
    ofs_file << std::fixed << std::setprecision(1) << " & " << integral << " $\\pm$ " << error;
  else if(integral < 10)
    ofs_file << std::fixed << std::setprecision(2) << " & " << integral << " $\\pm$ " << error;

}

void MT2Estimate::print( std::ofstream& ofs_file, Int_t mt2_bin ){

  Double_t error;
  Double_t integral = yield->IntegralAndError(mt2_bin, mt2_bin, error);
  
  if(integral >= 10)
    ofs_file << std::fixed << std::setprecision(1) << " & " << integral << " $\\pm$ " << error;
  else if(integral < 10)
    ofs_file << std::fixed << std::setprecision(2) << " & " << integral << " $\\pm$ " << error;
  
}

void MT2Estimate::printData( std::ofstream& ofs_file, Int_t mt2_bin, Bool_t lastBin ){

  Double_t error;
  Double_t integral;
  if(!lastBin)
    integral = yield->IntegralAndError(mt2_bin, mt2_bin, error);
  else
    integral = yield->IntegralAndError(mt2_bin, -1, error);

  ofs_file << std::fixed << std::setprecision(0) << " & " << integral;
  
}

void MT2Estimate::print( std::ofstream& ofs_file, Float_t m1, Float_t m2, Int_t mt2_bin, float k ){

  TH1D* h_sig;
  TH3D* h_sig3d;
  h_sig3d = yield3d;

  //  int nBinsM=81;
  int nBinsM=93;
  double binWidthM=25.;
  double binsM[nBinsM+1];
  for (int b=0; b<=nBinsM; ++b)
    binsM[b]=b*binWidthM;

  std::cout << "Printing for m1 = " << m1 << ", m2 = " << m2 << std::endl;

  if( h_sig3d == 0 ){
    
    std::cout << "3d histogram does not exist, initializing empty histogram..." << std::endl;
    h_sig3d = new TH3D("emptyHisto", "", nBinsM, binsM, nBinsM, binsM, nBinsM, binsM);
  
  }

  int binY, binZ;
  binY = h_sig3d->GetYaxis()->FindBin(m1);
  binZ = h_sig3d->GetYaxis()->FindBin(m2);
  h_sig = h_sig3d->ProjectionX("mt2_0", binY, binY, binZ, binZ);

  Double_t error;
  Double_t integral = h_sig->IntegralAndError(mt2_bin, mt2_bin, error);
  
  integral*=k;
  error*=k;

  if(integral >= 10)
    ofs_file << std::fixed << std::setprecision(1) << " & " << integral << " $\\pm$ " << error;
  else if(integral < 10)
    ofs_file << std::fixed << std::setprecision(2) << " & " << integral << " $\\pm$ " << error;
  
}

void MT2Estimate::randomizePoisson( float scale, int seed ){

  TRandom3 rand(seed);
  
  for( int ibin=1; ibin<yield->GetXaxis()->GetNbins()+1; ++ibin ) {
    
    int poisson_data = rand.Poisson(scale * yield->GetBinContent(ibin));
    yield->SetBinContent(ibin, poisson_data);
    yield->SetBinError(ibin, 0.); // it's data 
    
  } 
  
}

// friend functions

MT2Estimate operator*( float k, const MT2Estimate& rhs ) {

  return rhs*k;

}


MT2Estimate operator/( float k, const MT2Estimate& rhs ) {

  return rhs/k;

}
