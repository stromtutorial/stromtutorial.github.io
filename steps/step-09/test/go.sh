#!/bin/bash

# Usage: 
#   ./go.sh keep  <-- does not delete data and output files after running
#   ./go.sh clean <-- runs program and then deletes data and output files (default)

STROM=~/bin/step09
if [ ! -f "$STROM" ]; then
    echo "$STROM not found, aborting."
    exit 0
fi

DIR="$(pwd)"
cd $DIR

echo
echo "Copying data files to \"$DIR\"..."
cp ../../../assets/data/rbcL.nex .
cp ../../../assets/data/datatest.nex .

cp strom-a.conf strom.conf

echo
echo "Running $STROM on first example..."
$STROM &>output-a.txt

cp strom-b.conf strom.conf

echo
echo "Running $STROM on second example..."
$STROM &>output-b.txt

echo
echo "Comparing output files with reference..."
diff output-a.txt reference-output/output-a.txt > diff-output-a.txt
if [ -s diff-output-a.txt ]
then
     echo "  output-a.txt differs from reference (see diff-output-a.txt for details)"
else
     echo "  output-a.txt is identical to reference"
fi

diff output-b.txt reference-output/output-b.txt > diff-output-b.txt
if [ -s diff-output-b.txt ]
then
     echo "  output-b.txt differs from reference (see diff-output-b.txt for details)"
else
     echo "  output-b.txt is identical to reference"
fi

if [[ "$#" -ne 1 || "$1" != "keep" ]]
then
    echo
    echo "Cleaning up (specify \"keep\" to skip)..."
    rm -f rbcL.nex
    rm -f datatest.nex
    rm -f output-a.txt
    rm -f output-b.txt
    rm -f diff-output-a.txt
    rm -f diff-output-b.txt
    rm -f strom.conf
fi
