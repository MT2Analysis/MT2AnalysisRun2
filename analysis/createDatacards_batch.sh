# Script to launch the data card production of a given mass mass point of a model
# creates a tar of the datacards - assumes no directory structure inside the tar
# example:
#qsub -q short.q  -l h_vmem=6g -o `pwd`/log_900_1000_0_100.out -e `pwd`/log_900_1000_0_100.err -N creatingDatacards_T1qqqq_900_1000_0_100 createDatacards_batch.sh moriond2019_35p9ifb T1qqqq 900 1000 0 100 V0
# NOTE: this script assumes that you have created a dir /pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards
#!/bin/bash
echo $#;
if [ $# -lt 6 ]; then
    echo "USAGE: ${0} MODEL CFG M1 M2 M11 M22 [LABEL]";
    exit;
fi

CFG=$1
MODEL=$2
M1=$3
M2=$4
M11=$5
M22=$6

if [ $# -ge 7 ]; then
    LABEL=$7;
else
    LABEL="";
fi

echo $CFG $1
echo $MODEL $2
echo $M1 $3
echo $M2 $4
echo $M11 $5
echo $M22 $6


source $VO_CMS_SW_DIR/cmsset_default.sh
#source /mnt/t3nfs01/data01/swshare/glite/external/etc/profile.d/grid-env.sh
export SCRAM_ARCH=slc6_amd64_gcc491
export LD_LIBRARY_PATH=/mnt/t3nfs01/data01/swshare/glite/d-cache/dcap/lib/:$LD_LIBRARY_PATH

echo "Loading 80X"
cd /shome/mratti/mt2_workarea/CMSSW_8_0_12/src/
echo $PWD
eval `scramv1 runtime -sh`

JOBDIR=/scratch/$USER/datacards/datacardCreation_$JOB_ID
INDIR=/shome/mratti/mt2_workarea/CMSSW_8_0_12/src/master/analysis/
OUTPUTDIR=/pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards/EventYields_$CFG/


echo "Content of input dir"
ls -al $INDIR
ls -al $INDIR/EventYields_$CFG/analyses.root

echo "Copying all needed stuff..."

mkdir -p $JOBDIR/analysis/ # makes parents directories if needed
mkdir $JOBDIR/analysis/EventYields_$CFG
# copy only necessary stuff in the jobdir
cp    $INDIR/EventYields_$CFG/analyses.root                $JOBDIR/analysis/EventYields_$CFG/.
#cp    $INDIR/EventYields_$CFG/llepEstimateCombined.root    $JOBDIR/analysis/EventYields_$CFG/.
#cp    $INDIR/EventYields_$CFG/zinvFromZllCombined.root     $JOBDIR/analysis/EventYields_$CFG/.
cp -r $INDIR/EventYields_$CFG/datacard_templates_combined/ $JOBDIR/analysis/EventYields_$CFG/.
cp -r $INDIR/cfgs $JOBDIR/analysis/
cp -r $INDIR/../samples $JOBDIR

cp    $INDIR/createDatacards_combined  $JOBDIR/analysis/

cd $JOBDIR/analysis/
echo $PWD

echo "Content of job dir"
ls -al $JOBDIR/
ls -al $JOBDIR/analysis/
ls -al $JOBDIR/analysis/EventYields_$CFG

echo "Starting to create datacards..."

./createDatacards_combined $1 moriond2019_41p9ifb_2017 moriond2019_59p9ifb_2018 $2 $3 $4 $5 $6 $7

echo "Finished creating datacards , at least in principle, content of local datacards directory:"

ls $JOBDIR/analysis/EventYields_$CFG/datacards_$MODEL
#ls $JOBDIR/analysis/
#ls $JOBDIR/

#if no datacards produced, don't copy anything (to be tested)
echo "About to issue copy"
cd $JOBDIR/analysis/EventYields_$CFG/ #datacards_$MODEL
ls -d $JOBDIR/analysis/EventYields_$CFG/datacards_$MODEL
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
xrdcp -v tared_${M1}_${M11}.tar.gz root://t3dcachedb.psi.ch:1094/$OUTPUTDIR/datacards_${MODEL}_${LABEL}/.
echo "After copying command"

cd /scratch/$USER/
#rm -rf $JOBDIR
