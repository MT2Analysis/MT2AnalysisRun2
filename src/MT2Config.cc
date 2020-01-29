#include "../interface/MT2Config.h"

#include <iostream>
#include <fstream>
#include <cmath>

#include "TH1F.h"



MT2Config::MT2Config( const std::string& name ) {

  name_ = name;

  std::string configFileName = "cfgs/" + name + ".txt";

  std::cout << std::endl;
  std::cout << "-> Reading config file: " << configFileName << std::endl;
  std::cout << std::endl;

  lumi_ = 0.;
  lumi_JetHT_ = 0.;
  lumi_HTMHT_ = 0.;
  lumi_SinglePhoton_ = 0.;
  lumi_DoubleEG_ = 0.;
  lumi_DoubleMu_ = 0.;
  year_ = 0;
  useETHdata_ = 1;
  useETHmc_ = 1;
//  applyJSONforSR_ = 0.;

  regionsSet_ = "";
  mcSamples_ = "";
  qcdMCSamples_ = "";
  qcdMonoJetMCSamples_ = "";
  sigSamples_ = "";
  dataSamples_ = "";
  zllmcSamples_="";
  llepmcSamples_="";
  zllsigSamples_="";
  llepsigSamples_="";
  zlldataSamples_="";
  llepdataSamples_="";
  qcdDataSamples_ = "";
  qcdMonoJetDataSamples_ = "";
  additionalStuff_ = "";
  dataMLscore_="";
  mcMLscore_="";
  zllmcMLscore_="";
  llepmcMLscore_="";
  zllsigMLscore_="";
  llepsigMLscore_="";
  zlldataMLscore_="";
  llepdataMLscore_="";
  analysisType_ = "mt2";
  crRegionsSet_ = "13TeV_inclusive";
  qcdRegionsSet_ = "zurich_onlyHT";
  MLcut_=-1;//default no cut on MLscore;
  isMLcut_=0;
  usebinMLcut_=0;
  binMLcutinf_="";
  gammaTemplateRegions_ = "13TeV_inclusive"; // default
  gammaTemplateType_    = "RC"; // default
  gammaIsoCut_    = 2.5; // default
  gamma2bMethod_    = "default"; // default

  zllRegions_ = "13TeV_inclusive"; //default

  std::ifstream IN(configFileName.c_str());
  char buffer[200];
  char StringValue[1000];


  while( IN.getline(buffer, 200, '\n') ) {

    if (buffer[0] == '#') {
      continue; // Skip lines commented with '#'                                                                                                                                                                                 
    }

    std::cout << buffer << std::endl;

    char this_name_c[200];
    sscanf(buffer, "%s %s", this_name_c, StringValue);
    std::string this_name(this_name_c);

    if( this_name=="lumi" )
      lumi_ = atof(StringValue);
    else if( this_name=="lumi_JetHT" )
      lumi_JetHT_ = atof(StringValue);
    else if( this_name=="lumi_HTMHT" )
      lumi_HTMHT_ = atof(StringValue);
    else if( this_name=="lumi_SinglePhoton" )
      lumi_SinglePhoton_ = atof(StringValue);
    else if( this_name=="lumi_DoubleEG" )
      lumi_DoubleEG_ = atof(StringValue);
    else if( this_name=="lumi_DoubleMu" )
      lumi_DoubleMu_ = atof(StringValue);
    else if( this_name=="year")
      year_ = atoi(StringValue);
    else if( this_name=="useETHdata")
      useETHdata_ = atoi(StringValue);
    else if( this_name=="useETHmc")
      useETHmc_ = atoi(StringValue);
//    else if( this_name=="applyJSONforSR")
//      applyJSONforSR_ = atoi(StringValue);
    else if( this_name=="regionsSet" )
      regionsSet_ = std::string(StringValue);
    else if( this_name=="mcSamples" )
      mcSamples_ = std::string(StringValue);
    else if( this_name=="qcdMCSamples" )
      qcdMCSamples_ = std::string(StringValue);
    else if( this_name=="qcdMonoJetMCSamples" )
      qcdMonoJetMCSamples_ = std::string(StringValue);
    else if( this_name=="sigSamples" )
      sigSamples_ = std::string(StringValue);
    else if( this_name=="dataSamples" )
      dataSamples_ = std::string(StringValue);
    else if( this_name=="zllmcSamples" )
      zllmcSamples_ = std::string(StringValue);
    else if( this_name=="llepmcSamples" )
      llepmcSamples_ = std::string(StringValue);
    else if( this_name=="zllsigSamples" )
      zllsigSamples_ = std::string(StringValue);
    else if( this_name=="llepsigSamples" )
      llepsigSamples_ = std::string(StringValue);
    else if( this_name=="zlldataSamples" )
      zlldataSamples_ = std::string(StringValue);
    else if( this_name=="llepdataSamples" )
      llepdataSamples_ = std::string(StringValue);
    else if( this_name=="qcdDataSamples" )
      qcdDataSamples_ = std::string(StringValue);
    else if( this_name=="qcdMonoJetDataSamples" )
      qcdMonoJetDataSamples_ = std::string(StringValue);
    else if( this_name=="additionalStuff" )
      additionalStuff_ = std::string(StringValue);
    else if( this_name=="gammaTemplateRegions" )
      gammaTemplateRegions_ = std::string(StringValue);
    else if( this_name=="gammaTemplateType" )
      gammaTemplateType_ = std::string(StringValue);
    else if( this_name=="gammaIsoCut" )
      gammaIsoCut_ = atof(StringValue);
    else if( this_name=="analysisType" )
      analysisType_ = std::string(StringValue);
    else if( this_name=="crRegionsSet" )
      crRegionsSet_ = std::string(StringValue);
    else if( this_name=="qcdRegionsSet" )
      qcdRegionsSet_ = std::string(StringValue);
    else if( this_name=="gamma2bMethod" )
      gamma2bMethod_ = std::string(StringValue);
    else if( this_name=="zllRegions" )
      zllRegions_ = std::string(StringValue);
    else if( this_name=="mcMLscore" )
      mcMLscore_ = std::string(StringValue);
    else if( this_name=="dataMLscore" )
      dataMLscore_ = std::string(StringValue);
    else if( this_name=="sigMLscore")
      sigMLscore_ = std::string(StringValue);
    else if( this_name=="zllmcMLscore" )
      zllmcMLscore_ = std::string(StringValue);
    else if( this_name=="llepmcMLscore" )
      llepmcMLscore_ = std::string(StringValue);
    else if( this_name=="zllsigMLscore")
      zllsigMLscore_ = std::string(StringValue);
    else if( this_name=="llepsigMLscore")
      llepsigMLscore_ = std::string(StringValue);
    else if( this_name=="zlldataMLscore")
      zlldataMLscore_ = std::string(StringValue);
    else if( this_name=="llepdataMLscore")
      llepdataMLscore_ = std::string(StringValue);
    else if( this_name=="MLcut")
      MLcut_ = atof(StringValue);
    else if( this_name=="isMLcut")
      isMLcut_ = atoi(StringValue);
    else if (this_name=="usebinMLcut")
      usebinMLcut_=atoi(StringValue);
    else if (this_name=="binMLcutinf")
      binMLcutinf_=std::string(StringValue);
  } // while getline

  std::cout << std::endl;


  if( this->useMC() && lumi_ <=0. ) {
    std::cout << "[MT2Config] ERROR!! If you process MC files you need to set a valid lumi value in your cfg!" << std::endl;
    std::cout << "Exiting..." << std::endl;
    exit(761);
  }


  if( gammaTemplateType_!="FR" && gammaTemplateType_!="MC" && gammaTemplateType_!="RC" ) {
    std::cout << "[MT2Config::gammaTemplateType] ERROR! gammaTemplateType may only be 'MC' or 'FR' or 'RC'" << std::endl;
    exit(1111);
  }

     
}


