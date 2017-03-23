#!/bin/bash

set -e

if [ $# -eq 0 ]; then
    CASE="case0.txt"
elif [ $# -eq $1 ]; then
    CASE="case$1.txt"
else
    echo "Err, para"
fi

./build.sh
cp ./case_example/$CASE ./bin

cd ./bin
RESULT=${CASE%.*}.result
CMD="./cdn"

if [ ! -e "./$CASE" ]; then
    echo "Err, no case"
    exit 1
fi

$CMD $CASE $RESULT
cat $RESULT
