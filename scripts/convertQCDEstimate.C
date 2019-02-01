
// run with root -l -b convertQCDEstimate.C

void convertQCDEstimate(){

  // dictionary to convert name of topo region
  std::map<TString, TString> regionsMap;
  // monojet to be checked
  regionsMap["HT250to350_j1_b0"] = "sr1J";
  regionsMap["HT350to450_j1_b0"] = "sr2J";
  regionsMap["HT450to575_j1_b0"] = "sr3J";
  regionsMap["HT575to700_j1_b0"] = "sr4J";
  regionsMap["HT700to1000_j1_b0"] = "sr5J";
  regionsMap["HT1000to1200_j1_b0"] = "sr6J";
  regionsMap["HT1200toInf_j1_b0"] = "sr7J";
  regionsMap["HT250to350_j1_b1toInf"] = "sr11J";
  regionsMap["HT350to450_j1_b1toInf"] = "sr12J";
  regionsMap["HT450to575_j1_b1toInf"] = "sr13J";
  regionsMap["HT575to700_j1_b1toInf"] = "sr14J";
  regionsMap["HT700toInf_j1_b1toInf"] = "sr15J";

  // VL
  regionsMap["HT250to450_j2to3_b0"] = "sr1VL";
  regionsMap["HT250to450_j2to3_b1"] = "sr2VL";
  regionsMap["HT250to450_j2to3_b2"] = "sr3VL";
  regionsMap["HT250to450_j4to6_b0"] = "sr4VL";
  regionsMap["HT250to450_j4to6_b1"] = "sr5VL";
  regionsMap["HT250to450_j4to6_b2"] = "sr6VL";
  regionsMap["HT250to450_j7toInf_b0"] = "sr7VL";
  regionsMap["HT250to450_j7toInf_b1"] = "sr8VL";
  regionsMap["HT250to450_j7toInf_b2"] = "sr9VL";
  regionsMap["HT250to450_j2to6_b3toInf"] = "sr10VL";
  regionsMap["HT250to450_j7toInf_b3toInf"] = "sr11VL";

  // L
  regionsMap["HT450to575_j2to3_b0"] = "sr1L";
  regionsMap["HT450to575_j2to3_b1"] = "sr2L";
  regionsMap["HT450to575_j2to3_b2"] = "sr3L";
  regionsMap["HT450to575_j4to6_b0"] = "sr4L";
  regionsMap["HT450to575_j4to6_b1"] = "sr5L";
  regionsMap["HT450to575_j4to6_b2"] = "sr6L";
  regionsMap["HT450to575_j7toInf_b0"] = "sr7L";
  regionsMap["HT450to575_j7toInf_b1"] = "sr8L";
  regionsMap["HT450to575_j7toInf_b2"] = "sr9L";
  regionsMap["HT450to575_j2to6_b3toInf"] = "sr10L";
  regionsMap["HT450to575_j7toInf_b3toInf"] = "sr11L";

  // M
  regionsMap["HT575to1200_j2to3_b0"] = "sr1M";
  regionsMap["HT575to1200_j2to3_b1"] = "sr2M";
  regionsMap["HT575to1200_j2to3_b2"] = "sr3M";
  regionsMap["HT575to1200_j4to6_b0"] = "sr4M";
  regionsMap["HT575to1200_j4to6_b1"] = "sr5M";
  regionsMap["HT575to1200_j4to6_b2"] = "sr6M";
  regionsMap["HT575to1200_j2to6_b3toInf"] = "sr10M";
  regionsMap["HT575to1200_j7to9_b0"] = "sr20M";
  regionsMap["HT575to1200_j7to9_b1"] = "sr21M";
  regionsMap["HT575to1200_j7to9_b2"] = "sr22M";
  regionsMap["HT575to1200_j7to9_b3"] = "sr23M";
  regionsMap["HT575to1200_j7to9_b4toInf"] = "sr24M";
  regionsMap["HT575to1200_j10toInf_b0"] = "sr25M";
  regionsMap["HT575to1200_j10toInf_b1"] = "sr26M";
  regionsMap["HT575to1200_j10toInf_b2"] = "sr27M";
  regionsMap["HT575to1200_j10toInf_b3"] = "sr28M";
  regionsMap["HT575to1200_j10toInf_b4toInf"] = "sr29M";

  // H
  regionsMap["HT1200to1500_j2to3_b0"] = "sr1H";
  regionsMap["HT1200to1500_j2to3_b1"] = "sr2H";
  regionsMap["HT1200to1500_j2to3_b2"] = "sr3H";
  regionsMap["HT1200to1500_j4to6_b0"] = "sr4H";
  regionsMap["HT1200to1500_j4to6_b1"] = "sr5H";
  regionsMap["HT1200to1500_j4to6_b2"] = "sr6H";
  regionsMap["HT1200to1500_j2to6_b3toInf"] = "sr10H";
  regionsMap["HT1200to1500_j7to9_b0"] = "sr20H";
  regionsMap["HT1200to1500_j7to9_b1"] = "sr21H";
  regionsMap["HT1200to1500_j7to9_b2"] = "sr22H";
  regionsMap["HT1200to1500_j7to9_b3"] = "sr23H";
  regionsMap["HT1200to1500_j7to9_b4toInf"] = "sr24H";
  regionsMap["HT1200to1500_j10toInf_b0"] = "sr25H";
  regionsMap["HT1200to1500_j10toInf_b1"] = "sr26H";
  regionsMap["HT1200to1500_j10toInf_b2"] = "sr27H";
  regionsMap["HT1200to1500_j10toInf_b3"] = "sr28H";
  regionsMap["HT1200to1500_j10toInf_b4toInf"] = "sr29H";

  // UH
  regionsMap["HT1500toInf_j2to3_b0"] = "sr1UH";
  regionsMap["HT1500toInf_j2to3_b1"] = "sr2UH";
  regionsMap["HT1500toInf_j2to3_b2"] = "sr3UH";
  regionsMap["HT1500toInf_j4to6_b0"] = "sr4UH";
  regionsMap["HT1500toInf_j4to6_b1"] = "sr5UH";
  regionsMap["HT1500toInf_j4to6_b2"] = "sr6UH";
  regionsMap["HT1500toInf_j2to6_b3toInf"] = "sr10UH";
  regionsMap["HT1500toInf_j7to9_b0"] = "sr20UH";
  regionsMap["HT1500toInf_j7to9_b1"] = "sr21UH";
  regionsMap["HT1500toInf_j7to9_b2"] = "sr22UH";
  regionsMap["HT1500toInf_j7to9_b3"] = "sr23UH";
  regionsMap["HT1500toInf_j7to9_b4toInf"] = "sr24UH";
  regionsMap["HT1500toInf_j10toInf_b0"] = "sr25UH";
  regionsMap["HT1500toInf_j10toInf_b1"] = "sr26UH";
  regionsMap["HT1500toInf_j10toInf_b2"] = "sr27UH";
  regionsMap["HT1500toInf_j10toInf_b3"] = "sr28UH";
  regionsMap["HT1500toInf_j10toInf_b4toInf"] = "sr29UH";

  int k=0;
  for ( auto imap: regionsMap){
    k++;
     std::cout  << k << " "<<  imap.first << " " << imap.second << std::endl;
  }

  std::vector<TString> regionsETH;
  std::vector<TString> regionsSnT;
  regionsETH.reserve(regionsMap.size());
  regionsSnT.reserve(regionsMap.size());
  for(auto imap: regionsMap) {
    regionsETH.push_back(imap.first);
    regionsSnT.push_back(imap.second);
  }
  std::cout << regionsSnT.size() << " "  << regionsETH.size() << std::endl;

  // dictionary to convert name of histo
  std::map<TString, TString> varNamesMap;
  varNamesMap["nominal"] = "h_mt2bins";
  varNamesMap["syst_jer"] = "h_mt2bins_JERvar";
  varNamesMap["syst_tail"] = "h_mt2bins_TailVar";
  varNamesMap["syst_sigmasoft"] = "h_mt2bins_SigmaSoftVar";
  varNamesMap["syst_njetshape"] = "h_mt2bins_NJetShape";
  varNamesMap["syst_nbjetshape"] = "h_mt2bins_NBJetShape";

  std::vector<TString> varNamesETH; // = {"nom", "jer"};
  std::vector<TString> varNamesSnT;
  varNamesETH.reserve(varNamesMap.size());
  varNamesSnT.reserve(varNamesMap.size());
  for(auto imap: varNamesMap) {
    varNamesETH.push_back(imap.first);
    varNamesSnT.push_back(imap.second);
  }


  // Inputs
  TFile *fileSnT = new TFile("~/QCD/qcdFromRS_FullRunIICombined_V00-10-07.root", "read");

  // create the directory structure of the output
  TFile *fileQCD = new TFile("qcdEstimate.root", "recreate");

  TDirectory *dirs[varNamesETH.size()];
  TDirectory* regDirs[varNamesETH.size()][regionsETH.size()];

  TH1D* histos[varNamesETH.size()][regionsETH.size()];
  TH3D* histos3D[varNamesETH.size()][regionsETH.size()];

  // create as many dirs as variations
  for (int j=0; j<varNamesETH.size(); j++){

    dirs[j] = fileQCD->mkdir(varNamesETH[j]);
    dirs[j]->cd();

    // create a sub-dir for each region
    for (int i=0; i<regionsETH.size(); i++){ // must define regions properly ireg should be a string

      regDirs[j][i]=dirs[j]->mkdir(regionsETH[i]);
      regDirs[j][i]->cd();
      //std::cout << "created dir" << std::endl;


      // name of the output histogram
      TString hname = "yield_" + varNamesETH[j] + "_" + regionsETH[i];
      //if(varNamesETH.at(j)=="nom") hname = "yield_qcdEstimate_" + regionsETH[i];
      //else                         hname = "yield_" + varNamesETH.at(j) + "_UP" + "_qcdEstimate_" + regionsETH[i];

      TH1D *hSnT = (TH1D*) fileSnT->Get( regionsSnT[i] + "/" + varNamesSnT[j]);
      if (hSnT){
        //std::cout << "regionsSnT[i]" << regionsSnT[i] << " varNamesSnT[j]" << varNamesSnT[j] << std::endl;
        histos[j][i] = (TH1D*) hSnT->Clone();  // want to clone it from  //new TH1F(hname,htitle,100,0,100);
        histos[j][i]->SetName(hname);
      }else{
        histos[j][i] = new TH1D(hname, hname, 1., 0., 1.);
        //histos[i][j]->SetName(hname);
      }

      TString h3name = "yield3d_" + varNamesETH[j] + "_" + regionsETH[i];
      histos3D[j][i] = new TH3D(h3name, h3name, 1, 0., 1., 1, 0., 1., 1, 0., 1.);

      dirs[j]->cd();
    }  // end loop over top regions
  } // end loop over systematics

  fileQCD->Write();
  delete fileQCD;





}
