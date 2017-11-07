#!/bin/bash

if [ "$#" -ne 4 ]; then
    echo $#
    echo "wrong #paramateres, want: performSimulation fileBIM fileSAM fileEVENT fileEDP"
    exit
fi

# set filenames
filenameBIM="$1"
filenameSAM="$2"
filenameEVENT="$3"
filenameEDP="$4"

# perform simulation
./mainPreprocessor $1 $2 $3 $4 example.tcl
OpenSees example.tcl
./mainPostprocessor $1 $2 $3 $4
#rm example.tcl