float MT2Config::lumi_JetHT() const { 

  return this->defaultLumi(lumi_JetHT_);

}


float MT2Config::lumi_HTMHT() const { 

  return this->defaultLumi(lumi_HTMHT_);

}


float MT2Config::lumi_SinglePhoton() const { 

  return this->defaultLumi(lumi_SinglePhoton_);

}


float MT2Config::lumi_DoubleEG() const { 

  return this->defaultLumi(lumi_DoubleEG_);

}


float MT2Config::lumi_DoubleMu() const { 

  return this->defaultLumi(lumi_DoubleMu_);

}


float MT2Config::defaultLumi( float lumi ) const {

  float returnLumi = (lumi>0.) ? lumi : lumi_; // if not over-written, return (common) lumi_
  return returnLumi;

}



bool MT2Config::useMC() const {

  return mcSamples_!="";

}



bool MT2Config::dummyAnalysis() const {

  return dataSamples_=="datatest";

}


std::string MT2Config::getEventYieldDir() const {

  std::string outputdir = "EventYields_" + name_;
  if( this->dummyAnalysis() ) outputdir += "_dummy";
  return outputdir;

}


std::string MT2Config::getGammaCRdir() const {

  std::string outputdir = this->getEventYieldDir() + "/gammaControlRegion/";

  return outputdir;

}

