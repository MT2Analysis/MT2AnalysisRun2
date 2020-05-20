#!/bin/bash
echo $#;
if [ $# != 1 ];then
    echo "USAGE: ${0} signalname";
    exit;
fi
SIGNAL=$1
python BDTtraining_1_binV3.py  ${SIGNAL}
