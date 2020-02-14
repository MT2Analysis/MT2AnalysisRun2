#!/bin/bash
datatag="superMerged"
#for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2016*_bin.root
#do
#    echo "$filename" >> samples_2016Signals_scoreV02.dat
#done


#for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2016*_bin.root
#do
#    if [[ "$filename" == *$datatag* ]] 
#    then
#       echo "$filename" >> samples_2016llepdata_scoreV02.dat
#    else
#       echo "$filename" >> samples_2016llepMC_scoreV02.dat
#    fi
#done

#for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2016*_bin.root
#do
#    if [[ "$filename" == *$datatag* ]]
#    then
#       echo "$filename" >> samples_2016zlldata_scoreV02.dat
#    else
#       echo "$filename" >> samples_2016zllMC_scoreV02.dat
#    fi
#done

#for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2016*_bin.root
#do
#    echo "$filename" >> samples_2016MC_scoreV02.dat
#done

#for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2016*_bin.root
#do
#    echo "$filename" >> samples_2016data_scoreV02.dat
#done


for filename in /scratch/wjin/featurereduced4/sig_score/score1_forT1bbbb_2017*_bin.root
do
    echo "$filename" >> samples_2017Signals_scoreV02.dat
done


for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2017*_bin.root
do
    if [[ "$filename" == *$datatag* ]]
    then
       echo "$filename" >> samples_2017llepdata_scoreV02.dat
    else
       echo "$filename" >> samples_2017llepMC_scoreV02.dat
    fi
done

for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2017*_bin.root
do
    if [[ "$filename" == *$datatag* ]]
    then
       echo "$filename" >> samples_2017zlldata_scoreV02.dat
    else
       echo "$filename" >> samples_2017zllMC_scoreV02.dat
    fi
done

for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2017*_bin.root
do
    echo "$filename" >> samples_2017MC_scoreV02.dat
done

for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2017*_bin.root
do
    echo "$filename" >> samples_2017data_scoreV02.dat
done




for filename in /scratch/wjin/featurereduced4/llcontrol_score/score1_forT1bbbb_2018*_bin.root
do
    if [[ "$filename" == *$datatag* ]]
    then
       echo "$filename" >> samples_2018llepdata_scoreV02.dat
    else
       echo "$filename" >> samples_2018llepMC_scoreV02.dat
    fi
done

for filename in /scratch/wjin/featurereduced4/zinvcontrol_score/score1_forT1bbbb_2018*_bin.root
do
    if [[ "$filename" == *$datatag* ]]
    then
       echo "$filename" >> samples_2018zlldata_scoreV02.dat
    else
       echo "$filename" >> samples_2018zllMC_scoreV02.dat
    fi
done

for filename in /scratch/wjin/featurereduced4/bkg_score/score1_forT1bbbb_2018*_bin.root
do
    echo "$filename" >> samples_2018MC_scoreV02.dat
done

for filename in /scratch/wjin/featurereduced4/datasigregion_score/score1_forT1bbbb_2018*_bin.root
do
    echo "$filename" >> samples_2018data_scoreV02.dat
done
