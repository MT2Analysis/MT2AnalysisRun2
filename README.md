# MT2AnalysisRun2
Package to perform the MT2 analysis from mt2 baby-trees

 This README is work-in-progress and contains instructions on how to run
 the basic blocks of the package, excluding the QCD estimates.

 For inspiration about what was done in the past, consult
 https://github.com/MT2Analysis/MT2Analysis2015/tree/MT2Analysis2015\_RandD

## Setup and Installation
MT2Analysis2015 must work within the cmsenv, which is set through a valid release of CMSSW

```
cmsrel CMSSW_8_0_12 # later versions are not tested
cd CMSSW_8_0_12/src
cmsenv
git cms-init
```

Add a "link" to the remote repository where the code is

```
git remote add ana-mt2 git@github.com:MT2Analysis/MT2AnalysisRun2.git
```

To clone the master locally from the remote

```
git clone -o ana-mt2 git@github.com:MT2Analysis/MT2AnalysisRun2.git myMT2Analysis
```

To clone a given branch locally from the remote

```
git clone -o ana-mt2 -b <branch-name> git@github.com:MT2Analysis/MT2AnalysisRun2.git myMT2Analysis
```

After first installation, every time you log in

```
cd CMSSW_8_0_12/src
cmsenv
```
However, for the machine learning training step, the cmsenv should not be executed (log out first).
Instead you need to prepare a conda environment with python and libraries tensorflow, numpy, pandas, matplotlib, seaborn,
sklearn, statistics, math, random installed. Or you can export the enviroment

```
source /t3home/wjin/.bashrc
```
Then to do the training and score evaluation with gpu
```
conda activate tf-gpu
```
Or to transfer .root files to .npy files, and the plot the limits with
```
conda activate tensorflow_cpu
```
## Compilation

```
make <name-of-file-you-want-to-compile>
```

Note: everytime you change interface/mt2.h or interface/MT2Analysis.h you have to:

```
make clean
```

and then recompile

## Run - time !
###Preselection
Under the cmsenv, you can prepare the files for machine learning. Remember to check the file paths and names of the nominal ntuples, prepare the directory for the output, and change the path of the output files. This step skims the signal, background, and control region data and MC. 
```
cd MLnotebooks/preselection
root preselection\_forML\_reducefeature\_1.cpp
root preselection\_forML\_reducefeature\_llcontrol.cpp
root preselection\_forML\_reducefeature\_zinvcontrol.cpp
bash dataskim\_control\_signal.sh
```
Then log out, reenter the system with conda enviroment tensorflow\_cpu, to transfer the skimmed .root files into .npy files. Also remember to change the file paths. Under the same directory 
```
python preselection\_root\_to\_npy.py
```
### Machine learning training
Then you can do the machine learning training. It requires gpu usage
```
conda activate tf-gpu
voms-proxy-init --voms cms --valid 196:00
```
For the training with neural network
```
cd MLnotebooks/MLbinned\_V10\_2
bash batch\_train.sh
```
This submits the training jobs to the slurm system. The jobs take several hours.
For the training with BDT
```
cd MLnotebooks/MLbinned\_BDTV3\_2
bash batch\_train.sh
```
The jobs takes about one day.
for the training with neural network training in inclusive phase space
```
cd MLnotebooks/MLbinned\_incluV3
bash batch\_train.sh
```
This takes about two hours. Sometimes the system can kill the jobs. You need to try again.
After the training, the models are saved in the directores`MLnotebooks/MLbinned_V10_2`,`MLnotebooks/MLbinned_BDTV3_2`or `MLnotebooks/MLbinned_incluV3`. The significance changes in the cut position scan are saved in the same directories, with names `significance_T1bbbb_output.npy`,`significance_T1qqqq_output.npy`,`significance_T2bb_output.npy` or `significance_T2qq_output.npy`. You can get the cut positions corresponding to optimized significance
```
python readsign.py T1bbbb
python readsign.py T1qqqq
python readsign.py T2bb
python readsign.py T2qq
```
This produces files that will be used in the analysis. You need to move them into the configuration folder
```
mv cutinf\*.txt ../../analysis/cfgs/.
```

The files such as `cutinfBDTV3_2_1_T1bbbb.txt` correspond to the cuts with best Asimov significance, while`cutinfBDTV3_2_5_T1bbbb.txt` and `cutinfBDTV3_2_10_T1bbbb.txt` correspond to cuts with best significance but require at least 5 and 10 events in control regions respectively.

Then you can evaluated the skimmed ntuples with the models. Before running you need to change the file paths(it reads the files in the path of `directory` and produce score files in the path of `targetdir`)

```
python submitbatch\_evaluate.py
```
Some jobs can fail, you may need to submit them again. Then you can transfer the .npy score files into .root files (you also need to change the paths in the scripts)
```
conda activate tensorflow\_cpu
bash createfriendtree\_T1bbbb.sh
bash createfriendtree\_T1qqqq.sh
bash createfriendtree\_T2bb.sh
bash createfriendtree\_T2qq.sh
```
You may also need to change the paths of the score files in the directory `samples`. In the analysis the program matches the names of score files in the `\*.dat` files to those of the ntuples. A script `createscoresamples.py` in the main directory may help you create the `\*.dat` files conveniently if you want to create a new optimization version.

