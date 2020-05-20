#!/bin/bash
for filename in /scratch/wjin/featurereduced4/sig_score/score1_2016*T2bb*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2016*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2016*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2016*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2016*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
wait

for filename in /scratch/wjin/featurereduced4/sig_score/score1_2017*T2bb*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2017*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2017*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2017*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2017*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
wait



for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT2bb_2018*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT2bb_2018*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT2bb_2018*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT2bb_2018*_binV10_2.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2bb
done&
wait
