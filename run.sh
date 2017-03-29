#!/bin/bash

# set -e

if [ $# -eq 0 ]; then
    CASE="new/0/case0.txt"
elif [ $# -eq 3 ]; then
    CASE="$1/$2/case$3.txt"
else
    echo "Err, para"
fi

./build.sh
cp ./case_example/$CASE ./bin

cd ./bin
CASE_NAME=$(basename $CASE)
RESULT=$(basename ${CASE%.*}).result
CMD="./cdn"

if [ ! -e $(basename $CASE) ]; then
    echo "Err, no case"
    exit 1
fi

$CMD $CASE_NAME $RESULT
cat $RESULT
