#!/bin/bash
for filename in /scratch/wjin/featurereduced4/sig_score/score1_2016*T1bbbb*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2016*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2016*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
wait

for filename in /scratch/wjin/featurereduced4/sig_score/score1_2017*T1bbbb*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2017*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2017*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2017*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
wait



for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2018*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2018*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2018*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2018*_incluV3.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1bbbb
done&
wait
