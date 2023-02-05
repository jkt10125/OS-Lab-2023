#!/bin/bash
file=$1
string=$2

while read -r line; do
    line_num=$(echo $line | grep -n "$string" | cut -f1 -d:)
    if [ -n "$line_num" ]; then
        echo "Line number: $line_num, Line: $line"
    fi
done < <(grep "$string" "$file")