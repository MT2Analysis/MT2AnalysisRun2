

#python cutflow.py -f /shome/mratti/heppy_workarea/CMSSW_9_4_1/src/CMGTools/TTHAnalysis/cfg/testmc_94X_std_5K/WJetsToLNu_HT-600To800/mt2.root -s Wlv -t mt2 -y 2017 -g SR_kinematic -d mini
CR2lep_kinematic

python cutflow.py -s data -g CR2lep_kinematic -d nano -t Events -y 2017 -f $XRDLOC//pnfs/psi.ch/cms/trivcat/store/user/mratti/merged_nanoMT2/2017_V03_V11_V0/merged/merged_Run2017F_noDupl.root 

python cutflow.py -s data -g CR2lep_kinematic -d nano -t Events -y 2018 -f $XRDLOC//pnfs/psi.ch/cms/trivcat/store/user/mratti/merged_nanoMT2/2018_V03_V14_V0/merged/merged_Run2018A_noDupl.root



python cutflow.py -s Wlv -g SR_kinematic -d nano  -t Events -y 2017 -f /shome/mratti/nanoaod_workarea/CMSSW_10_0_3/src/PhysicsTools/NanoAODTools/python/postprocessing/analysis/mt2/output/testmc_94X_nano_std/test94X_Wlv_NANO_5K_noselIT_Skim.root
python cutflow.py -s Wlv -g SR_kinematic -d mini  -t mt2 -y 2017 -f /shome/mratti/heppy_workarea/CMSSW_9_4_1/src/CMGTools/TTHAnalysis/cfg/testmc_94X_std_5K_noEleId_v2/WJetsToLNu_HT-600To800/mt2.root

python cutflow.py -s Wlv -g CR1lep_kinematic -d nano  -t Events -y 2017 -f /shome/mratti/nanoaod_workarea/CMSSW_10_0_3/src/PhysicsTools/NanoAODTools/python/postprocessing/analysis/mt2/output/testmc_94X_nano_std/test94X_Wlv_NANO_5K_noselIT_Skim.root
python cutflow.py -s Wlv -g CR1lep_kinematic -d mini  -t mt2 -y 2017 -f /shome/mratti/heppy_workarea/CMSSW_9_4_1/src/CMGTools/TTHAnalysis/cfg/testmc_94X_std_5K_noEleId_v2/WJetsToLNu_HT-600To800/mt2.root

python cutflow.py -s Zll -g CR2lep_kinematic -d nano  -t Events -y 2017 -f /shome/mratti/nanoaod_workarea/CMSSW_10_0_3/src/PhysicsTools/NanoAODTools/python/postprocessing/analysis/mt2/output/testmc_94X_nano_std_Zll/test94X_Zll_NANO_5K_nodxyIT_Skim.root
python cutflow.py -s Zll -g CR2lep_kinematic -d mini  -t mt2 -y 2017 -f /shome/mratti/heppy_workarea/CMSSW_9_4_1/src/CMGTools/TTHAnalysis/cfg/testmc_94X_std_5K_noEleId_v2_Zll/DYJetsToLL_M-50_HT-600to800/mt2.root