### Data and backgrounds

Preliminary comment:
Check that you use the correct binning by setting the correct booleans to true in:
- src/MT2Region.cc
- analysis/computeZinvFromZll.cc

First you need to make the regions selections and compute the yields. Examples of <cfg-file-name> for ML optimization: `config_MLoptimize_<YEAR>_For<SIGNAL>_<VERSION>_<SAMPLENUMBER>`, <YEAR>=2016, 2017, 2018, <SIGNAL>=T1bbbb, T1qqqq, T2bb, T2qq, <VERSION>=BDTV3_2, binV10_2, incluV3, <SAMPLENUMBER>=1, 5, 10.

```
./regionEventYields <cfg-file-name> <data/mc/signal>
./llepControlRegion <cfg-file-name> <data/mc/signal>
./zllControlRegion <cfg-file-name> <data/mc/signal>
```

Then you can make the Data/MC plots

```
./drawLostLeptonControlRegionDataMC <cfg-file-name> <lumi/shape>
./drawZllControlRegion <cfg-file-name> <lumi/shape>
```

Run the background estimation !

```
./computeLostLepton <cfg-file-name>
./computeZinvFromZll <cfg-file-name>
```

To compute the purity of the Zll region current, 
edit drawRSFOF.C with the year chosen, edit the input directory.

You must manually  create the output dir of the plots 
ratio2016 , ratio2017, ratio2018, ratioCombined

Compile as usual and run:
```
./drawRSFOF
```

Create the template data-cards. For the general syntax of data-card creation, see here: https://cms-hcomb.gitbooks.io/combine/content/
Edit the options at the beginning of the script
```
./createDatacards\_combined <cfg-file-name1> <cfg-file-name2> <cfg-file-name3> <model-name> <m1> <m2> <M1> <M2> <label-for-SE>
```
Run with m1=m2 and M1=M2 to skip signal part of data-card creation.

Data/Bkg plotting:

In all topological regions
```
./compareYield\_all <cfg-file-name> <moriond2019/moriond2017>
```
'moriond2019' is set by default. 'moriond2017' produces the plots with the 'zurich2016' regions set.

In each HT region
```
./compareYield\_bins\_all <cfg-file-name> <moriond2019/moriond2017>
```
'moriond2019' is set by default. 'moriond2017' produces the plots with the 'zurich2016' regions set.

For post-fit plots, please consult https://github.com/MT2Analysis/MT2Analysis2015/tree/MT2Analysis2015_RandD

#### Automatic launch

To run the analysis automatically, use launch\_analysis\_ML\_<SIGNAL>\_<VERSION>\_<SAMPLENUMBER>.sh

Open the file, and set the parameters in the user's decision board according to your wish.

Once done, do

```
source launch\_analysis\_ML\_<SIGNAL>\_<VERSION>\_<SAMPLENUMBER>.sh
```
This launches the event selections for signal, background, then the background estimation and datacard creation for the background.

### Signals

Merge the output signals in one root file to be stored in the '16 cfg file':
```
hadd EventYields\_<cfg-file-name16>/analyses\_signals\_merged.root EventYields\_<cfg-file-name16>/analyses.root EventYields\_<cfg-file-name17>/analyses.root EventYields\_<cfg-file-name18>/analyses.root
```


Edit desired options in ```createDatacards_combined.cpp``` 
Run the data-card creation for the signals, e.g.:
```
./createDatacards\_combined config\_MLoptimize\_2016\_ForT2qq\_BDTV3\_2\_1 config\_MLoptimize\_2017\_ForT2qq\_BDTV3\_2\_1 config\_MLoptimize\_2018\_ForT2qq\_BDTV3\_2\_1  T2qq 300 305 200 205 pippo  # for single mass point
./createDatacards\_combined config\_MLoptimize\_2016\_ForT2qq\_BDTV3\_2\_1 config\_MLoptimize\_2017\_ForT2qq\_BDTV3\_2\_1 config\_MLoptimize\_2018\_ForT2qq\_BDTV3\_2\_1 T2qq 1000 1105 200 255      # for mass scan
```

