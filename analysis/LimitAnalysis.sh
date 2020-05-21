#!/bin/bash
echo $#;
if [ $# -lt 7 ]; then
    echo "USAGE: ${0} CFG VER MODEL M Mup m mup";
    exit;
fi

CFG=$1
VER=$2
MODEL=$3
M=$4
Mup=$5
m=$6
mup=$7

DATE_START=`date +%s`

shape=true

#setup env variables -> change these to point towards your areas!
AN_JOBDIR=/work/wjin/CMSSW_8_0_12/src/CMSSW_8_0_12/src/myMT2Analysis/analysis/
LIM_JOBDIR=/work/wjin/CMSSW_8_1_0/src/HiggsAnalysis/CombinedLimit/MT2Scripts/

#setup the environment 
source $VO_CMS_SW_DIR/cmsset_default.sh
echo "Loading root via cmssw"
cd $AN_JOBDIR
eval `scramv1 runtime -sh`

# actually start the job
echo "Check presence of signal files"
if [ "$(ls -al $AN_JOBDIR/EventYields_$CFG/analyses_signals_merged.root | wc -l)" -eq "0" ]
then 
  echo "No signal files found, exiting"
  return
fi

echo "Working from input dir" $AN_JOBDIR

echo "Starting to create datacards..."
echo $MODEL $M $MUP $m $mup $VER
./createDatacards_combined $CFG moriond2019_41p9ifb_2017 moriond2019_59p9ifb_2018 $3 $4 $5 $6 $7 $2 
# the two additional cfg files are irrelevant, but some argument is needed

echo "Finished creating datacards, at least in principle, content of datacards directory:"
if [  "$shape" = true  ]
then
  ls $AN_JOBDIR/EventYields_$CFG/datacards_$MODEL/datacard_*$M*$m*.txt
else
  ls $AN_JOBDIR/EventYields_$CFG/datacards_${MODEL}_noshape/datacard_*$M*$m*.txt
fi

if [ "$shape" = true ]
then
  if [ "$(ls ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}/datacard_*${M}*${m}*.txt | wc -l)" -eq "0" ]
  then
    echo "No data-cards were created, not going to do anything exiting..."
    return 
  fi
else
  if [ "$(ls ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}_noshape/datacard_*${M}*${m}*.txt | wc -l)" -eq "0" ]
  then
    echo "No data-cards were created, not going to do anything exiting..."
    return
  fi
fi
# Now limit environment
echo "Continue with data-card combination"  
echo "Setting up combine environment"
cd $LIM_JOBDIR
eval `scramv1 runtime -sh`

# data-card combination
echo "Combining data-cards"
if [  "$shape" = true  ]
then
  combineCards.py -S ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}/datacard_*${M}*${m}*.txt > ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}/datacard_${M}_${m}_combined.txt
else
  combineCards.py -S ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}_noshape/datacard_*${M}*${m}*.txt > ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}_noshape/datacard_${M}_${m}_combined.txt
fi

if [  "$shape" = true  ]
then
  if [ "$(ls ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}/datacard_${M}_${m}_combined.txt | wc -l)" -eq "0" ]
  then
    echo "No combined data-card was created, exiting..."
    return
  fi
else
  if [ "$(ls ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}_noshape/datacard_${M}_${m}_combined.txt | wc -l)" -eq "0" ]
  then
    echo "No combined data-card was created, exiting..."
    return
  fi
fi

# limit calculation
echo "Going to run limit calculation"
if [  "$shape" = true ]
then
  combine -M AsymptoticLimits ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}/datacard_${M}_${m}_combined.txt --run blind -n ${MODEL}_${M}_${m} &> ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}/log_${MODEL}_${M}_${m}_combined.txt
else
  combine -M AsymptoticLimits ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}_noshape/datacard_${M}_${m}_combined.txt --run blind -n ${MODEL}_${M}_${m} &> ${AN_JOBDIR}/EventYields_${CFG}/datacards_${MODEL}_noshape/log_${MODEL}_${M}_${m}_combined.txt
fi
#
echo "This is it, folks!"
DATE_END=`date +%s`
RUNTIME=$((DATE_END-DATE_START))
echo "Wallclock running time: $RUNTIME s"


