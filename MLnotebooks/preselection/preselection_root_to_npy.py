import uproot
import pandas as pd
import numpy as np
'''
filesbkg=['/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_dyjetsll.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_gjets.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_qcd.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_singletop.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_tt.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_tt_negligible.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_wjets.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_ww_wz.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_zinv.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_dyjetsll.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_gjets.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_qcd.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_singletop.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_tt.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_tt_negligible.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_wjets.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_ww_wz.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2017_zinv.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_dyjetsll.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_gjets.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_qcd.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_singletop.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_tt.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_tt_negligible.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_wjets.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_ww_wz.root',
'/scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2018_zinv.root']

filessig=['/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1bbbb1.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1bbbb2.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1bbbb3.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1qqqq1.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1qqqq2.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1qqqq3.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T2bb1.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T2bb2.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T2bb3.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T2qq1.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T2qq2.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T2qq3.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1bbbb1.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1bbbb2.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1bbbb3.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1bbbb4.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1bbbb5.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1bbbb6.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1bbbb7.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1bbbb8.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1qqqq1.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1qqqq2.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1qqqq3.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1qqqq4.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1qqqq5.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1qqqq6.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1qqqq7.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T1qqqq8.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2bb1.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2bb2.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2bb3.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2bb4.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2bb5.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2bb6.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2bb7.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq1.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq2.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq3.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq4.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq5.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq6.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq7.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq8.root',
'/scratch/wjin/featurereduced2/rootfiles/sig/preselection_2017_T2qq9.root']
'''

