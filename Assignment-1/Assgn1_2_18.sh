#!/usr/bin/bash
while read username
do
    output="NO"
    if grep -Pq "^[a-zA-Z](?=.*[0-9])[a-zA-Z0-9]{4,19}$" <<< "$username" && ! grep -iqFf fruits.txt <<< "$username"
    then
    output="YES"
    # while read fruits
    # do
    #     if grep -i -q "$fruits"<<<"$username" 
    #     then
    #         output="NO"
    #     fi
    # done<fruits.txt
    fi
    echo "$output">>validation_results.txt
done<$1