### Limit tests
For limit calculation you need software Combine. To set up the necessary framework, see [here](https://twiki.cern.ch/twiki/bin/viewauth/SusyMECCA/SusyMT2cernETHLegacy#Setting_up_Combine).
From inside the ```HiggsAnalysis/CombinedLimit/MT2Scripts``` directory:

Combine the data cards

```
combineCards.py -S <input-card-1> <input-card-2> ...  >  <combined-card>
```

Submit a limit (for a single point, as a check)

```
combine -M AsymptoticLimits <combined-card> -n --run blind ${MODEL}\_${M1}\_${M2} >& log\_${MODEL}\_${M1}\_${M2}\_combined.txt
```
#### Automatic launch the datacards creation and limit calculation for one point
First change the file paths in LimitAnalysis.sh, then (<VER> is a customized tag)
```
bash LimitAnalysis.sh <16-config> <VER> <SIGNAL> <parent-mass-down> <parent-mass-up> <daugther-mass-down> <daugther-mass-up>
```

### Limits, full production and plotting 
#### data-card creation
From ```MT2AnalysisRun2/analysis``` submit data-card creation to the batch. 
Created data-cards will be saved to the storage element in the form of a tar file. 
1. make sure that ```/pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards``` exists
2. make sure that the data-card templates were already created
3. edit ```doOnlySig=true``` in ```createDatacards_combined.cpp``` and recompile
4. make sure that ```stepSize``` and ranges are set to desired values in ```launchCreateDatacards.py```
5. edit ```INDIR``` in ```createDatacards_batch_fromHome.sh```
6. if you do not have a valid proxy already, ```voms-proxy-init --voms cms --valid 196:00```
7. finally submit data card creation to the batch
```
python launchCreateDatacards.py <model-name> <label> 
```
Please don't forget the label because it is needed for the next steps.

Note that it is possible that a significant fraction of jobs will fail.
Therefore it may be needed to relaunch the data-card production with a different version and then merge 
together two sets of limits obtained with two different data-card productions.

#### data-card combination
From ```HiggsAnalysis/CombinedLimit/MT2Scripts``` directory sumbit the data-card combination to the batch.
Remember to modify the path in ```combineCards_batch_scan.sh``` to the location where the needed CMSSW release is.
The combined cards will be copied to the same storage element location as the tar files
```
python combineCards_scan.py <path> <model>
```
Note that often a non-negligible fraction of these jobs fails. 
Check for signficant fraction of data-cards with size 0.
You can relaunch the combination, use exactly the same command - it will remove combined data-cards of size 0 and relaunch only the jobs that failed previously.

#### limit calculation
From the same directory, sumbit limit calculation to the batch. 
Remember to modify the path in ```submitLimits_batch_scan.sh``` to the location where the needed CMSSW release is.
The limits will be copied in a ```limits``` subdir from the original SE path
```
python submitLimits_scan.py <path> <model>
```
#### plotting
Once you have the limits for each masses, copy them in your scratch area:
```
sh copyLimits.sh <model> <label> <path-minus-lastDir>
```

Then, create a .txt file with your full limits by running:
```
sh readAsymptoticLimits_Scan.sh <model> <label> 
```

Finally run interpolation and smoothing and create root file:
```
python drawSMSlimit.py <txt-file-you-just-created> 
```

### Contour plots
Setup the environment as described under this [link](https://twiki.cern.ch/twiki/bin/viewauth/SusyMECCA/SusyMT2cernETH#Preparing_PlotsSMS_code).

Once in the correct folder, create a config file, in the same way as [here](https://github.com/MT2Analysis/PlotsSMS/blob/master/config/SUS16015/T1tttt_2016_7p7ifb.cfg). Change the name of the directory and move the root file created in the previous step accordingly.

You are ready to create the plot:

```
python python/makeSMSplots.py config/<config-file> <name>  
```


### Significance
You might want to compute the significance as well. For that, once all the createDatacard jobs are finished, follow the same procedure as with limits, with the following commands:
```
python submitSignificance_scan.py <path> <model>
```

```
sh copySignificance.sh <model> <label> <path> 
```

```
sh readPLSignificance_Scan.sh <model> <label>  
```

```
python drawSMSsignificance.py <txt-file-just-created>
```


For plotting the contours in SUSY CMS style see  [this link](https://github.com/MT2Analysis/PlotsSMS/blob/master/README)


### Ranking of topological regions
You do this procedure to understand what topological regions are driving the sensitivity for a particular mass-mass point.

Make sure you have done once:

```
mkdir /scratch/`whoami`/datacards
mkdir /scratch/`whoami`/ranking
```

Run locally the data-cards creation for the mass-mass point of interest and copy them. Example
```
./createDatacards_combined moriond2019_35p9ifb moriond2019_41p9ifb_2017 moriond2019_59p9ifb_2018 T2qq 1200 1205 850 855
mkdir /scratch/`whoami`/datacards/datacards_T2qq_1200_850/
cp EventYields_moriond2019_35p9ifb/datacards_T2qq/*T2qq_1200_850*txt /scratch/`whoami`/datacards/datacards_T2qq_1200_850
```

Run the limits for each TR separately (this step usually requires 10-15 minutes)
```
source combineAsymptotic_forRanking.sh T2qq_1200_850
```

Extract the informatiion from the logs of the limits into a .txt file
```
source readAsymptoticLimits_forRanking.sh T2qq_1200_850
```

Rank the limits by the strongest to the weakest:
```
python rankRegions.py T2qq_1200_850
```

Create sig + bkg table yields
```
./printLatexBGTable_Run2 EventYields_moriond2019_35p9ifb
```
NOTE: following step not working yet
Edit the name of the latex table created at the previous step in ```makeRankingTables.py```
```
python makeRankingTables.py T2qq_1200_850 137
```