filesbkg={

'preselection_2016_dyjetsll':['/scratch/wjin/featurereduced4/rootfiles/2016/dyjetsll_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/dyjetsll_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/dyjetsll_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/dyjetsll_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/dyjetsll_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/dyjetsll_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/dyjetsll_ht800to1200.root'],

'preselection_2016_gjets':['/scratch/wjin/featurereduced4/rootfiles/2016/gjets_dr0p05_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/gjets_dr0p05_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/gjets_dr0p05_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/gjets_dr0p05_ht40to100.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/gjets_dr0p05_ht600toInf.root'],

'preselection_2016_qcd':['/scratch/wjin/featurereduced4/rootfiles/2016/qcd_ht1000to1500.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/qcd_ht1500to2000.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/qcd_ht2000toInf.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/qcd_ht300to500.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/qcd_ht500to700.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/qcd_ht700to1000.root'],

'preselection_2016_singletop':['/scratch/wjin/featurereduced4/rootfiles/2016/singletop_schan.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/singletop_tchan_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/singletop_tchan_top.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/singletop_tW_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/singletop_tW_top.root'],

'preselection_2016_tt':['/scratch/wjin/featurereduced4/rootfiles/2016/ttsl_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/ttsl_top.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/ttw_lnu_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/ttw_qq_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/ttz_mg_lo.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/ttdl.root'],

'preselection_2016_tt_negligible':['/scratch/wjin/featurereduced4/rootfiles/2016/ttg_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/tttt.root'],

'preselection_2016_wjets':['/scratch/wjin/featurereduced4/rootfiles/2016/wjets_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/wjets_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/wjets_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/wjets_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/wjets_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/wjets_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/wjets_ht800to1200.root'],

'preselection_2016_ww_wz':['/scratch/wjin/featurereduced4/rootfiles/2016/ww.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/wz.root'],

'preselection_2016_zinv':['/scratch/wjin/featurereduced4/rootfiles/2016/zinv_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/zinv_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/zinv_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/zinv_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/zinv_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/zinv_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles/2016/zinv_ht800to1200.root'],

'preselection_2017_dyjetsll':['/scratch/wjin/featurereduced4/rootfiles/2017/dyjetsll_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/dyjetsll_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/dyjetsll_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/dyjetsll_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/dyjetsll_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/dyjetsll_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/dyjetsll_ht800to1200.root'],

'preselection_2017_gjets':['/scratch/wjin/featurereduced4/rootfiles/2017/gjets_dr0p05_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/gjets_dr0p05_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/gjets_dr0p05_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/gjets_dr0p05_ht40to100.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/gjets_dr0p05_ht600toInf.root'],

'preselection_2017_qcd':['/scratch/wjin/featurereduced4/rootfiles/2017/qcd_ht1000to1500.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/qcd_ht1500to2000.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/qcd_ht2000toInf.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/qcd_ht300to500.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/qcd_ht500to700.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/qcd_ht700to1000.root'],

'preselection_2017_singletop':['/scratch/wjin/featurereduced4/rootfiles/2017/singletop_schan.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/singletop_tW_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/singletop_tW_top.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/singletop_tchan_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/singletop_tchan_top.root'],

'preselection_2017_tt':["/scratch/wjin/featurereduced4/rootfiles/2017/ttdl_mg.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/ttsl_tbar_mg.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/ttsl_top_mg.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/ttz_mg.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/ttw_mg.root"],

'preselection_2017_tt_negligible':["/scratch/wjin/featurereduced4/rootfiles/2017/ttg_amcatnlo.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/tttt.root"],

'preselection_2017_wjets':["/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht100to200.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht1200to2500.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht1200to2500_1.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht200to400.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht2500toInf.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht2500toInf_1.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht400to600.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht600to800.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/wjets_ht800to1200.root"],

'preselection_2017_ww_wz':['/scratch/wjin/featurereduced4/rootfiles/2017/ww.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/wz.root'],

'preselection_2017_zinv':['/scratch/wjin/featurereduced4/rootfiles/2017/zinv_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/zinv_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/zinv_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/zinv_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/zinv_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/zinv_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles/2017/zinv_ht800to1200.root'],

'preselection_2018_dyjetsll':["/scratch/wjin/featurereduced4/rootfiles/2018/dyjetsll_ht100to200.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/dyjetsll_ht1200to2500.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/dyjetsll_ht200to400.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/dyjetsll_ht2500toInf.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/dyjetsll_ht400to600.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/dyjetsll_ht600to800.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/dyjetsll_ht800to1200.root"],

'preselection_2018_gjets':["/scratch/wjin/featurereduced4/rootfiles/2018/gjets_dr0p05_ht200to400.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/gjets_dr0p05_ht400to600.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/gjets_dr0p05_ht40to100.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/gjets_dr0p05_ht600toInf_ext1.root"],

'preselection_2018_qcd':["/scratch/wjin/featurereduced4/rootfiles/2018/qcd_ht1000to1500.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/qcd_ht1500to2000.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/qcd_ht2000toInf.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/qcd_ht300to500.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/qcd_ht500to700.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/qcd_ht700to1000.root"],

'preselection_2018_singletop':["/scratch/wjin/featurereduced4/rootfiles/2018/singletop_schan.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/singletop_tW_tbar.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/singletop_tW_top.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/singletop_tchan_tbar.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/singletop_tchan_top.root"],

'preselection_2018_tt':["/scratch/wjin/featurereduced4/rootfiles/2018/ttdl_mg.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/ttsl_tbar_mg.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/ttsl_top_mg.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/ttz_mg.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/ttw_mg.root"],

'preselection_2018_tt_negligible':["/scratch/wjin/featurereduced4/rootfiles/2018/tttt.root"],

'preselection_2018_wjets':["/scratch/wjin/featurereduced4/rootfiles/2018/wjets_ht100to200.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/wjets_ht1200to2500.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/wjets_ht200to400.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/wjets_ht2500toInf.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/wjets_ht400to600.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/wjets_ht600to800.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/wjets_ht800to1200.root"],

'preselection_2018_ww_wz':["/scratch/wjin/featurereduced4/rootfiles/2018/ww.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/wz.root"],

'preselection_2018_zinv':["/scratch/wjin/featurereduced4/rootfiles/2018/zinv_ht100to200.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/zinv_ht1200to2500.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/zinv_ht200to400.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/zinv_ht2500toInf.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/zinv_ht400to600.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/zinv_ht600to800.root",
"/scratch/wjin/featurereduced4/rootfiles/2018/zinv_ht800to1200.root"]

}



