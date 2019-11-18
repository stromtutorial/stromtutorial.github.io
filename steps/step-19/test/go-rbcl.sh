#!/bin/bash

PAUP="/Users/plewis/bin/paup4a166_osx"

# Usage: 
#   ./go.sh keep  <-- does not delete data and output files after running
#   ./go.sh clean <-- runs program and then deletes data and output files (default)

STROM=~/bin/strom19
if [ ! -f "$STROM" ]; then
    echo "$STROM not found, aborting."
    exit 0
fi

DIR="$(pwd)"
cd $DIR

echo
echo "Copying data files to \"$DIR\"..."
cp ../../../assets/data/rbcl10.nex .
cp ../../../assets/data/rbcl10poly.tre .

echo
echo "Copying strom-rbcl.conf to strom.conf..."
cp strom-rbcl.conf strom.conf

echo
echo "Running $STROM..."
$STROM &>output.txt

echo
echo "Comparing output files with reference..."
diff params.txt reference-output/seven/params.txt > diff-params.txt
if [ -s diff-params.txt ]
then
     echo "  params.txt differs from reference (see diff-params.txt for details)"
else
     echo "  params.txt is identical to reference"
fi

diff trees.tre reference-output/seven/trees.tre > diff-trees.txt
if [ -s diff-trees.txt ]
then
     echo "  trees.tre differs from reference (see diff-trees.txt for details)"
else
     echo "  trees.tre is identical to reference"
fi

if [[ "$#" -ne 1 || "$1" != "keep" ]]
then
    echo
    echo "Cleaning up (specify \"keep\" to skip)..."
    rm -f rbcl10.nex
    rm -f rbcl10poly.tre
    rm -f output.txt
    rm -f params.txt
    rm -f trees.tre
    rm -f diff-trees.txt
    rm -f diff-params.txt
fi
