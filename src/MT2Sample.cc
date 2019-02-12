#include "../interface/MT2Sample.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include "TFile.h"
#include "TTree.h"

using namespace std;

MT2Sample::MT2Sample() {};



MT2Sample::~MT2Sample() {};




std::vector<MT2Sample> MT2Sample::loadSamples(const std::string& filename, int idMin, int idMax, int isETH) {

  return loadSamples(filename, "", idMin, idMax, isETH);

}



std::vector<MT2Sample> MT2Sample::loadSamples(const std::string& filename, const std::string& filter, int idMin, int idMax, int isETH) {


  std::vector<MT2Sample> fSamples;

  char buffer[200];
  std::ifstream IN(filename.c_str());


  std::cout << "------------------------------------" << std::endl;
  std::cout << "Sample File  " << filename << std::endl;

  int counter(0);
  TString fPath;


  while( IN.getline(buffer, 500, '\n') ) {
    if (buffer[0] == '#') {
      continue; // Skip lines commented with '#'                                                                                                                                                                                 
    }
    std::string sampleFilePath(buffer);
    TString sampleFilePath_tstr(sampleFilePath);
    if( !(sampleFilePath_tstr.EndsWith(".root")) ) continue;
    MT2Sample s;
    s.file = sampleFilePath;
    sampleFilePath_tstr.ReplaceAll( "/", " " );

    std::istringstream iss(sampleFilePath_tstr.Data());
    std::vector<std::string> pathDirs;
    do {
      std::string sub;
      iss >> sub;
      pathDirs.push_back(sub);
    } while (iss);
    TString rootFileName(pathDirs[pathDirs.size()-2]); // last element of vector always empty: take -2

    if( filter!="" && !(rootFileName.Contains(filter.c_str())) ) continue;

    rootFileName.ReplaceAll( "_", " " );
    rootFileName.ReplaceAll( ".", " " );
 
    std::vector<std::string> fileNameParts;
    std::istringstream iss2(rootFileName.Data());
    do {
      std::string sub;
      iss2 >> sub;
      fileNameParts.push_back(sub);
    } while (iss2);

    if( fileNameParts.size()==0 ) {
      std::cout << "[MT2Sample::loadSamples] ERROR! Illegal file name: " << rootFileName << std::endl;
      exit(1101);
    }
    TFile* file = TFile::Open(s.file.c_str());
    //std::cout << "debug working on file" << s.file.c_str() << std::endl;
    TString treeName = isETH ? "Events" : "mt2";
    TTree* tree = (TTree*)file->Get(treeName);
    
    ULong64_t evt_nEvts;
    int evt_id;
    float evt_filter, evt_kfactor, evt_xsec; // evt_scale1fb;
    tree->SetBranchAddress("evt_id",&evt_id);
//    tree->SetBranchAddress("evt_nEvts",&evt_nEvts);
    tree->SetBranchAddress("evt_filter",&evt_filter);
    tree->SetBranchAddress("evt_kfactor",&evt_kfactor);
    tree->SetBranchAddress("evt_xsec",&evt_xsec);
//    tree->SetBranchAddress("evt_scale1fb",&evt_scale1fb);
    
    tree->GetEntry(0);
    s.id       = evt_id;
//    if(s.id<=0){
      if( rootFileName.Contains("JetHT") )               s.id = 1;
      else if( rootFileName.Contains("HTMHT") )          s.id = 2;
      else if( rootFileName.Contains("MET") )            s.id = 3;
      else if( rootFileName.Contains("DoubleEG") )       s.id = 4;
      else if( rootFileName.Contains("DoubleMuon") )     s.id = 5;
      else if( rootFileName.Contains("MuonEG") )         s.id = 6;
      else if( rootFileName.Contains("SinglePhoton") )   s.id = 7;
      else if( rootFileName.Contains("SingleMuon") )     s.id = 8;
      else if( rootFileName.Contains("SingleElectron") ) s.id = 9;
      // else if( rootFileName.Contains("T1bbbb") )         s.id = 1020;
      //else if( rootFileName.Contains("singletop") )	 s.id = 403;
      //else if( rootFileName.Contains("mg_lo") )	         s.id =330 ;
      //else if( rootFileName.Contains("wjets") )	         s.id = 504;
      //else if( rootFileName.Contains("dyjetsll") )	 s.id = 705;
      //else if( rootFileName.Contains("T2bW") )	         s.id = 1490;
      // declaring numbering convention from SnT ntuples
      if (sampleFilePath_tstr.Contains("wjets_ht100to200"))                s.id=502;
      else if (sampleFilePath_tstr.Contains("wjets_ht200to400"))                s.id=503;
      else if (sampleFilePath_tstr.Contains("wjets_ht400to600"))                s.id=504;
      else if (sampleFilePath_tstr.Contains("wjets_ht600to800"))                s.id=505;
      else if (sampleFilePath_tstr.Contains("wjets_ht800to1200"))                s.id=506;
      else if (sampleFilePath_tstr.Contains("wjets_ht1200to2500"))                s.id=507;
      else if (sampleFilePath_tstr.Contains("wjets_ht2500toInf"))                s.id=508;

      else if (sampleFilePath_tstr.Contains("zinv_ht100to200"))                s.id=602;
      else if (sampleFilePath_tstr.Contains("zinv_ht200to400"))                s.id=603;
      else if (sampleFilePath_tstr.Contains("zinv_ht400to600"))                s.id=604;
      else if (sampleFilePath_tstr.Contains("zinv_ht600to800"))                s.id=605;
      else if (sampleFilePath_tstr.Contains("zinv_ht800to1200"))                s.id=606;
      else if (sampleFilePath_tstr.Contains("zinv_ht1200to2500"))                s.id=607;
      else if (sampleFilePath_tstr.Contains("zinv_ht2500toInf"))                s.id=608;
      
      else if (sampleFilePath_tstr.Contains("dyjetsll_ht100to200"))                s.id=702;
      else if (sampleFilePath_tstr.Contains("dyjetsll_ht200to400"))                s.id=703;
      else if (sampleFilePath_tstr.Contains("dyjetsll_ht400to600"))                s.id=704;
      else if (sampleFilePath_tstr.Contains("dyjetsll_ht600to800"))                s.id=705;
      else if (sampleFilePath_tstr.Contains("dyjetsll_ht800to1200"))                s.id=706;
      else if (sampleFilePath_tstr.Contains("dyjetsll_ht1200to2500"))                s.id=707;
      else if (sampleFilePath_tstr.Contains("dyjetsll_ht2500toInf"))                s.id=708;

      else if (sampleFilePath_tstr.Contains("ttdl_mg"))                s.id=303;
      else if (sampleFilePath_tstr.Contains("ttsl_top_mg"))                s.id=301;
      else if (sampleFilePath_tstr.Contains("ttsl_tbar_mg"))                s.id=302;


      else if (sampleFilePath_tstr.Contains("T1qqqq")) s.id = 1001;
      else if (sampleFilePath_tstr.Contains("T1bbbb")) s.id = 1101;
      else if (sampleFilePath_tstr.Contains("T1tttt")) s.id = 1201;
      else if (sampleFilePath_tstr.Contains("T2qq"))   s.id = 1301;
      else if (sampleFilePath_tstr.Contains("T2bb"))   s.id = 1401;
      else if (sampleFilePath_tstr.Contains("T2tt"))   s.id = 1501;
//    }

    
//    s.nevents  = evt_nEvts;
    s.xsection = evt_xsec;
    s.filter   = evt_filter;
    s.kfact    = evt_kfactor;
//    s.scale1fb = evt_scale1fb;

    if( (idMin>=0 && s.id<idMin) || (idMax>=0 && s.id>idMax) ) {
      file->Close();
      continue;
    }

    bool isData = s.id>0 && s.id<100;

    s.sname = fileNameParts[0];
    s.name  = fileNameParts[0];
    if( !isData ) {

      bool foundPU=false;
      int lastOnes = 3;
      for( unsigned i=1; i<fileNameParts.size() && lastOnes>0; ++i ) {
        if( fileNameParts[i] == "root" ) break;
        if( fileNameParts[i] == "babytree" ) continue;
        if( fileNameParts[i] == "prune" ) continue;
        if( fileNameParts[i] == "skim" ) continue;
	if( fileNameParts[i] == "post" ) continue;
        s.name += "_" + fileNameParts[i];
        if( fileNameParts[i]=="PU" ) foundPU = true;
        if( !foundPU )
          s.sname += "_" + fileNameParts[i];
        else
          lastOnes--;
      }

    } else {
      for( unsigned i=1; i<fileNameParts.size(); ++i ) {
        if( fileNameParts[i] == "root" ) break;
        if( fileNameParts[i] == "babytree" ) continue;
        if( fileNameParts[i] == "prune" ) continue;
        if( fileNameParts[i] == "skim" ) continue;
	if( fileNameParts[i] == "post" ) continue;
        s.name += "_" + fileNameParts[i];
        s.sname += "_" + fileNameParts[i];
      }

    }


    // Manually set variables
    s.PU_avg_weight = 1;
    s.lumi = 1000; // 1/fb
    
    std::cout << " ---- " << std::endl;
    std::cout << "  New sample added: " << s.name << std::endl;
    std::cout << "   Sample no.      " << counter << std::endl;
    std::cout << "   Short name:     " << s.sname << std::endl;
    std::cout << "   File:           " << s.file << std::endl;
    std::cout << "   ID:             " << s.id << std::endl;
    //std::cout << "   Events:         " << s.nevents  << std::endl;
    std::cout << "   Xsection:       " << s.xsection << std::endl;
    //std::cout << "   Lumi:           " << s.lumi << std::endl;
    std::cout << "   Kfactor:        " << s.kfact << std::endl;
    std::cout << "   Filter:         " << s.filter << std::endl;
    //std::cout << "   Scale to 1/fb:  " << s.scale1fb << std::endl;
    //std::cout << "   Avg PU weight:  " << s.PU_avg_weight << std::endl;
    //std::cout << "   type:           " << s.type << std::endl;
    //std::cout << "   Color:          " << s.color << std::endl;
    fSamples.push_back(s);
    file->Close();
    tree = 0;
    counter++;
    
  }

  std::cout << "------------------------------------" << std::endl;

  return fSamples;

}


