#ifndef MT2Sample_h
#define MT2Sample_h

#include <string>
#include <vector>




class MT2Sample {


 public:

  MT2Sample();
  ~MT2Sample();


  static std::vector<MT2Sample> loadSamples(const std::string& filename, const std::string& filter="", int idMin=-1, int idMax=-1, int isETH=1);
  static std::vector<MT2Sample> loadSamples(const std::string& filename, int idMin, int idMax=-1, int isETH=1);
  static std::vector<MT2Sample> loadSamples(const std::string& filename, const std::string& scorefilename="",const std::string& filter="", int idMin=-1, int idMax=-1, int isETH=1);
  // publica data members:
  std::string name;
  std::string sname;
  std::string dir;
  std::string file;
  std::string score="";
  int id;
  int nevents;
  float xsection;
  float lumi;
  float kfact;
  float filter;
  float scale1fb;
  float PU_avg_weight;

 private:


};

#endif
