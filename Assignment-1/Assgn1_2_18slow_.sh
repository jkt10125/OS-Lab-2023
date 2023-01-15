#!/usr/bin/bash
uname=(`tr '[A-Z]' '[a-z]' <$1`)
fruit=(`tr '[A-Z]' '[a-z]' <fruits.txt`)
for p in ${uname[@]}; do
    ok="YES"
    for q in ${fruit[@]}; do
        [ ${#p} -lt 5 ] ||
        [ ${#p} -gt 20 ] ||
        [[ "$p" =~ ^([^0-9]*)$ ]] ||
        [[ "$p" =~ .*[^a-zA-Z0-9].* ]] ||
        [[ "$p" =~ ^[^a-zA-Z] ]] ||
        [[ "$p" =~ .*"$q".* ]] && ok="NO"
    done
    echo $ok
done
