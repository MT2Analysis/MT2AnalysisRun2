for filename in /scratch/wjin/featurereduced4/rootfiles/2016/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2016_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2016_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done

for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2016/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2016_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2016_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done

for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2016/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2016_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2016_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done

for filename in /scratch/wjin/featurereduced4/rootfiles/2017/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2017_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2017_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done

for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2017/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2017_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2017_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done

for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2017/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2017_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2017_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done

for filename in /scratch/wjin/featurereduced4/rootfiles/2018/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2018_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2018_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done

for filename in /scratch/wjin/featurereduced4/rootfiles_zinvcontrol/2018/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2018_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2018_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done

for filename in /scratch/wjin/featurereduced4/rootfiles_llcontrol/2018/data/*.root
do
    filename1=${filename##*/}
    if [ -f /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2018_${filename1/".root"/"_binV5_1.npy"} ];then
        continue
    fi
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2018_${filename1/".root"/"_binV5_1.npy"} "Events" T2bb
done
wait
