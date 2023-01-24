#!/usr/bin/bash
gcd() { 
    [ $(($1%$2)) == 0 ]&&echo $2||gcd $2 $(($1%$2))
 }
lcm=1
for p in `rev $1|sed 's/^0*//'`
do
    lcm=$((p/$(gcd $lcm $p)*lcm))
done
echo $lcm