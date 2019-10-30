#!/bin/bash

# Usage: 
#   ./go.sh keep  <-- does not delete data and output files after running
#   ./go.sh clean <-- runs program and then deletes data and output files (default)

STROM=~/bin/step13
if [ ! -f "$STROM" ]; then
    echo "$STROM not found, aborting."
    exit 0
fi

DIR="$(pwd)"
cd $DIR

echo
echo "Running $STROM..."
$STROM &>output.txt

echo
echo "Comparing output files with reference..."
diff output.txt reference-output/output.txt > diff-output.txt
if [ -s diff-output.txt ]
then
     echo "  output.txt differs from reference (see diff-output.txt for details)"
else
     echo "  output.txt is identical to reference"
fi

if [[ "$#" -ne 1 || "$1" != "keep" ]]
then
    echo
    echo "Cleaning up (specify \"keep\" to skip)..."
    rm -f output.txt
    rm -f diff-output.txt
fi
