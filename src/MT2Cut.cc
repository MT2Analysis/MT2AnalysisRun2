#include "../interface/MT2Cut.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <map>
void MT2Cut::fillmap(std::string& cutsetcfgname) {
  std::cout << std::endl;
  std::cout << "-> Reading cut config file: " << cutsetcfgname << std::endl;
  std::cout << std::endl;
  std::ifstream IN(cutsetcfgname.c_str());
 // std::cout << IN.rdbuf();
  char buffer[200];
  char StringValue_cuts[200];
  char StringValue_bkgtrainfraction[200];
  char StringValue_sigtrainfraction[200];
  char StringValue_scorebranchname[1000];
  while( IN.getline(buffer, 200, '\n') ) {
    if (buffer[0] == '#') {
      continue; // Skip lines commented with '#'                                                                                                                                                                                 
    }
  cout<<buffer<<endl;
  char this_name_c[200];
  sscanf(buffer, "%s %s %s %s %s", this_name_c, StringValue_cuts, StringValue_bkgtrainfraction, StringValue_sigtrainfraction, StringValue_scorebranchname);
  std::string this_name(this_name_c); //cout<<this_name<<endl;
  regionnames.push_back(this_name);
  cuts[this_name]=atof(StringValue_cuts);
  bkgtrainfraction[this_name]=atof(StringValue_bkgtrainfraction);
  sigtrainfraction[this_name]=atof(StringValue_sigtrainfraction);
  scorebranchname[this_name]=std::string(StringValue_scorebranchname);
  }

}
bool MT2Cut::istrain_bkg(int MLtag, std::string& regionname) {
  if(MLtag<20*bkgtrainfraction[regionname]) return 1;
  else return 0;
}
bool MT2Cut::istrain_sig(int MLtag, std::string& regionname) {
  if(MLtag<20*sigtrainfraction[regionname]) return 1;
  else return 0;
}

