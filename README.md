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
```

```
cd CMSSW_8_0_12/src

cmsenv

git cms-init
```

Add a "link" to the remote repository where the code is

```git remote add ana-mt2 git@github.com:MT2Analysis/MT2AnalysisRun2.git```

To clone the master locally from the remote

```
git clone -o ana-mt2 git@github.com:MT2Analysis/MT2AnalysisRun2.git myMT2Analysis
```

To clone a given branch locally from the remote

```
git clone -o ana-mt2 -b <branch-name> git@github.com:MT2Analysis/MT2AnalysisRun2.git myMT2Analysis
```

After first installation, every time you log in

```cd CMSSW_8_0_12/src```

```cmsenv```

## Compilation

```make <name-of-file-you-want-to-compile>```

Note: everytime you change interface/mt2.h you have to:

```make clean```

and then recompile

## Run - time !

First you need to make the regions selections and compute the yields

```./regionEventYields <cfg-file-name> <data/mc/signal>```

```./llepControlRegion <cfg-file-name> <data/mc/signal>```

```./zllControlRegion <cfg-file-name> <data/mc/signal>```

Then you can make the Data/MC plots

```./drawLostLeptonControlRegionDataMC <cfg-file-name> <lumi/shape>```

```./drawZllControlRegion <cfg-file-name> <lumi/shape>```

Run the background estimation !

```./computeLostLepton <cfg-file-name> ```

```./computeZinvFromZll <cfg-file-name> ```

Note that the 1.13 factor present in https://github.com/MT2Analysis/MT2Analysis2015/blob/mg-data2017/analysis/computeZinvFromZll.cpp#L629 
is derived from plotZllGratio_2d.C   --> this script is also used to make the purity plots (?)

Create the template data card

```./createDatacards_general_zllZinvEst  <cfg-file-name> <model> <m1> <m2> <m11> <m22> <label>  # mass range```

```                                                          T2qq    200    200                    ```

To submit the card making on the batch, make sure that copySE is set to true in createDatacards_general_zllZinvEst 
divide the submission based on mass ranges, use this script for submission

```python launchCreateDatacards_2016.py```

Note: most of the times some jobs will fail, so you can as well submit twice and then check from one or the other which are missing 
