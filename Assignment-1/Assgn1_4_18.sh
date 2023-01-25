#!/bin/bash
sed "/$2/{s/\([a-zA-Z]\)/\U\1/g;s/\([a-zA-Z]\)\([^a-zA-Z]*\)\([a-zA-Z]\)/\1\2\L\3\E/g;}" $1
