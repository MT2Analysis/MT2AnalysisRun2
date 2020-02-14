#!/bin/bash
for filename in /scratch/wjin/featurereduced4/sig_score/score1*T1bbbb*binV5.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb &
done
wait
echo "T1bbbb score created"
for filename in /scratch/wjin/featurereduced4/sig_score/score1*T1qqqq*binV5.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq &
done
wait
echo "T1qqqq score created"
for filename in /scratch/wjin/featurereduced4/sig_score/score1*T2bb*binV5.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb &
done
wait
echo "T2bb score created"
for filename in /scratch/wjin/featurereduced4/sig_score/score1*T2qq*binV5.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq &
done
wait
echo "T2qq score created"
