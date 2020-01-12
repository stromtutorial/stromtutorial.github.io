#!/bin/bash

# Usage: 
#   ./go.sh keep  <-- does not delete data and output files after running
#   ./go.sh clean <-- runs program and then deletes data and output files (default)

export LD_LIBRARY_PATH="$HOME/lib:$LD_LIBRARY_PATH"

STROM=~/bin/step10
if [ ! -f "$STROM" ]; then
    echo "$STROM not found, aborting."
    exit 0
fi

DIR="$(pwd)"
cd $DIR

echo
echo "Copying data files to \"$DIR\"..."
cp ../../../assets/data/rbcL.nex .
cp ../../../assets/data/rbcLjc.tre .

cp strom-a.conf strom.conf

echo
echo "Running $STROM (unpartitioned case)..."
$STROM &>output-unpartitioned.txt

cp strom-b.conf strom.conf

echo
echo "Running $STROM (partitioned case)..."
$STROM &>output-partitioned.txt

echo
echo "Comparing output files with reference..."
diff output-unpartitioned.txt reference-output/output-unpartitioned.txt > diff-output-unpartitioned.txt
if [ -s diff-output-unpartitioned.txt ]
then
     echo "  output-unpartitioned.txt differs from reference (see diff-output-unpartitioned.txt for details)"
else
     echo "  output-unpartitioned.txt is identical to reference"
fi

diff output-partitioned.txt reference-output/output-partitioned.txt > diff-output-partitioned.txt
if [ -s diff-output-partitioned.txt ]
then
     echo "  output-partitioned.txt differs from reference (see diff-output-partitioned.txt for details)"
else
     echo "  output-partitioned.txt is identical to reference"
fi

if [[ "$#" -ne 1 || "$1" != "keep" ]]
then
    echo
    echo "Cleaning up (specify \"keep\" to skip)..."
    rm -f rbcl.nex
    rm -f rbcljc.tre
    rm -f output-partitioned.txt
    rm -f output-unpartitioned.txt
    rm -f diff-output-partitioned.txt
    rm -f diff-output-unpartitioned.txt
    rm -f strom.conf
fi
