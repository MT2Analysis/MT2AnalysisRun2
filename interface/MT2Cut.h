#ifndef MT2Cut_h
#define Mt2Cut_h
#endif
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <map>
using namespace std;
class MT2Cut{

 public:

  MT2Cut(){};
  std::vector<std::string> regionnames;
  std::map<std::string, float> cuts;
  std::map<std::string, float> bkgtrainfraction;
  std::map<std::string, float> sigtrainfraction;
  std::map<std::string,std::string> scorebranchname;
  bool istrain_bkg(int MLtag, std::string&);
  bool istrain_sig(int MLtag, std::string&);
  void fillmap(std::string& cutsetcfgname);
  ~MT2Cut(){};
};
