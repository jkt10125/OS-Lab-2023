#!/bin/bash
# awk -v word=$2 '{ if ( match($0, word) ) print gensub(/(.)/,"\\u\\1", "g", $0); else print $0}' $1
awk -v word=$2 '{ if ( match($0, word) ) { print gensub(/(.)/,"\\L\\1", "g", gensub(/(.)/,"\\U\\1", "g", $0)); } else { print $0 } }' $1