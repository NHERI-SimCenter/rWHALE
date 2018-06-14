#!/bin/bash          
#
# usage: wrapper dirName mainScript.tcl 
#   dirName: directory containing the mainScript
#   mainScript: the main input file to run
#
# written: fmk

# make sure we catch errors!
set -e

# need to ensure OpenSees and dakota can be called

platform='Darwin'

platform=$(uname)

if [ "x$PEGASUS_WF_UUID" != "x" ]; then
    echo "Pegasus job detected - Pegasus will set up the env"
elif [[ $platform == 'Darwin' ]]; then
    export DAKOTA_PATH=$HOME/dakota/bin
    export OPENSEES_PATH=$HOME/bin
    export PATH=$PATH:$OPENSEES_PATH:$DAKOTA_PATH    
    source $HOME/.profile
elif [[ $platform == 'Linux' ]]; then
    export DAKOTA_PATH=$HOME/dakota/dakota-6.5/bin
    export LD_LIBRARY_PATH=$HOME/dakota/dakota-6.5/lib
    export OPENSEES_PATH=$HOME/bin
    export PATH=$PATH:$OPENSEES_PATH:$DAKOTA_PATH
    source $HOME/.bashrc

else
    echo "PLATFORM NOT RECOGNIZED"
fi

#
# input parameters
#

bimName=$2
samName=$4
evtName=$6
edpName=$8
lossName=${10}
simName=${12}
driverFile=${14}

#dakotaName=$6
#outName=$7

scriptDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
numSamples=5


# parse json file, creating dakota input and other files
#  note: done in python

echo $scriptDIR/preprocessJSON.py $bimName $evtName $samName $edpName $lossName $simName $driverFile
python $scriptDIR/preprocessJSON.py $bimName $evtName $samName $edpName $lossName $simName $driverFile $scriptDIR

#
# create a dir templatedir to place all files needed by a dakota run
# & place all the needed files in here
#

rm -fr templatedir
rm -fr workdir.*

mkdir templatedir
chmod 'u+x' workflow_driver
#cp -r $scriptDIR/createSAM/data ./templatedir
cp workflow_driver ./templatedir
cp $scriptDIR/dpreproSimCenter ./templatedir
cp $bimName ./templatedir/bim.j
cp $evtName ./templatedir/evt.j
cp $samName ./templatedir/sam.j
cp $edpName ./templatedir/edp.j
cp $simName ./templatedir/sim.j

#
# run dakota
#
dakota -input dakota.in -output dakota.out -error dakota.err

chmod 'u+x' finishUP.sh
./finishUP.sh

# copy dakota.out up to word Kurtosis
#cp dakota.out dakota.tmp

#if [[ $platform == 'Darwin' ]]; then
#    sed -i '' '1,/Kurtosis/d' dakota.tmp
#else
#    sed -i '1,/Kurtosis/d' dakota.tmp
#fi

#cp dakota.out $dirNAME/
#cp dakota.tmp $dirNAME/
#cp dakotaTab.out $dirNAME/

exit
