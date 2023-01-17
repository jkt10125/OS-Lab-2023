#!/usr/bin/bash
mkdir -p $2
for x in {a..z}; do
    >"$2/$x.txt"
done

for file in "$1/*.txt"; do
    for p in `cat ${file[@]}`; do
        first=${p::1}
        echo $p >>$2/${first,,}.txt
        sort -o $2/${first,,}.txt{,}
    done
done