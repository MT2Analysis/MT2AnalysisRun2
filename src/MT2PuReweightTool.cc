#include "../interface/MT2PuReweightTool.h"
using namespace std;

// Tool to apply pu reweighting based on number of true interactions


MT2PuReweightTool::MT2PuReweightTool(){}


bool MT2PuReweightTool::setPuWeightHist(int year){

  // get the files
  TString fname = TString::Format("../data/pileUp/puWeight%d.root", year);
//cout<<fname<<"?"<<endl;
	TFile *f = new TFile(fname);
  if (!f->IsOpen()) std::cout << " ERROR: Could not file with PU distribution " << std::endl;
//cout<<"file opened"<<endl;
  // get the histograms from the file
  TH1D* h_nTrueInt_weights_temp = 0;
	h_nTrueInt_weights_temp = (TH1D*) f->Get("pileupWeight");
//cout<<"pileupWeight got"<<endl;
  if (!h_nTrueInt_weights_temp){
    std::cout << std::endl << std::endl << "ERROR: Could not find histogram with PU distribution "<< std::endl << std::endl;
  }

  h_nTrueInt_weights = (TH1D*) h_nTrueInt_weights_temp->Clone("h_nTrueInt_weights");
//cout<<"h_nTrueInt_weights got"<<endl;
  h_nTrueInt_weights->SetDirectory(0); // the histogram doesn't belong to any directory anymore
  f->Close();
//cout<<"file closed"<<endl;
//  delete h_nTrueInt_weights_temp;
  delete f;

  return true;
}


double MT2PuReweightTool::getPuWeight(int evt_nTrueInt){

	double puWeight = h_nTrueInt_weights->GetBinContent(h_nTrueInt_weights->FindBin(evt_nTrueInt));
  return puWeight;

}
