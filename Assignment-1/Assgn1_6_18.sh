#!/bin/bash
istream="input.txt"
ostream="output.txt"
MAX=1000000

declare -ia leastPrimeFactor

sieveOfEratosthenes() {

    echo "Sieve of eratosthenes algorithm running...">&2

    i=1
    while [ $i -le $MAX ]
    do
        leastPrimeFactor[$i]=$i
        i=$((i+1))
    done

    i=4
    while [ $i -le $MAX ]
    do
        leastPrimeFactor[$i]=2
        i=$((i+2))
    done

    j=3
    while [ $((j*j)) -le $MAX ]
    do 
        if [ ${leastPrimeFactor[$j]} -eq $j ]
        then
            i=$((j*j))
            while [ $i -le $MAX ]
            do
                if [[ ${leastPrimeFactor[$i]} -eq $i ]]
                then
                    leastPrimeFactor[$i]=$j
                fi
                i=$((i+j))
            done   
        fi 
        j=$((j+2))
    done
}

sieveOfEratosthenes

echo "Sieve of eratosthenes algorithm completed">&2

echo "Reading numbers from $istream...">&2

while read -r n
do
    i=1
    while [ $i -le $n ]
    do
        while [ $n -ne 1 ]
        do
            factor=${leastPrimeFactor[$n]}
            while [ $((n%factor)) -eq 0 ]
            do
                n=$((n/factor))
            done
            echo -n "$factor "
        done
        i=$((i+1))
    done
    echo ""
done < $istream > $ostream

echo "Script executed successfully">&2
echo "Output saved to $ostream">&2
