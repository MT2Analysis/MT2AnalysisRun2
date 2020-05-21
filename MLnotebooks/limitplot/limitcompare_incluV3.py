import numpy as np
import sys
import matplotlib.pyplot as plt

def limitread(filename):
    print("open: ",filename)
    with open(filename) as f:
        for line in f:
            limits=line.split()
            if len(limits)>0 and limits[0]=="Expected":
                if limits[1]=="2.5%:":
                   result1=float(limits[4])
                elif limits[1]=="16.0%:":
                   result2=float(limits[4])
                elif limits[1]=="50.0%:":
                   result3=float(limits[4])
                elif limits[1]=="84.0%:":
                   result4=float(limits[4])
                elif limits[1]=="97.5%:":
                   result5=float(limits[4])
    results=[result1,result2, result3,result4,result5]
    return results

def plotlimit_robust(filenamelist,labels,title):
#    result_origin=limitread(filename_origin)
#    result_optimize_1=limitread(filename_optimize_1)
#    result_optimize_2=limitread(filename_optimize_2)
#    result_optimize_3=limitread(filename_optimize_3)
    result=[]
    for filename in filenamelist:
        result.append(limitread(filename))
    num_result=len(filenamelist)
    fig=plt.figure(figsize=(16,10))
    ax=fig.add_subplot(111)
    positions=[i+1 for i in range(num_result)]
    ax.set_xlim([0,num_result+1])
    ax.set_xticks(positions)
    ax.set_xticklabels(labels)
    ax.bar([i+1 for i in range(num_result)],[result[i][3]-result[i][1] for i in range(num_result)],bottom=[result[i][1] for i in range(num_result)],width=0.5,color="blue",alpha=0.5,label="16.0% to 84.0% expected limit")
    lowererror=[result[i][2]-result[i][0] for i in range(num_result)]
    uperror   =[result[i][4]-result[i][2] for i in range(num_result)]
    ax.errorbar([i+1 for i in range(num_result)],[result[i][2] for i in range(num_result)],yerr=[lowererror,uperror],ecolor="blue", c="blue",fmt="o",label="50.0% expected limit in 2.5% to 97.5% expected limit range")
    ax.legend(loc='best',prop={'size': 16})
#    ax.set_xlabel("dataset",fontsize=20)
    ax.set_ylabel("limit of signal strength "+r"$\mu$",fontsize=20)
    ax.tick_params(axis='both', which='major', labelsize=15)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.set_title(title,size=22)
    return fig

directory_T1bbbb_origin = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1bbbb_btagexclu_noMLcutallevents/datacards_T1bbbb/"
directory_T1bbbb_BDT_1      = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1bbbb_BDTV3_2_1/datacards_T1bbbb/"
directory_T1bbbb_NN_1      = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1bbbb_binV10_2_1/datacards_T1bbbb/"
directory_T1bbbb_incluNN3_1 = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1bbbb_incluV3_1/datacards_T1bbbb/"

directory_T1qqqq_origin = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1bbbb_btagexclu_noMLcutallevents/datacards_T1qqqq/"
directory_T1qqqq_BDT_1      = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1qqqq_BDTV3_2_1/datacards_T1qqqq/"
directory_T1qqqq_NN_1      = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1qqqq_binV10_2_1/datacards_T1qqqq/"
directory_T1qqqq_incluNN3_1 = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1qqqq_incluV3_1/datacards_T1qqqq/"

directory_T2bb_origin = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1bbbb_btagexclu_noMLcutallevents/datacards_T2bb/"
directory_T2bb_BDT_1      = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT2bb_BDTV3_2_1/datacards_T2bb/"
directory_T2bb_NN_1      = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT2bb_binV10_2_1/datacards_T2bb/"
directory_T2bb_incluNN3_1 = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT2bb_incluV3_1/datacards_T2bb/"

directory_T2qq_origin = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT1bbbb_btagexclu_noMLcutallevents/datacards_T2qq/"
directory_T2qq_BDT_1      = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT2qq_BDTV3_2_1/datacards_T2qq/"
directory_T2qq_NN_1      = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT2qq_binV10_2_1/datacards_T2qq/"
directory_T2qq_incluNN3_1 = "/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/EventYields_config_MLoptimize_2016_ForT2qq_incluV3_1/datacards_T2qq/"

points_T1bbbb=["log_T1bbbb_1400_1350_combined.txt",
               "log_T1bbbb_2000_1600_combined.txt",
               "log_T1bbbb_2400_1400_combined.txt",
               "log_T1bbbb_2400_200_combined.txt",
               "log_T1bbbb_2600_1400_combined.txt",
               "log_T1bbbb_2600_200_combined.txt",
               "log_T1bbbb_1600_1550_combined.txt",
               "log_T1bbbb_2200_1800_combined.txt"]

