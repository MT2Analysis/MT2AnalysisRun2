# Script to launch the data card production of a given mass mass point of a model
# creates a tar of the datacards - assumes no directory structure inside the tar
# example:
# qsub -l h_vmem=6g -q short.q -o $PWD/test.out -e $PWD/test.err -N test createDatacards_batch_2016.sh dataETH_SnTMC_35p9ifb T2qq 400 405 200 205 FUCK
# NOTE: this script assumes that you have created a dir /pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards
# NOTE: make sure that the dir datacards_MODEL does not exist already and containes datacards in your local workdir
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
cd /mnt/t3nfs01/data01/shome/mschoene/80X/src/
echo $PWD
eval `scramv1 runtime -sh`

JOBDIR=/scratch/$USER/datacards/datacardCreation_$JOB_ID
INDIR=/shome/mratti/mt2_workarea/CMSSW_8_0_12/src/myMT2Analysis/analysis/
OUTPUTDIR=/pnfs/psi.ch/cms/trivcat/store/user/$USER/datacards/EventYields_$CFG/

echo "Copying all needed stuff..."

mkdir -p $JOBDIR/analysis/ # makes parents directories if needed

# copy all necessary stuff in the jobdir
# signal scans
cp -r /mnt/t3nfs01/data01/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/signalScansFromDominick $JOBDIR/analysis/
# bkg and data
cp -r $INDIR/EventYields_$CFG/ $JOBDIR/analysis/
#cp -r /mnt/t3nfs01/data01/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/EventYields_data_2016_SnT_36p8_FixedWJets/ $JOBDIR/analysis/
cp -r $INDIR/cfgs $JOBDIR/analysis/
cp -r $INDIR/../samples $JOBDIR

cp $INDIR/createDatacards_general_zllZinvEst  $JOBDIR/analysis/
#cp /mnt/t3nfs01/data01/shome/mschoene/80X/src/myMT2Analysis/analysis/createDatacards_general $JOBDIR/analysis/

cd $JOBDIR/analysis/
echo $PWD

echo "Starting to create datacards..."
# ./createDatacards_general_zllZinvEst $CFG $MODEL $M1 $M2 $M12 $M22 $LABEL
./createDatacards_general_zllZinvEst $1 $2 $3 $4 $5 $6 $7
echo "Finished creating datacards , at least in principle"
#mkdir /mnt/t3nfs01/data01/shome/mschoene/8_0_12_analysisPlayArea/src/mschoene_newBinning/analysis/testCopy/

ls $JOBDIR/analysis/EventYields_$CFG/datacards_$MODEL
#ls $JOBDIR/analysis/
#ls $JOBDIR/

#if no datacards produced, don't copy anything (to be tested)
echo "Before copying command"
cd $JOBDIR/analysis/EventYields_$CFG/ #datacards_$MODEL
ls -d $JOBDIR/analysis/EventYields_$CFG/datacards_$MODEL
echo "before tar"
echo "name of arcive" tared_${M1}_${M11}.tar.gz
tar -czvf tared_${M1}_${M11}.tar.gz datacards_$MODEL #datacard*txt
echo "after tar"
ls -al
echo ""
du -sh
xrdfs t3dcachedb03.psi.ch mkdir $OUTPUTDIR
echo "creating output dir" $OUTPUTDIR/datacards_${MODEL}_${LABEL}/
xrdfs t3dcachedb03.psi.ch mkdir $OUTPUTDIR/datacards_${MODEL}_${LABEL}/
echo "now copying tar to output dir"
xrdcp -v tared_${M1}_${M11}.tar.gz root://t3dcachedb.psi.ch:1094/$OUTPUTDIR/datacards_${MODEL}_${LABEL}/.
echo "After copying command"

cd /scratch/$USER/
#rm -rf $JOBDIR
