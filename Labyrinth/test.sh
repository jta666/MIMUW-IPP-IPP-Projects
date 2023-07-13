#!/bin/bash
prog=$1
dir=$2

for f in ${dir}/*.in; do
    filename=$(basename -- "$f")
    touch "my${filename%in}out";
    touch "my${filename%in}err";
    touch a.txt;
    valgrind --error-exitcode=123 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all --log-file="a.txt" ${prog} <${f} >my${filename%in}out 2>my${filename%in}err;
    es0=$?
    if [ $es0 -ne 0 ]; then
        echo "VALGRIND "${filename%.in}" not OK."
    else
        echo "VALGRIND "${filename%.in}" OK."
    fi
    diff "my${filename%in}out" "${f%in}out";
    es1=$?
    if [ $es1 -ne 0 ]; then
        echo "STDOUT "${filename%.in}" not OK."
    else 
        echo "STDOUT "${filename%.in}" OK."
    fi
    diff "my${filename%in}err" "${f%in}err";
    es2=$?
    if [ $es2 -ne 0 ]; then
        echo "STDERR "${filename%.in}" not OK."
    else
        echo "STDERR "${filename%.in}" OK."
    fi
    rm "my${filename%in}out";
    rm "my${filename%in}err";
    rm a.txt;
done
