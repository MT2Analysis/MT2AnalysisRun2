# Script to tar analysis output and copy to eos

VERSION="2019_02_25_V0"
CFGS=("moriond2019_35p9ifb_incl" "moriond2019_41p9ifb_incl_2017" "moriond2019_59p9ifb_incl_2018")
CFG="moriond2019_35p9ifb"
YEARS=("2016" "2017" "2018")

# 
arraylength=${#CFGS[@]}
plotExpr="incl*log*"

#create dir where to store all output
mkdir ana_$VERSION
cd ana_$VERSION
cp ../HTACCESS_0 .htaccess

# for each config, copy llep & zll control region plots, TF plots
for (( i=0; i<${arraylength}; i++ ));
do
  cfg=${CFGS[$i]}
  year=${YEARS[$i]}
  echo $cfg $year
  # data/MC pots
  mkdir llep_$year
  mkdir zll_$year/
  cp ../../EventYields_$cfg/llepControlRegion/plots/lepton/250ht-1_shape/$plotExpr llep_$year/.
  cp ../../EventYields_$cfg/zllControlRegion/plots/plotsDataMC_shape/$plotExpr zll_$year/.
  cp ../HTACCESS_1 llep_$year/.htaccess
  cp ../HTACCESS_1 zll_$year/.htaccess

  # TF plots
  mkdir ratios_llep_$year
  mkdir ratios_zll_$year
  cp ../../EventYields_$cfg/ratioPlots/ratios_llep/$plotExpr ratios_llep_$year
  cp ../../EventYields_$cfg/ratioPlots/ratios_zll/$plotExpr ratios_zll_$year
  cp ../HTACCESS_1 ratios_llep_$year/.htaccess
  cp ../HTACCESS_1 ratios_zll_$year/.htaccess

done
 
# for combined configs:
#   - combined control region plots, 
#   - combined TF plots
#   - combined RSFOF plots 
#   - combined SR plots
mkdir llep_combined
mkdir zll_combined
cp ../../combined_CRplots/llepControlRegion/lepton/250ht-1_shape/$plotExpr llep_combined/.
cp ../../combined_CRplots/zllControlRegion/SF_shape/$plotExpr zll_combined/.
cp ../HTACCESS_1 llep_combined/.htaccess
cp ../HTACCESS_1 zll_combined/.htaccess

mkdir ratios_llep_combined
mkdir ratios_zll_combined
cp ../../EventYields_${CFGS[$0]}/ratioPlots_combined/ratios_llep/$plotExpr ratios_llep_combined/
cp ../../EventYields_${CFGS[$0]}/ratioPlots_combined/ratios_zll/$plotExpr ratios_zll_combined/
cp ../HTACCESS_1 ratios_llep_combined/.htaccess
cp ../HTACCESS_1 ratios_zll_combined/.htaccess

mkdir ratios_RSFOF_combined
cp ../../ratioCombined/RSFOF*.p* ratios_RSFOF_combined
cp ../HTACCESS_1 ratios_RSFOF_combined/.htaccess

mkdir SRplots
cp ../../EventYields_$CFG/YieldComparison_dataMC_binned_combined/mt2_*_fullEstimate.* SRplots/.
cp ../HTACCESS_1 SRplots/.htaccess

mkdir MT2vsNb
cp -r ../../MT2vsNb/Zll MT2vsNb/.
cp -r ../../MT2vsNb/Zinv MT2vsNb/.
cp ../HTACCESS_1 MT2vsNb/.htaccess
cp ../HTACCESS_2 MT2vsNb/Zll/.htaccess
cp ../HTACCESS_2 MT2vsNb/Zinv/.htaccess

# tar everything and copy to eos
cd ../
tar -czvf ana_${VERSION}.tar.gz ana_$VERSION
scp ana_${VERSION}.tar.gz mratti@lxplus.cern.ch:/eos/user/m/mratti/www/MT2/.

