#!/bin/bash

if [ "$#" -lt 10 ]; then
    echo $#
    echo "wrong #paramateres, want: performSimulation fileBIM fileSAM fileEVENT fileEDP"
    exit
fi

# set filenames


filenameBIM="$2"
filenameSAM="$4"
filenameEVENT="$6"
filenameEDP="$8"
filenameSIM="$10"

scriptDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$#" -ne 9 ]; then
# perform simulation
$scriptDIR/mainPreprocessor $2 $4 $6 $8 example.tcl
OpenSees example.tcl
$scriptDIR/mainPostprocessor $2 $4 $6 $8
#rm example.tcl

else

# get random variables
$scriptDIR/getUncertainty $2 $4 $6 $10 

fi