filessig=["/scratch/wjin/featurereduced4/rootfiles/2016/signal/T1bbbb_94x_1.root",
"/scratch/wjin/featurereduced4/rootfiles/2016/signal/T1bbbb_94x_2.root",
"/scratch/wjin/featurereduced4/rootfiles/2016/signal/T1bbbb_94x.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_1.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_2.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1_1.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1_2.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1_3.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1_4.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb.root"]
''',
"/work/wjin/featurereduced4/rootfiles/2016/signal/T1qqqq_94x_1.root",
"/work/wjin/featurereduced4/rootfiles/2016/signal/T1qqqq_94x_2.root",
"/work/wjin/featurereduced4/rootfiles/2016/signal/T1qqqq_94x.root",
"/work/wjin/featurereduced4/rootfiles/2016/signal/T2bb_1.root",
"/work/wjin/featurereduced4/rootfiles/2016/signal/T2bb_mSbot1650to2600.root",
"/work/wjin/featurereduced4/rootfiles/2016/signal/T2bb.root",
"/work/wjin/featurereduced4/rootfiles/2016/signal/T2qq_1.root",
"/work/wjin/featurereduced4/rootfiles/2016/signal/T2qq_mSq1850to2600.root",
"/work/wjin/featurereduced4/rootfiles/2016/signal/T2qq.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq_1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq_2.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq_ext1_1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq_ext1_2.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq_ext1_3.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq_ext1_4.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq_ext1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2bb_1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2bb_ext1_1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2bb_ext1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2bb_mSbot1650to2600_ext1_1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2bb_mSbot1650to2600_ext1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2bb_mSbot1650to2600.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2bb.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq_1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq_2.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq_ext1_1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq_ext1_2.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq_ext1_3.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq_ext1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq_mSq_1850to2600_ext1.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq_mSq_1850to2600.root",
"/work/wjin/featurereduced4/rootfiles/2017/signal/T2qq.root"]
'''