points_T1qqqq=["log_T1qqqq_1400_1300_combined.txt",
               "log_T1qqqq_1800_1200_combined.txt",
               "log_T1qqqq_2200_200_combined.txt",
               "log_T1qqqq_2200_800_combined.txt",
               "log_T1qqqq_1600_1500_combined.txt",
               "log_T1qqqq_2000_1400_combined.txt",
               "log_T1qqqq_2400_200_combined.txt",
               "log_T1qqqq_2400_800_combined.txt"]

points_T2bb=["log_T2bb_800_700_combined.txt",
             "log_T2bb_1100_700_combined.txt",
             "log_T2bb_1400_100_combined.txt",
             "log_T2bb_1400_500_combined.txt",
             "log_T2bb_1000_900_combined.txt",
             "log_T2bb_1300_900_combined.txt",
             "log_T2bb_1600_200_combined.txt",
             "log_T2bb_1600_500_combined.txt"]

points_T2qq=["log_T2qq_1100_900_combined.txt",
             "log_T2qq_1500_900_combined.txt",
             "log_T2qq_1900_200_combined.txt",
             "log_T2qq_1900_600_combined.txt",
             "log_T2qq_1300_1100_combined.txt",
             "log_T2qq_1700_1100_combined.txt",
             "log_T2qq_2100_200_combined.txt",
             "log_T2qq_2100_600_combined.txt"]
masses_T1bbbb=[[1400,1350],[2000,1600],[2400,1400],[2400,200],[2600,1400],[2600,200],[1600,1550],[2200,1800]]
masses_T1qqqq=[[1400,1300],[1800,1200],[2200,200],[2200,800],[1600,1500],[2000,1400],[2400,200],[2400,800]]
masses_T2bb  =[[800,700],[1100,700],[1400,100],[1400,500],[1000,900],[1300,900],[1600,200],[1600,500]]
masses_T2qq  =[[1100,900],[1500,900],[1900,200],[1900,600],[1300,1100],[1700,1100],[2100,200],[2100,600]]
labels=("nominal","after optimize (GBDT)","after optimize (NN)","after optimize (incluNN)")

for i in range(8):
    point=points_T1bbbb[i]
    mass=masses_T1bbbb[i]
    figure=plotlimit_robust([directory_T1bbbb_origin+point,directory_T1bbbb_BDT_1+point,directory_T1bbbb_NN_1+point,directory_T1bbbb_incluNN3_1+point],labels,"Expected Limit comparison for T1bbbb mass "+str(mass[0])+" "+str(mass[1]))
    figure.savefig("limit_T1bbbb_"+str(mass[0])+"_"+str(mass[1])+"_compareincluNN3"+".png")
    figure.savefig("limit_T1bbbb_"+str(mass[0])+"_"+str(mass[1])+"_compareincluNN3"+".pdf")
    plt.close('all')

for i in range(8):
    point=points_T1qqqq[i]
    mass=masses_T1qqqq[i]
    figure=plotlimit_robust([directory_T1qqqq_origin+point,directory_T1qqqq_BDT_1+point,directory_T1qqqq_NN_1+point,directory_T1qqqq_incluNN3_1+point],labels,"Expected Limit comparison for T1qqqq mass "+str(mass[0])+" "+str(mass[1]))
    figure.savefig("limit_T1qqqq_"+str(mass[0])+"_"+str(mass[1])+"_compareincluNN3"+".png")
    figure.savefig("limit_T1qqqq_"+str(mass[0])+"_"+str(mass[1])+"_compareincluNN3"+".pdf")
    plt.close('all')

for i in range(8):
    point=points_T2bb[i]
    mass=masses_T2bb[i]
    figure=plotlimit_robust([directory_T2bb_origin+point,directory_T2bb_BDT_1+point,directory_T2bb_NN_1+point,directory_T2bb_incluNN3_1+point],labels,"Expected Limit comparison for T2bb mass "+str(mass[0])+" "+str(mass[1]))
    figure.savefig("limit_T2bb_"+str(mass[0])+"_"+str(mass[1])+"_compareincluNN3"+".png")
    figure.savefig("limit_T2bb_"+str(mass[0])+"_"+str(mass[1])+"_compareincluNN3"+".pdf")
    plt.close('all')

for i in range(8):
    point=points_T2qq[i]
    mass=masses_T2qq[i]
    figure=plotlimit_robust([directory_T2qq_origin+point,directory_T2qq_BDT_1+point,directory_T2qq_NN_1+point,directory_T2qq_incluNN3_1+point],labels,"Expected Limit comparison for T2qq mass "+str(mass[0])+" "+str(mass[1]))
    figure.savefig("limit_T2qq_"+str(mass[0])+"_"+str(mass[1])+"_compareincluNN3"+".png")
    figure.savefig("limit_T2qq_"+str(mass[0])+"_"+str(mass[1])+"_compareincluNN3"+".pdf")
    plt.close('all')
