#!/bin/bash

# Script name: Assgn1_8_18.sh

# Create "main.csv" if it doesn't exist
if [ ! -f "main.csv" ]
then
    echo "date,category,amount,name" > "main.csv"
fi


while getopts  ":c:n:s:h" opt
do
    case $opt in
        c)
            optarg_c=${OPTARG,,} ;;
        n)
            optarg_n=${OPTARG,,} ;;
        s)
            optarg_s=${OPTARG,,} ;;
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

if [[ $# -gt 0 ]] && [[ $# -lt 4 ]]
then
    echo "Incomplete arguments"
    exit 1
fi

if [ $# -ge 4 ]
then
    echo "$1,$2,$3,$4" >> "main.csv"
    echo "$(head -n1 "main.csv" && tail -n+2 "main.csv" | sort -t- -k 3.1,3.2 -k 2n -k 1n)" > "main.csv"
fi


if [ -n "$optarg_c" ]
then
    tail -n+2 "main.csv" | awk -v column=$optarg_c -F, 'BEGIN{sum=0} { if ( tolower($2) == column ) sum += $3 }; END {print "Total spend on " column ": " sum}' 
fi

if [ -n "$optarg_n" ]
then
    tail -n+2 "main.csv" | awk -v column=$optarg_n -F, 'BEGIN{sum=0} { if ( tolower($4) == column ) sum += $3 }; END {print "Total spend by " column ": " sum}' 
fi

if [ -n "$optarg_s" ]
then
    if [ $optarg_s == "category" ]
    then echo "$(head -n1 "main.csv" && tail -n+2 "main.csv" | sort -t, -k2 )" > "main.csv"
    elif [ $optarg_s == "amount" ]
    then echo "$(head -n1 "main.csv" && tail -n+2 "main.csv" | sort -t, -k3n )" > "main.csv"
    elif [ $optarg_s == "name" ]
    then echo "$(head -n1 "main.csv" && tail -n+2 "main.csv" | sort -t, -k4 )" > "main.csv"
    elif [ $optarg_s == "date" ]
    then echo "$(head -n1 "main.csv" && tail -n+2 "main.csv" | sort -t- -k 3.1,3.2 -k 2n -k 1n )" > "main.csv"
    else
        echo "Invalid argument to -s."
    fi
fi

if [ -n "$optarg_h" ]
then
    echo -e "\n\n\nNAME"
    echo -e "\t\tAssgn1_8_18.sh - A shell script for tracking expenses\n"

    echo "SYNOPSIS"
    echo -e "\t\tsh Assgn1_8_18.sh [-c category] [-n name] [-s column] [-h] record [...]\n"

    echo "DESCRIPTION"
    echo -e "\t\tAssgn1_8_18.sh is a shell script that helps track expenses by manipulating a csv file named "main.csv" [in same directory as script]."
    echo -e "\t\tThe csv file stores the following columns: Date, Category, Amount, and Name."
    echo -e "\t\tBy default, the script accepts 4 arguments representing a new record (row) in the csv and adds the record to the csv."
    echo -e "\t\tThe script also has the following optional flags:"
    echo -e "\t\t-c 'category': accepts a category and prints the amount of money spent in that category"
    echo -e "\t\t-n 'name': accepts a name and prints the amount spent by that person"
    echo -e "\t\t-s 'column': sorts the csv by the specified column name"
    echo -e "\t\t-h: shows the help prompt, including the name of the utility, usage, and description\n"

    echo "EXAMPLES"
    echo -e "\t\tsh Assgn1_8_18.sh 01-01-23 drinks 250 Mohan"
    echo -e "\t\tsh Assgn1_8_18.sh -c food -n Mohan -s amount 01-01-23 food 550 Sudarshan\n"

    echo "NOTE"
    echo -e "\t\tBy default, all rows in "main.csv" are sorted in chronological order by date. The user may use any number of flags depending upon the need.\n"

    echo "AUTHOR"
    echo -e "\t\tWritten by Abhay, Ani, and JKT\n"
fi
