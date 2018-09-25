#ifndef UTILS
#define UTILS

#include <iostream>
#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TString.h"

// Function to calculate the number of generated events
Double_t getNgen(std::string sampleFileName, std::string branch){

  std::cout << "-> Calculating sum of gen weights from branch " << branch.c_str() << std::endl; 

  TFile* file = TFile::Open(sampleFileName.c_str());
  if(!file) std::cout << "ERROR did not find input file " << sampleFileName.c_str() << std::endl;
  
  TTree* runs = (TTree*)file->Get("Runs"); 
  if(!runs) std::cout << "ERROR did not find input tree Runs " << std::endl; 
  
  TString branch_ = branch.c_str();

  TH1::AddDirectory(kTRUE);

  TH1D *histo = new TH1D("histo", "histo", 1, 0., 1000000000); // max 1 G evts per file   

  TString projExpr = branch_ + ">>histo";

  runs->Draw(projExpr, "", "goff");
  
  if(!histo || histo->GetEntries()==0) std::cout << "ERROR projected histogram not available" << std::endl;

  Double_t result = histo->GetEntries()*histo->GetMean();

  delete histo;

  std::cout << "   Result is " << std::setprecision(10) << result << std::endl;

  return result;

}


#endif
