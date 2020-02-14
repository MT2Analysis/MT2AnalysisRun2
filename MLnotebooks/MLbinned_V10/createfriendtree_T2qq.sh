#!/bin/bash
for filename in /work/wjin/featurereduced4/sig_score/score1_2016*T2qq*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/llcontrol_score/score1_forT2qq_2016*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/zinvcontrol_score/score1_forT2qq_2016*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/bkg_score/score1_forT2qq_2016*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/datasigregion_score/score1_forT2qq_2016*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
wait

for filename in /work/wjin/featurereduced4/sig_score/score1_2017*T2qq*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/llcontrol_score/score1_forT2qq_2017*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/zinvcontrol_score/score1_forT2qq_2017*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/bkg_score/score1_forT2qq_2017*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/datasigregion_score/score1_forT2qq_2017*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
wait



for filename in /work/wjin/featurereduced4/llcontrol_score/score1_forT2qq_2018*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/zinvcontrol_score/score1_forT2qq_2018*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/bkg_score/score1_forT2qq_2018*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
for filename in /work/wjin/featurereduced4/datasigregion_score/score1_forT2qq_2018*_binV10.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T2qq
done&
wait