'''
files_llcontrol={'preselection_2016_dyjetsll':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/dyjetsll_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/dyjetsll_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/dyjetsll_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/dyjetsll_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/dyjetsll_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/dyjetsll_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/dyjetsll_ht800to1200.root'],

'preselection_2016_gjets':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/gjets_dr0p05_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/gjets_dr0p05_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/gjets_dr0p05_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/gjets_dr0p05_ht40to100.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/gjets_dr0p05_ht600toInf.root'],

'preselection_2016_qcd':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/qcd_ht1000to1500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/qcd_ht1500to2000.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/qcd_ht2000toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/qcd_ht300to500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/qcd_ht500to700.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/qcd_ht700to1000.root'],

'preselection_2016_singletop':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/singletop_schan.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/singletop_tchan_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/singletop_tchan_top.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/singletop_tW_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/singletop_tW_top.root'],

'preselection_2016_tt':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/ttsl_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/ttsl_top.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/ttw_lnu_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/ttw_qq_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/ttz_mg_lo.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/ttdl.root'],

'preselection_2016_tt_negligible':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/ttg_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/tttt.root'],

'preselection_2016_wjets':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/wjets_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/wjets_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/wjets_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/wjets_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/wjets_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/wjets_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/wjets_ht800to1200.root'],

'preselection_2016_ww_wz':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/ww.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/wz.root'],

'preselection_2016_zinv':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/zinv_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/zinv_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/zinv_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/zinv_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/zinv_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/zinv_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/zinv_ht800to1200.root']}


files_zinvcontrol={'preselection_2016_dyjetsll':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/dyjetsll_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/dyjetsll_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/dyjetsll_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/dyjetsll_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/dyjetsll_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/dyjetsll_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/dyjetsll_ht800to1200.root'],

'preselection_2016_gjets':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/gjets_dr0p05_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/gjets_dr0p05_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/gjets_dr0p05_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/gjets_dr0p05_ht40to100.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/gjets_dr0p05_ht600toInf.root'],

'preselection_2016_qcd':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/qcd_ht1000to1500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/qcd_ht1500to2000.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/qcd_ht2000toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/qcd_ht300to500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/qcd_ht500to700.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/qcd_ht700to1000.root'],

'preselection_2016_singletop':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/singletop_schan.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/singletop_tchan_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/singletop_tchan_top.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/singletop_tW_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/singletop_tW_top.root'],

'preselection_2016_tt':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/ttsl_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/ttsl_top.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/ttw_lnu_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/ttw_qq_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/ttz_mg_lo.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/ttdl.root'],

'preselection_2016_tt_negligible':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/ttg_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/tttt.root'],

'preselection_2016_wjets':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/wjets_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/wjets_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/wjets_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/wjets_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/wjets_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/wjets_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/wjets_ht800to1200.root'],

'preselection_2016_ww_wz':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/ww.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/wz.root'],

'preselection_2016_zinv':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/zinv_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/zinv_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/zinv_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/zinv_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/zinv_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/zinv_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/zinv_ht800to1200.root']}


'''
'''

files_llcontrol={'preselection_2017_dyjetsll':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/dyjetsll_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/dyjetsll_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/dyjetsll_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/dyjetsll_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/dyjetsll_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/dyjetsll_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/dyjetsll_ht800to1200.root'],

'preselection_2017_gjets':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/gjets_dr0p05_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/gjets_dr0p05_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/gjets_dr0p05_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/gjets_dr0p05_ht40to100.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/gjets_dr0p05_ht600toInf.root'],

'preselection_2017_qcd':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/qcd_ht1000to1500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/qcd_ht1500to2000.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/qcd_ht2000toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/qcd_ht300to500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/qcd_ht500to700.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/qcd_ht700to1000.root'],

'preselection_2017_singletop':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/singletop_schan.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/singletop_tchan_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/singletop_tchan_top.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/singletop_tW_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/singletop_tW_top.root'],

'preselection_2017_tt':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/ttsl_tbar_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/ttsl_top_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/ttw_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/ttz_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/ttdl_mg.root'],

'preselection_2017_tt_negligible':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/ttg_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/tttt.root'],

'preselection_2017_wjets':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht1200to2500_1.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht2500toInf_1.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wjets_ht800to1200.root'],

'preselection_2017_ww_wz':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/ww.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/wz.root'],

'preselection_2017_zinv':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/zinv_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/zinv_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/zinv_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/zinv_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/zinv_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/zinv_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/zinv_ht800to1200.root'],


'preselection_2018_dyjetsll':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/dyjetsll_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/dyjetsll_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/dyjetsll_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/dyjetsll_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/dyjetsll_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/dyjetsll_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/dyjetsll_ht800to1200.root'],

'preselection_2018_gjets':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/gjets_dr0p05_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/gjets_dr0p05_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/gjets_dr0p05_ht40to100.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/gjets_dr0p05_ht600toInf_ext1.root'],

'preselection_2018_qcd':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/qcd_ht1000to1500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/qcd_ht1500to2000.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/qcd_ht2000toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/qcd_ht300to500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/qcd_ht500to700.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/qcd_ht700to1000.root'],

'preselection_2018_singletop':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/singletop_schan.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/singletop_tchan_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/singletop_tchan_top.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/singletop_tW_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/singletop_tW_top.root'],

'preselection_2018_tt':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/ttsl_tbar_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/ttsl_top_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/ttw_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/ttz_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/ttdl_mg.root'],

'preselection_2018_tt_negligible':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/tttt.root'],

'preselection_2018_wjets':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/wjets_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/wjets_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/wjets_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/wjets_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/wjets_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/wjets_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/wjets_ht800to1200.root'],

'preselection_2018_ww_wz':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/ww.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/wz.root'],

'preselection_2018_zinv':['/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/zinv_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/zinv_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/zinv_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/zinv_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/zinv_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/zinv_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/zinv_ht800to1200.root']}

files_zinvcontrol={'preselection_2017_dyjetsll':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/dyjetsll_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/dyjetsll_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/dyjetsll_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/dyjetsll_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/dyjetsll_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/dyjetsll_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/dyjetsll_ht800to1200.root'],

'preselection_2017_gjets':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/gjets_dr0p05_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/gjets_dr0p05_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/gjets_dr0p05_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/gjets_dr0p05_ht40to100.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/gjets_dr0p05_ht600toInf.root'],

'preselection_2017_qcd':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/qcd_ht1000to1500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/qcd_ht1500to2000.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/qcd_ht2000toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/qcd_ht300to500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/qcd_ht500to700.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/qcd_ht700to1000.root'],

'preselection_2017_singletop':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/singletop_schan.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/singletop_tchan_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/singletop_tchan_top.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/singletop_tW_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/singletop_tW_top.root'],

'preselection_2017_tt':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/ttsl_tbar_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/ttsl_top_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/ttw_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/ttz_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/ttdl_mg.root'],

'preselection_2017_tt_negligible':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/ttg_amcatnlo.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/tttt.root'],

'preselection_2017_wjets':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht1200to2500_1.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht2500toInf_1.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wjets_ht800to1200.root'],

'preselection_2017_ww_wz':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/ww.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/wz.root'],

'preselection_2017_zinv':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/zinv_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/zinv_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/zinv_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/zinv_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/zinv_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/zinv_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/zinv_ht800to1200.root'],


'preselection_2018_dyjetsll':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/dyjetsll_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/dyjetsll_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/dyjetsll_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/dyjetsll_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/dyjetsll_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/dyjetsll_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/dyjetsll_ht800to1200.root'],

'preselection_2018_gjets':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/gjets_dr0p05_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/gjets_dr0p05_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/gjets_dr0p05_ht40to100.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/gjets_dr0p05_ht600toInf_ext1.root'],

'preselection_2018_qcd':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/qcd_ht1000to1500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/qcd_ht1500to2000.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/qcd_ht2000toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/qcd_ht300to500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/qcd_ht500to700.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/qcd_ht700to1000.root'],

'preselection_2018_singletop':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/singletop_schan.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/singletop_tchan_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/singletop_tchan_top.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/singletop_tW_tbar.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/singletop_tW_top.root'],

'preselection_2018_tt':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/ttsl_tbar_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/ttsl_top_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/ttw_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/ttz_mg.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/ttdl_mg.root'],

'preselection_2018_tt_negligible':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/tttt.root'],

'preselection_2018_wjets':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/wjets_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/wjets_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/wjets_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/wjets_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/wjets_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/wjets_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/wjets_ht800to1200.root'],

'preselection_2018_ww_wz':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/ww.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/wz.root'],

'preselection_2018_zinv':['/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/zinv_ht100to200.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/zinv_ht1200to2500.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/zinv_ht200to400.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/zinv_ht2500toInf.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/zinv_ht400to600.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/zinv_ht600to800.root',
'/scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/zinv_ht800to1200.root']}
'''
features=['deltaPhiMin','diffMetMht',
           'jet1_eta','jet1_phi','jet1_btagDeepCSV','jet2_eta','jet2_phi','jet2_btagDeepCSV','jet3_eta','jet3_phi','jet3_btagDeepCSV',
          'jet4_eta','jet4_phi','jet4_btagDeepCSV','jet5_eta','jet5_phi','jet5_btagDeepCSV','jet6_eta','jet6_phi','jet6_btagDeepCSV',
          'jet7_eta','jet7_phi','jet7_btagDeepCSV','jet8_eta','jet8_phi','jet8_btagDeepCSV','jet9_eta','jet9_phi','jet9_btagDeepCSV',
          'jet10_eta','jet10_phi','jet10_btagDeepCSV','jet11_eta','jet11_phi','jet11_btagDeepCSV','jet12_eta','jet12_phi','jet12_btagDeepCSV',
          'jet13_eta','jet13_phi','jet13_btagDeepCSV','jet14_eta','jet14_phi','jet14_btagDeepCSV','jet15_eta','jet15_phi','jet15_btagDeepCSV',
           'jet1_pt','jet1_mass','jet2_pt','jet2_mass','jet3_pt','jet3_mass','jet4_pt','jet4_mass','jet5_pt','jet5_mass',
              'jet6_pt','jet6_mass','jet7_pt','jet7_mass','jet8_pt','jet8_mass','jet9_pt','jet9_mass','jet10_pt','jet10_mass',
              'jet11_pt','jet11_mass','jet12_pt','jet12_mass','jet13_pt','jet13_mass','jet14_pt','jet14_mass','jet15_pt','jet15_mass',
         'nJet30','nJet40','nBJet20','nBJet30','nBJet40','ht','mt2','mht_pt','met_pt','mht_phi','met_phi','njet',
         'evt_scale1fb',"GenSusyMScan1","GenSusyMScan2",'evt','evt_id','absweight',
          'MLtag' 


         ]

