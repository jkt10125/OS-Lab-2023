#!/usr/bin/bash
gcd() {
    [ $(($1%$2)) -eq 0 ] && return $2 || gcd $2 $(($1%$2))
}
lcm=1
arr=(`rev $1 | sed 's/^0*//'`)
for p in ${arr[@]}
do
    gcd $lcm $p
    lcm=$((lcm*p/$?))
done
echo $lcm