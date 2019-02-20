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

Create the template data-cards. For the general syntax of data-card creation, see here: https://cms-hcomb.gitbooks.io/combine/content/
Edit the options at the beginning of the script
```
./createDatacards_combined <cfg-file-name1> <cfg-file-name2> <cfg-file-name3> <model-name> <m1> <m2> <M1> <M2> <label-for-SE>
```
Run with m1=m2 and M1=M2 to skip signal part of data-card creation.

Data/Bkg plotting:

In all topological regions
```
./compareYield_all <cfg-file-name>
```

In each HT region
```
./compareYield_bins_all <cfg-file-name>
```

For post-fit plots, please consult https://github.com/MT2Analysis/MT2Analysis2015/tree/MT2Analysis2015_RandD

### Signals

Run regionEventYields on the desired signal scan, supported scans are (only for 2016):
T1qqqq, T1bbbb, T1tttt, T2qq, T2bb, T2tt.
Signal contamination removal not currently supported - so T1tttt, T2tt yields are not reliable

```
./regionEventYields <cfg-file-name16> signal <model>
```

Edit desired options in ```createDatacards_combined.cpp``` 
Run the data-card creation for the signals, e.g.:
```
./createDatacards_combined moriond2019_35p9ifb moriond2019_41p9ifb_2017 moriond2019_59p9ifb_2018 T2qq 300 305 200 205 pippo  # for single mass point
./createDatacards_combined moriond2019_35p9ifb moriond2019_41p9ifb_2017 moriond2019_59p9ifb_2018 T2qq 1000 1105 200 255      # for mass scan
```

### Limit tests
For limit calculation you need software Combine. From inside the ```HiggsAnalysis/CombinedLimit/MT2Scripts``` directory:

Combine the data cards

```
combineCards.py -S <input-card-1> <input-card-2> ...  >  <combined-card>
```

Run a limit

```
combine -M Asymptotic <combined-card> -n ${MODEL}_${M1}_${M2} >& log_${MODEL}_${M1}_${M2}_combined.txt
```

### Limits, full production and plotting 
#### data-card creation
From ```MT2AnalysisRun2/analysis``` submit data-card creation to the batch. 
Created data-cards will be saved to the storage element. 
1. make sure that ```/pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards``` exists
2. make sure that the data-card templates were already created
3. edit ```doOnlySig=true``` in ```createDatacards_combined.cpp``` and recompile
4. make sure that stepSize and ranges are set to desired values in ```launchCreateDatacards.py```
5. edit ```INDIR``` in createDatacards_batch.sh
```
python launchCreateDatacards.py <model-name> <label> 
```
TODO: split more wisely instead of one job per point, to avoid overloading the I/O of the tier3.

#### data-card combination
From ```HiggsAnalysis/CombinedLimit/MT2Scripts``` directory sumbit the data-card combination to the batch
```
python combineCards_scan.py <path> <model>
```
#### limit calculation
From the same directory, Sumbit limit calculation to the batch
```
python submitLimits_scan.py <path> <model>
```
#### plotting
For the next steps (interpolation and contour extraction), please follow from step 4 of [this link](https://github.com/MT2Analysis/HiggsAnalysis-CombinedLimit/blob/BASE_MT2Combine/MT2Scripts/HOWTORUN_limits_and_significance.txt)

For plotting the contours in SUSY CMS style see  [this link](https://github.com/MT2Analysis/PlotsSMS/blob/master/README)
