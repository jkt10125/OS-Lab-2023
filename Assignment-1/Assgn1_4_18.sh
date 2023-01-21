#!/usr/bin/bash

IFS=$'\n'
>temp.txt
while read line
do
    [[ $line =~ .*$2.* ]] &&
    echo $line | sed 's/\([a-zA-Z]\)\([a-zA-Z]\)/\U\1\L\2/g' >>temp.txt ||
    echo $line >>temp.txt
done <$1
mv temp.txt input1.txt