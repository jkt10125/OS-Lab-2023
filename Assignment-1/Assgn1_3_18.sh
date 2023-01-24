#!/usr/bin/bash

mkdir -p $2

input_jq_string="[."$3
for atr in ${@:4}
do
    input_jq_string=$input_jq_string",."$atr
done
input_jq_string=$input_jq_string"]"

for file in $1/*.jsonl
do

    filename=$2/`basename $file .jsonl`.csv
    echo "$input_jq_string" | sed 's/\[//' | sed 's/\]//' | sed 's/\.//g' > $filename
    curl -s $file | cat $file | jq -r --arg input_jq_string $input_jq_string "$input_jq_string | join(\",\")" >> $filename 

done
