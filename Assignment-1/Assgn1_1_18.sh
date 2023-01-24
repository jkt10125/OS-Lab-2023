#!/bin/bash
function gcd {
    test `expr $1 % $2` -eq 0&&echo $2||gcd $2 `expr $1 % $2`    
}
lcm=1
for p in `rev $1 | sed 's/^0*//'`
do
    lcm=`expr $p / \`gcd $lcm $p\` \* $lcm`
done
echo $lcm
