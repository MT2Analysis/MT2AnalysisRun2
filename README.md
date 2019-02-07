# MT2AnalysisRun2
Package to perform the MT2 analysis from mt2 baby-trees

 This README is work-in-progress and contains instructions on how to run
 the basic blocks of the package, excluding the QCD estimates.

 For inspiration about what was done in the past, consult
 https://github.com/MT2Analysis/MT2Analysis2015/tree/MT2Analysis2015_RandD

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

### Data and backgrounds

Preliminary comment:
Check that you use the correct binning by setting the correct booleans to true in:
- src/MT2Region.cc
- analysis/computeZinvFromZll.cc

First you need to make the regions selections and compute the yields

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

Create the template data card. For the general syntax of data card creation, see here: https://cms-hcomb.gitbooks.io/combine/content/
Edit the script to allow QCD estimate or not, currently supported only for Moriond19 
```
./createDatacards <cfg-file-name> <model-name> <m1> <m2> <M1> <M2> <label-for-SE>
```

Data/Bkg plotting:

In all topological regions
```
./compareYield_all <cfg-file-name> <moriond2019/moriond2017>
```
'moriond2019' is set by default. 'moriond2017' produces the plots with the 'zurich2016' regions set.

In each HT region
```
./compareYield_bins_all <cfg-file-name> <moriond2019/moriond2017>
```
'moriond2019' is set by default. 'moriond2017' produces the plots with the 'zurich2016' regions set.

For post-fit plots, please consult https://github.com/MT2Analysis/MT2Analysis2015/tree/MT2Analysis2015_RandD

### Signals

Run regionEventYields on the signal, for the moment just use those already run before

```
/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/signalScansFromDominick/*root
```

Run the data card creation also for the signal, e.g.
```
./createDatacards_general_zllZinvEst dataETH_SnTMC_35p9ifb T2qq 300 305 200 205 pippo  # -> mass point or mass range
```
Be sure that the you created some data cards for the signal, not only the template data card

### Limit tests
For limit calculation you need software Combine. To set up the necessary framework, see [here](https://twiki.cern.ch/twiki/bin/viewauth/SusyMECCA/SusyMT2cernETHLegacy#Setting_up_Combine).

Combine the data cards

```
combineCards.py -S <input-card-1> <input-card-2> ...  >  <combined-card>
```

Submit a limit (for a single point, as a check)

```
combine -M AsymptoticLimits <combined-card> -n ${MODEL}_${M1}_${M2} >& log_${MODEL}_${M1}_${M2}_combined.txt
```

### Limits, full production and plotting 
Submit data card creation to the batch (copySE=true, so that files are stored in your storage element)
```
python launchCreateDatacards_2016.py <model-name> <label> 
```
Please don't forget the label because it is needed for the next steps.

TODO: split more wisely instead of one job per point, to avoid overloading the I/O of the tier3.

From now on the scripts are found in HiggsAnalysis-CombinedLimit/MT2Scripts/ 

Sumbit data card combination to the batch
```
python combineCards_scan.py <path> <model>
```
Note that the path is the one to your Storage Element.

Submit limit calculation to the batch
```
python submitLimits_scan.py <path> <model>
```
Once you have the limits for each masses, copy them in your scratch area:
```
sh copyLimits.sh <model> <label> <path>
```

Then, create a txt file with your full limits:
```
sh readAsymptoticLimits_Scan.sh <model> <label> 
```

Note that the .txt file will be created where you launched the command.

Finally run interpolation and smoothing and create root file:
```
python drawSMSLimit.py <txt-file-you-just-created> 
```

###Contour plots
Setup the environment as described under this [link](https://twiki.cern.ch/twiki/bin/viewauth/SusyMECCA/SusyMT2cernETH#Preparing_PlotsSMS_code).

Once in the correct folder, create a config file, in the same way as [here](https://github.com/MT2Analysis/PlotsSMS/blob/master/config/SUS16015/T1tttt_2016_7p7ifb.cfg). Change the name of the directory and move the root file created in the previous step accordingly.

You are ready to create the plot:

```
python python/makeSMSplots.py config/<config-file> <name>  
```


### Significance
You might want to compute the significance as well. For that, once all the createDatacard jobs are finished, follow the same procedure as with limits, with the following commands:
```
python submitSignificance_scan.py <pathSE> <pathSE> 
```
Note: the two path can be the same (to the storage element)

```
sh copySignificance.sh <model> <label> <pathSE> 
```

```
sh readPLSignificance_Scan.sh <model> <label>  
```

```
python drawSMSsignificance.py <txt-file-just-created>
```



For more info, Please follow [this link](https://github.com/MT2Analysis/HiggsAnalysis-CombinedLimit/blob/BASE_MT2Combine/MT2Scripts/HOWTORUN_limits_and_significance.txt)

For plotting in SUSY CMS style see  [this link](https://github.com/MT2Analysis/PlotsSMS/blob/master/README)
