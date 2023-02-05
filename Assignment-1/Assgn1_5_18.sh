#!/bin/bash
const=2
for file in $(find "$1" -name '*.py')
do
    echo "Starting to Parse File ==========>  $file"

    line_numbers=1
    reading_double=0
    reading_single=0

    while read -r line
    do
        if grep -q "^[\s]*\"\"\".*\"\"\"$" <<< "$line"
        then
            echo -n "$line_numbers: "
            grep -o \"\"\".*\"\"\"$ <<< "$line"
            ((line_numbers++))
            continue
        fi
        if grep -q ^[\s]*\'\'\'.*\'\'\'$ <<< "$line"
        then
            echo -n "$line_numbers: "
            grep -o \'\'\'.*\'\'\'$ <<< "$line"
            ((line_numbers++))
            continue
        fi

        count=$(echo $line | grep -o '"""' | wc -l)
        if [[ $((count%2)) == 1 ]] && [[ $((reading_double%2)) == 1 ]];
        then
            grep -o ^.*\"\"\" <<< "$line"
            reading_double=0
        elif (( reading_double == 1 )); then
            echo "    $line"
        elif [[ $((count%2)) == 1 ]] && [[ $((reading_double%2)) == 0 ]]
        then
            echo -n "$line_numbers: "
            grep -o \"\"\".*$ <<< "$line"
            reading_double=1
        fi

        count=$(echo $line | grep -o "'''" | wc -l)
        if [[ $((count%2)) == 1 ]] && [[ $((reading_single%2)) == 1 ]];
        then
            grep -o ^.*\'\'\' <<< "$line"
            reading_single=0
        elif (( reading_single == 1 )); then
            echo "    $line"
        elif [[ $((count%2)) == 1 ]] && [[ $((reading_single%2)) == 0 ]]
        then
            echo -n "$line_numbers: "
            grep -o \'\'\'.*$ <<< "$line"
            reading_single=1
        fi

        if grep -q ^.*[#].*$ <<< $line
        then  
            echo -n "$line_numbers: "
            grep -o '#.*$' <<< "$line"
        fi
        ((line_numbers++))
    done < $file
    echo ""
done