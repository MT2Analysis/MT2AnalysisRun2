#!/bin/bash
for filename in /scratch/wjin/featurereduced4/sig_score/score1_2016*T1qqqq*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1qqqq_2016*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1qqqq_2016*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1qqqq_2016*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1qqqq_2016*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
wait

for filename in /scratch/wjin/featurereduced4/sig_score/score1_2017*T1qqqq*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1qqqq_2017*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1qqqq_2017*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1qqqq_2017*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1qqqq_2017*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
wait



for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1qqqq_2018*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1qqqq_2018*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1qqqq_2018*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1qqqq_2018*_BDTV3_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done&
wait
