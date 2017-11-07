#!/bin/bash


startTime=`date +%s`

if [ $# -eq 0 ]
  then
    building=0;
  else
    building=$1
fi

make

cd createBIM
#rm *.json
if [ $building -eq 0 ]
  then
    ./createBIM 10 10
#    cp 10-BIM.json exampleBIM.json
  else
#    ./createBIM $1 $1
    cp $1-BIM.json exampleBIM.json
fi
cd ..

cd createEVENT
rm example*.json
cp ../createBIM/example*.json ./
./createEVENT exampleBIM.json exampleEVENT.json
cd ..


cd createSAM
rm example*.json
cp ../createEVENT/example*.json ./
./createSAM exampleBIM.json exampleEVENT.json exampleSAM.json
cd ..

cd createEDP
rm example*.json
cp ../createSAM/example*.json ./
./createEDP exampleBIM.json exampleSAM.json exampleEVENT.json exampleEDP.json
cd ..

cd performSIMULATION
rm example*.json
cp ../createEDP/example*.json ./
./getUNCERTAINTY exampleBIM.json exampleSAM.json exampleEVENT.json exampleSIMULATION.json
cd ..

cd performUQ
rm example*.json
cp ../performSIMULATION/example*.json ./
./preprocessDakota.sh exampleBIM.json exampleEVENT.json exampleSAM.json exampleEDP.json  exampleSIMULATION.json dakota exampleEDP-OUT.json
cd ..

cd createLOSS
rm example*.json
cp ../performUQ/exampleBIM.json ./
cp ../performUQ/exampleEDP.json ./
./createLOSS exampleBIM.json exampleEDP.json exampleLOSS.json

cat exampleLOSS.json

endTime=`date +%s`
runtime=$((end-start))
echo $runtime