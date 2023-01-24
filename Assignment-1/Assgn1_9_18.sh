#!/bin/bash
cat $1|cut -d" " -f2|sort|uniq -c|sort -k 1nr -k2
cat $1|cut -d" " -f1|sort|uniq -c| awk '{if($1==1) count++; else print $2} END {print count}'