#for files in filesbkg:
'''
for files in ['preselection_2016_singletop','preselection_2016_tt','preselection_2016_wjets','preselection_2016_zinv',
              'preselection_2017_singletop','preselection_2017_tt','preselection_2017_wjets','preselection_2017_zinv',
              'preselection_2018_singletop','preselection_2018_tt','preselection_2018_wjets','preselection_2018_zinv']:
    i=False;
    for fn in filesbkg[files]:
        tree=uproot.open(fn)["mt2"]
        arrays=tree.arrays(features)
        print("load samples from"+fn)
        if i:
            for feature in features:
                arraystotal[str.encode(feature)]=np.concatenate((arraystotal[str.encode(feature)],arrays[str.encode(feature)]))
        else:
            arraystotal=arrays
            i=True
    np.save('/scratch/wjin/featurereduced4/bkg/'+files+'_skim.npy', arraystotal)
'''

#for fn in filessig:
for fn in ["/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_1.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_2.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1_1.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1_2.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1_3.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1_4.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb_ext1.root",
"/scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb.root"]:
    tree=uproot.open(fn)["mt2"]
    arrays=tree.arrays(features)
    print("load samples from"+fn)
    np.save('/scratch/wjin/featurereduced4/sig/preselection_'
            +fn.replace("/scratch/wjin/featurereduced4/rootfiles/",'').replace("/signal/",'_').replace(".root",'')+'_skim.npy', arrays)

'''

for files in files_llcontrol:
    i=False;
    for fn in files_llcontrol[files]:
        tree=uproot.open(fn)["mt2"]
        arrays=tree.arrays(features)
        print("load samples from"+fn)
        if i:
            for feature in features:
                arraystotal[str.encode(feature)]=np.concatenate((arraystotal[str.encode(feature)],arrays[str.encode(feature)]))
        else:
            arraystotal=arrays
            i=True
    np.save('/scratch/wjin/featurereduced4/llcontrol/'+files+'_skim.npy', arraystotal)

for files in files_zinvcontrol:
    i=False;
    for fn in files_zinvcontrol[files]:
        tree=uproot.open(fn)["mt2"]
        arrays=tree.arrays(features)
        print("load samples from"+fn)
        if i:
            for feature in features:
                arraystotal[str.encode(feature)]=np.concatenate((arraystotal[str.encode(feature)],arrays[str.encode(feature)]))
        else:
            arraystotal=arrays
            i=True
    np.save('/scratch/wjin/featurereduced4/zinvcontrol/'+files+'_skim.npy', arraystotal)





'''







