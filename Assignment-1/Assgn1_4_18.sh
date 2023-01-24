#!/bin/bash
sed "/$2/{s/\([a-zA-Z]\)/\L\1/g;s/\([a-zA-Z]\)\([a-zA-Z]\)/\1\U\2/g;}" $1