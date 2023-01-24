#!/usr/bin/bash
mkdir -p $2
for letter in {a..z}
do
    awk -v letter=$letter 'tolower($0)~"^"letter' $1/*.txt | sort > $2/$letter.txt 
done
