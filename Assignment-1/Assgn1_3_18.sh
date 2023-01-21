#!/usr/bin/bash

atr=`echo ${@:3}`

dir=`basename -s .json -a json/*.json`

`mkdir -p $2`

for file in $dir
do
    `echo ${@:3} | tr " " "," >$2/file.csv`
done

