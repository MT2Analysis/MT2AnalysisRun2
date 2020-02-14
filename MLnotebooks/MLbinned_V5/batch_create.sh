#!/bin/bash
echo $#;
if [ $# != 2 ];then
    echo "USAGE: ${0} inputfile signalname";
    exit;
fi
filename=$1
signalname=$2
python createfriendtree.py $filename ${filename/".npy"/".root"} $signalname
