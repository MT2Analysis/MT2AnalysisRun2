#!/bin/bash
for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT1qqqq_2016*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1qqqq_2016*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1qqqq_2016*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1qqqq_2016*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1qqqq_2016*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done


for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT1qqqq_2017*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1qqqq_2017*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1qqqq_2017*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1qqqq_2017*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1qqqq_2017*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done




for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1qqqq_2018*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1qqqq_2018*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1qqqq_2018*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1qqqq_2018*_binV4.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"} T1qqqq
done
