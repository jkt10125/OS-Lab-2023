#!/usr/bin/bash
uname=(`sed 's/.*/\L&/' <$1`)
fruit=(`sed 's/.*/\L&/' <fruits.txt`)
for p in ${uname[@]}; do
    ok="YES"
    [ ${#p} -lt 5 ] ||
    [ ${#p} -gt 20 ] ||
    [[ "$p" =~ ^([^0-9]*)$ ]] ||
    [[ "$p" =~ .*[^a-zA-Z0-9].* ]] ||
    [[ "$p" =~ ^[^a-zA-Z] ]] && ok="NO"
    for q in ${fruit[@]}; do
        [[ "$p" =~ .*"$q".* ]] && ok="NO"
    done
    echo $ok
done
