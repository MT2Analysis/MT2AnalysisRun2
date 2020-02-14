#!/bin/bash

for filename in /scratch/wjin/featurereduced4/rootfiles/2016/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2016_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done



for filename in /scratch/wjin/featurereduced4/rootfiles/2016/signal/T2bb*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2016_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done



for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2016_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done



for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2016_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done


for filename in /scratch/wjin/featurereduced4/rootfiles/2017/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2017_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done



for filename in /scratch/wjin/featurereduced4/rootfiles/2017/signal/T2bb*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2017_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done


for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2017_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done


for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2017_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done


for filename in /scratch/wjin/featurereduced4/rootfiles/2018/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2018_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done

for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2018_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done

for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/*.root
do
    filename1=${filename##*/}
    python addMLscore_V04.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2018_${filename1/".root"/"_binV4.npy"} "mt2" "T2bb" &
done
