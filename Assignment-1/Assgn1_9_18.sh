#!/bin/bash
cat $1|cut -d" " -f2|sort|uniq -c|sort -k 1nr -k2
# cat majors.txt | awk  '{arr[$2]++; } END {for(dep in arr) printf "%s %d\n", dep , arr[dep]}' | sort  -k 2nr -k1
cat $1|cut -d" " -f1|sort|uniq -c| awk '{if($1==1) count++; else print $2} END {print count}'
# cat $1|cut -d" " -f1|sort|uniq -c|awk '$1>1 {print $2}' 
# cat $1|cut -d" " -f1|sort|uniq -c|awk '$1 == 1 {count++} END {print count}'
# cat $1|cut -d" " -f1|sort|uniq -c|tee >(awk '$1>1 {print $2}') >(awk '$1==1 {count++} END {print count}') >/dev/null

