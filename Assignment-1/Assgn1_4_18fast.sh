#!/bin/bash
sed  "/$2/{s/\(.\)\(.\)/\L\1\U\2/g;p;}" "$1" 

# sed "/$2/{s/\(.\)\(.\)/\L\1\U\2/g;p;}" 
