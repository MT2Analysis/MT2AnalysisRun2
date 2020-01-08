#ifndef MT2Cut_h
#define Mt2Cut_h

#include <string>
#include <vector>

class MT2Cut{

 public:

  MT2Cut();
  std::vector<std::string> regionnames;
  std::map <std::string,float> cuts;
  std::map <std::string,float> bkgtrainfraction;
  std::map <std::string,float> sigtrainfraction;
  std::map <std::string,std::string> scorebranchname;
  void fillmap(std::string& cutsetcfgname) const;
  ~MT2Cut(){};
}
