#!/bin/bash
for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2017*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2017*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2017*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2017*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done




for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2018*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2018*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2018*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2018*.npy
do
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
