#!/bin/bash
echo $#;
if [ $# != 4 ];then
    echo "USAGE: ${0} inputfile outputfile treename signalname";
    exit;
fi
INPUT=$1
OUTPUT=$2
TREENAME=$3
SIGNAL=$4
python addMLscore_V05.py ${INPUT} ${OUTPUT} ${TREENAME} ${SIGNAL}
