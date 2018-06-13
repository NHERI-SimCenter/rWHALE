#!/bin/bash
FROM=$1
TO=$2
OutputFilename=$3

rm $3
while [ $FROM -le $TO ]
do
    cat $FROM.out >> $OutputFilename
    (( FROM++ ))  
done