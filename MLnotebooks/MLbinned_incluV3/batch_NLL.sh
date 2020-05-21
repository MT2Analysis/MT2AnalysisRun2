#!/bin/bash
echo $#;
if [ $# != 1 ];then
    echo "USAGE: ${0} signalname";
    exit;
fi
SIGNAL=$1
python NLLtraining_1_incluV3.py ${SIGNAL}
