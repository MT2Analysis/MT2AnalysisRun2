#!/bin/bash
#python addMLscore.py /scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_qcd.root /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016_qcd_btagDeepCSV_excluded.root model_T1bbbb2016_unweighted_btagDeepCSV_excluded.h5
#python addMLscore.py /scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_singletop.root /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016_singletop_btagDeepCSV_excluded.root model_T1bbbb2016_unweighted_btagDeepCSV_excluded.h5
#python addMLscore.py /scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_tt.root /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016_tt_btagDeepCSV_excluded.root model_T1bbbb2016_unweighted_btagDeepCSV_excluded.h5
#python addMLscore.py /scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_wjets.root /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016_wjets_btagDeepCSV_excluded.root model_T1bbbb2016_unweighted_btagDeepCSV_excluded.h5
#python addMLscore.py /scratch/wjin/featurereduced2/rootfiles/bkg/preselection_2016_zinv.root /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016_zinv_btagDeepCSV_excluded.root model_T1bbbb2016_unweighted_btagDeepCSV_excluded.h5
#python addMLscore.py /scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1bbbb1.root /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2016_T1bbbb1_btagDeepCSV_excluded.root model_T1bbbb2016_unweighted_btagDeepCSV_excluded.h5
#python addMLscore.py /scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1bbbb2.root /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2016_T1bbbb2_btagDeepCSV_excluded.root model_T1bbbb2016_unweighted_btagDeepCSV_excluded.h5
#python addMLscore.py /scratch/wjin/featurereduced2/rootfiles/sig/preselection_2016_T1bbbb3.root /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2016_T1bbbb3_btagDeepCSV_excluded.root model_T1bbbb2016_unweighted_btagDeepCSV_excluded.h5

:'
for filename in /scratch/wjin/featurereduced4/rootfiles/2016/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done



for filename in /scratch/wjin/featurereduced4/rootfiles/2016/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2016_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done



for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done



for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done


for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done


for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5

done
'
for filename in /scratch/wjin/featurereduced4/rootfiles/2017/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2017_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done



for filename in /scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2017_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done



for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done



for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done


for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done


for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5

done

for filename in /scratch/wjin/featurereduced4/rootfiles/2018/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2018_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done

for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2018_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done

for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/*.root
do
    filename1=${filename##*/}
    python addMLscore.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2018_${filename1/".root"/"_btagDeepCSV_excluded.npy"} ../model_T1bbbb2016_unweighted_btagDeepCSV_excluded_V02.h5
done