#!/bin/bash
for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT2bb_2016*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2016*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2016*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2016*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2016*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
wait
echo "2016 friendtree created"

for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT2bb_2017*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2017*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2017*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2017*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2017*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
wait
echo "2017 friendtree created"


for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2018*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2018*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2018*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2018*_binV5_1.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
wait
echo "2018 friendtree created"
