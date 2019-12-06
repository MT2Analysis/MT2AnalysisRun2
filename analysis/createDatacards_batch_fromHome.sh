#!/bin/bash

# Script to submit the data card production of a given mass mass point of a model to slurm
# output is a tar of the datacards in /pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards 
# NOTE: assumes that you have created a dir /pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards

echo $#;
if [ $# -lt 7 ]; then
    echo "USAGE: ${0} MODEL CFG M1 M2 M11 M22 LABEL";
    exit;
fi


# script arguments
CFG=$1
MODEL=$2
M1=$3
M2=$4
M11=$5
M22=$6
LABEL=$7

echo $CFG $1
echo $MODEL $2
echo $M1 $3
echo $M2 $4
echo $M11 $5
echo $M22 $6
echo $LABEL $7


# setup the environment 
source $VO_CMS_SW_DIR/cmsset_default.sh
echo "Loading root via cmssw"
cd /work/mratti/mt2_workarea/CMSSW_8_0_12/src/
echo $PWD
eval `scramv1 runtime -sh`

# setup variables for job
JOBDIR=/scratch/$USER/datacards/datacardCreation_$SLURM_JOB_ID
INDIR=/work/mratti/mt2_workarea/CMSSW_8_0_12/src/master/analysis/
OUTPUTDIR=/pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards/EventYields_$CFG/

# actually start the job
echo "Content of input dir"
ls -al $INDIR
ls -al $INDIR/EventYields_$CFG/analyses_signals_merged.root

cd $INDIR
echo "Working from input dir" $PWD

echo "Creating job directory where to put output"
mkdir -p $JOBDIR/datacards_$MODEL
ls -al $JOBDIR/datacards_$MODEL

echo "Starting to create datacards..."
./createDatacards_combined $1 moriond2019_41p9ifb_2017 moriond2019_59p9ifb_2018 $2 $3 $4 $5 $6 $LABEL $JOBDIR/datacards_$MODEL
# the two additional cfg files are irrelevant, but some argument is needed

echo "Finished creating datacards, at least in principle, content of datacards directory:"
ls $JOBDIR/datacards_$MODEL

cd $JOBDIR

if [ "$(ls datacards_$MODEL | wc -l)" -eq "0" ] 
then 
  echo "No data-cards were created, not going to do anything"
else
  echo "About to tar the data-cards"
  echo "Name of archive" tared_${M1}_${M11}.tar.gz
  tar -czvf tared_${M1}_${M11}.tar.gz datacards_$MODEL #datacard*txt
  echo "After tar"
  ls -al
  echo ""
  du -sh

  xrdfs t3dcachedb03.psi.ch mkdir $OUTPUTDIR
  echo "Creating output dir" $OUTPUTDIR/datacards_${MODEL}_${LABEL}/
  xrdfs t3dcachedb03.psi.ch mkdir $OUTPUTDIR/datacards_${MODEL}_${LABEL}/
  echo "Now copying tar to output dir"
  xrdcp -v -f tared_${M1}_${M11}.tar.gz root://t3dcachedb.psi.ch:1094/$OUTPUTDIR/datacards_${MODEL}_${LABEL}/.
  echo "After copying command"
fi


cd /scratch/$USER/
rm -rf $JOBDIR
