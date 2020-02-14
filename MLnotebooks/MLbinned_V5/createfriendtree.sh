#!/bin/bash
for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2016*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done

for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2016*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2016*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done


for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2017*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2017*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2017*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2017*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done




for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2018*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2018*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2018*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2018*_binV5.npy
do
    if [ -f ${filename/".npy"/".root"} ];then
        continue
    fi
    python createfriendtree.py $filename ${filename/".npy"/".root"}
done
