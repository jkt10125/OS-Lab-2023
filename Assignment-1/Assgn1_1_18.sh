#!/usr/bin/bash
gcd() { 
    [ $(($1%$2)) == 0 ]&&echo $2||gcd $2 $(($1%$2))
 }
lcm=1
arr=(`rev $1|sed 's/^0*//'`)
for p in ${arr[@]} 
do
    t=$(gcd $lcm $p)
    lcm=$((p/t*lcm))
done
echo $lcm