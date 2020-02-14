#!bin/bash
for filename in /scratch/wjin/featurereduced4/rootfiles/2016/signal/T2qq*.root
do
    filename1=${filename##*/}
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2016_${filename1/".root"/"_binV5.npy"} "mt2" "T2qq" &
done

for filename in /scratch/wjin/featurereduced4/rootfiles/2017/signal/T2qq*.root
do
    filename1=${filename##*/}
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2017_${filename1/".root"/"_binV5.npy"} "mt2" "T2qq" &
done

wait

for filename in /scratch/wjin/featurereduced4/rootfiles/2016/signal/T2bb*.root
do
    filename1=${filename##*/}
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2016_${filename1/".root"/"_binV5.npy"} "mt2" "T2bb" &
done

for filename in /scratch/wjin/featurereduced4/rootfiles/2017/signal/T2bb*.root
do
    filename1=${filename##*/}
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2017_${filename1/".root"/"_binV5.npy"} "mt2" "T2bb" &
done

wait

for filename in /scratch/wjin/featurereduced4/rootfiles/2016/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2016_${filename1/".root"/"_binV5.npy"} "mt2" "T1bbbb" &
done

for filename in /scratch/wjin/featurereduced4/rootfiles/2017/signal/T1bbbb*.root
do
    filename1=${filename##*/}
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2017_${filename1/".root"/"_binV5.npy"} "mt2" "T1bbbb" &
done

wait

for filename in /scratch/wjin/featurereduced4/rootfiles/2016/signal/T1qqqq*.root
do
    filename1=${filename##*/}
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2016_${filename1/".root"/"_binV5.npy"} "mt2" "T1qqqq" &
done

for filename in /scratch/wjin/featurereduced4/rootfiles/2017/signal/T1qqqq*.root
do
    filename1=${filename##*/}
    python addMLscore_V05.py $filename /scratch/wjin/featurereduced4/sig_score/score1_2017_${filename1/".root"/"_binV5.npy"} "mt2" "T1qqqq" &
done

wait
