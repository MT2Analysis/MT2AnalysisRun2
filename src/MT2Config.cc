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

  regionsSet_ = "";
  mcSamples_ = "";
  qcdMCSamples_ = "";
  qcdMonoJetMCSamples_ = "";
  sigSamples_ = "";
  dataSamples_ = "";
  qcdDataSamples_ = "";
  qcdMonoJetDataSamples_ = "";
  additionalStuff_ = "";
  analysisType_ = "mt2";
  crRegionsSet_ = "13TeV_inclusive";
  qcdRegionsSet_ = "zurich_onlyHT";

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

  ofs << "regionsSet " << regionsSet_ << std::endl;
  if( mcSamples_!="" )       ofs << "mcSamples " << mcSamples_ << std::endl;
  if( sigSamples_!="" )      ofs << "sigSamples " << sigSamples_ << std::endl;
  if( dataSamples_!="" )     ofs << "dataSamples " << dataSamples_ << std::endl;
  if( additionalStuff_!="" ) ofs << "additionalStuff " << additionalStuff_ << std::endl;

  ofs << "gammaTemplateRegions " << gammaTemplateRegions_ << std::endl;
  ofs << "gammaTemplateType " << gammaTemplateType_ << std::endl;
  ofs << "gammaIsoCut " << gammaIsoCut_ << std::endl;
  ofs << "gamma2bMethod " << gamma2bMethod_ << std::endl;

  ofs << "zllRegions " << zllRegions_ << std::endl;

  std::cout << "[MT2Config] Saved config file as '" << filename << "'." << std::endl;

}