std::string MT2Config::getDiPhotonCRdir() const {

  std::string outputdir = this->getEventYieldDir() + "/diPhotonControlRegion/";

  return outputdir;

}



void MT2Config::saveAs( const std::string& filename ) const {


  std::ofstream ofs(filename.c_str());

  ofs << "#name " << name_ << std::endl;

  ofs << "lumi "  << lumi_  << std::endl;
  ofs << "lumi_JetHT "        <<   lumi_JetHT_          << std::endl;
  ofs << "lumi_HTMHT "        <<   lumi_HTMHT_          << std::endl;
  ofs << "lumi_SinglePhoton " <<   lumi_SinglePhoton_   << std::endl;
  ofs << "lumi_DoubleEG "     <<   lumi_DoubleEG_       << std::endl;
  ofs << "lumi_DoubleMu "     <<   lumi_DoubleMu_       << std::endl;
  ofs << "year "     <<   year_       << std::endl;
  ofs << "useETHdata " << useETHdata_ << std::endl;
  ofs << "useETHmc " << useETHmc_ << std::endl;
//  ofs << "applyJSONforSR "     <<   applyJSONforSR_       << std::endl;

  ofs << "regionsSet " << regionsSet_ << std::endl;
  if( mcSamples_!="" )       ofs << "mcSamples " << mcSamples_ << std::endl;
  if( sigSamples_!="" )      ofs << "sigSamples " << sigSamples_ << std::endl;
  if( dataSamples_!="" )     ofs << "dataSamples " << dataSamples_ << std::endl;
  if( zllmcSamples_!="" )    ofs << "zllmcSamples " << zllmcSamples_ << std::endl;
  if( llepmcSamples_!="" )    ofs << "llepmcSamples " << llepmcSamples_ << std::endl;
  if( zllsigSamples_!="" )    ofs << "zllsigSamples " << zllsigSamples_ << std::endl;
  if( llepsigSamples_!="" )    ofs << "llepsigSamples " << llepsigSamples_ << std::endl;
  if( additionalStuff_!="" ) ofs << "additionalStuff " << additionalStuff_ << std::endl;
  if( mcMLscore_!="" )         ofs << "mcMLscore "<<mcMLscore_<<std::endl;
  if( dataMLscore_!="" )         ofs << "dataMLscore "<<dataMLscore_<<std::endl;
  if( zllmcMLscore_!="" )         ofs << "zllmcMLscore "<<zllmcMLscore_<<std::endl;
  if( llepmcMLscore_!="" )         ofs << "llepmcMLscore "<<llepmcMLscore_<<std::endl;
  if( zllsigMLscore_!="" )         ofs << "zllsigMLscore "<<zllsigMLscore_<<std::endl;
  if( llepsigMLscore_!="" )         ofs << "llepsigMLscore "<<llepsigMLscore_<<std::endl;
  if( MLcut_>0 )                    ofs<< "cut on ML score: >= "<<MLcut_<<std::endl; else ofs<< "do not apply cut on ML score inclusively"<<std::endl;
  if( isMLcut_ )                   ofs<<"rejecting mc or signal events used for ML trainging and validation"<<std::endl; else ofs<<"not rejecting events used for ML training"<<std::endl;
  if( usebinMLcut_ )            ofs<<"use binned ML training and cut"<<std::endl;
  ofs << "gammaTemplateRegions " << gammaTemplateRegions_ << std::endl;
  ofs << "gammaTemplateType " << gammaTemplateType_ << std::endl;
  ofs << "gammaIsoCut " << gammaIsoCut_ << std::endl;
  ofs << "gamma2bMethod " << gamma2bMethod_ << std::endl;

  ofs << "zllRegions " << zllRegions_ << std::endl;

  std::cout << "[MT2Config] Saved config file as '" << filename << "'." << std::endl;

}
