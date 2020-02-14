for filename in /scratch/wjin/featurereduced4/rootfiles/2016/data/*.root
do
    filename1=${filename##*/}
    python addMLscore_V03.py $filename /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV3.npy"} "Events"
done

for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/data/*.root
do
    filename1=${filename##*/}
    python addMLscore_V03.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV3.npy"} "Events"
done

for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/data/*.root
do
    filename1=${filename##*/}
    python addMLscore_V03.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2016_${filename1/".root"/"_binV3.npy"} "Events"
done
