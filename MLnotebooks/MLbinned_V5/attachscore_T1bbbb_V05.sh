#!/bin/bash

for filename in /scratch/wjin/featurereduced4/rootfiles/2016/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &



#for filename in /scratch/wjin/featurereduced4/rootfiles/2016/signal/T1bbbb*.root
#do
#    filename1=${filename##*/}
#    if [ -f /scratch/wjin/featurereduced4/sig_score/score1_2016_${filename1/".root"/"_binV5.npy"} ];then
#        echo "file exist"
#        continue
#    fi
#    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2016_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
#done &



for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &


for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &


for filename in /scratch/wjin/featurereduced4/rootfiles/2017/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2017_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2017_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &



#for filename in /scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb*.root
#do
#    filename1=${filename##*/}
#    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2017_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
#done


for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &



for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &


for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2017_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &


for filename in /scratch/wjin/featurereduced4/rootfiles/2018/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2018_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2018_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &

for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2018_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2018_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &

for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2018_${filename1/".root"/"_binV5.npy"} ];then
        echo "file exist"
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2018_${filename1/".root"/"_binV5.npy"} "mt2" T1bbbb
done &
wait
