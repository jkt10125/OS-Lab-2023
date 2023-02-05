#!/bin/bash
# run using ./q5.sh nmt-master/nmt>output.txt

for f in $(find $1 -name "*.py"); do
    file=$(echo "$f"|tr "/" "\n"|tail -n 1)
    echo "Name: $file"
    echo "Path: $f"
    echo "Single and Multiline comments in this file:"
    echo
    
    grep -E -n "(^|[^\"])[^[:space:]]*#[^\n\"]*" $f | awk '{print "Line "$1; gsub(/^[0-9]+:([^#]*#)/,"#",$0); print $0}'
    
    multi_flag=0
    line_no=0
    output=""
    while read -r word; do
      if [[ $word == *'"""'* ]]; then
        if [ $multi_flag == 0 ]; then
          multi_flag=1
          output="$output $word"
          temp=0
            while IFS= read -r line; do
                temp=$((temp+1))
            if [ $temp > $line_no ]; then
              line_no=$((line_no+1))
              if [[ $line == *"$word"* ]]; then
                    echo "line $line_no:"
                    break
                fi
            fi
            done < "$f"
          else
            multi_flag=0
            output="$output $word"
            echo "$output"
            output=""
          fi
        elif [ $multi_flag == 1 ]; then
          output="$output $word"
        else
          if [ -n "$output" ]; then
            echo "$output"
            output=""
          fi
        fi
      done < <(tr -s '[[:space:]]' '\n' < $f)
    if [ -n "$output" ]; then
      echo "$output "
    fi
    
done