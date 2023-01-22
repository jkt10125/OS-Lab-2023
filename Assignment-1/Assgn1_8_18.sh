#!/bin/bash

# Script name: Assgn1_8_18.sh

# Create main.csv if it doesn't exist
if [ ! -f main.csv ]
then
    touch main.csv
fi


while getopts  ":c:n:s:h" opt
do
    case $opt in
        c)
            optarg_c=$OPTARG ;;
        n)
            optarg_n=$OPTARG ;;
        s)
            optarg_s=$OPTARG ;;
        h)
            optarg_h="true" ;;
        :)
            echo "$0: Must supply an argument to -$OPTARG." >&2
            exit 1 ;;
        \?)
            echo "Invalid option -$OPTARG ignored." >&2 ;;
    esac
done



shift $(($OPTIND-1))


if [ $# -lt 4 ]
then
    echo "Incomplete arguments"
    exit 1
fi


(cat main.csv && echo "$1,$2,$3,$4") | sort -t- -k 3.1,3.2 -k 2n -k 1n -o main.csv



if [ -n "$optarg_c" ]
then
    cat main.csv | awk -v column=$optarg_c -F, 'BEGIN{sum=0} { if ( $2 == column ) sum += $3 }; END {print sum}' 
fi

if [ -n "$optarg_n" ]
then
    cat main.csv | awk -v column=$optarg_n -F, 'BEGIN{sum=0} { if ( $4 == column ) sum += $3 }; END {print sum}' 
fi

if [ -n "$optarg_s" ]
then
    if [ $optarg_s == "category" ]
    then cat main.csv | sort -t, -k2 -o main.csv
    elif [ $optarg_s == "amount" ]
    then cat main.csv | sort -t, -k3n -o main.csv
    elif [ $optarg_s == "name" ]
    then cat main.csv | sort -t, -k4 -o main.csv
    elif [ $optarg_s == "date" ]
    then cat main.csv | sort -t- -k 3.1,3.2 -k 2n -k 1n -o main.csv
    else
        echo "Invalid argument to -s."
    fi
fi

if [ -n "$optarg_h" ]
then
    echo "NAME"
    echo -e "\t\tAssgn1_8_18.sh - A shell script for tracking expenses\n"

    echo "SYNOPSIS"
    echo -e "\t\tsh Assgn1_8_18.sh [-c category] [-n name] [-s column] [-h] record [...]\n"

    echo "DESCRIPTION"
    echo -e "\t\tAssgn1_8_18.sh is a shell script that helps track expenses by manipulating a csv file named main.csv [in same directory as script]."
    echo -e "\t\tThe csv file stores the following columns: Date, Category, Amount, and Name."
    echo -e "\t\tBy default, the script accepts 4 arguments representing a new record (row) in the csv and adds the record to the csv."
    echo -e "\t\tThe script also has the following optional flags:"
    echo -e "\t\t-c 'category': accepts a category and prints the amount of money spent in that category"
    echo -e "\t\t-n 'name': accepts a name and prints the amount spent by that person"
    echo -e "\t\t-s 'column': sorts the csv by the specified column name"
    echo -e "\t\t-h: shows the help prompt, including the name of the utility, usage, and description\n"

    echo "EXAMPLES"
    echo -e "\t\tsh script.sh 01-01-23 drinks 250 Mohan"
    echo -e "\t\tsh script.sh -c food -n Mohan -s amount 01-01-23 food 550 Sudarshan\n"

    echo "NOTE"
    echo -e "\t\tBy default, all rows in main.csv are sorted in chronological order by date. The user may use any number of flags depending upon the need.\n"

    echo "AUTHOR"
    echo -e "\t\tWritten by Abhay, Ani, JKT and Tanmay\n"
fi
