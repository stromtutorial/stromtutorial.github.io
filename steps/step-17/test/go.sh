#!/bin/bash

# Usage: 
#   ./go.sh keep  <-- does not delete data and output files after running
#   ./go.sh clean <-- runs program and then deletes data and output files (default)

export LD_LIBRARY_PATH="$HOME/lib:$LD_LIBRARY_PATH"

STROM=~/bin/step17
if [ ! -f "$STROM" ]; then
    echo "$STROM not found, aborting."
    exit 0
fi

STROMB=~/bin/step17b
if [ ! -f "$STROMB" ]; then
    echo "$STROMB not found, aborting."
    exit 0
fi

DIR="$(pwd)"
cd $DIR

echo
echo "Copying data files to \"$DIR\"..."
cp ../../../assets/data/rbcl10.nex .
cp ../../../assets/data/rbcl10.tre .

echo
echo "Running $STROM..."
$STROM &>output-posterior.txt
mv params.txt params-posterior.txt
mv trees.tre trees-posterior.tre

echo
echo "Running $STROMB..."
$STROMB --niter=100000  &>output-prior.txt
mv params.txt params-prior.txt
mv trees.tre trees-prior.tre

echo
echo "Comparing output files with reference..."
diff params-posterior.txt reference-output/params-posterior.txt > diff-params-posterior.txt
if [ -s diff-params-posterior.txt ]
then
     echo "  params-posterior.txt differs from reference (see diff-params-posterior.txt for details)"
else
     echo "  params-posterior.txt is identical to reference"
fi

diff trees-posterior.tre reference-output/trees-posterior.tre > diff-trees-posterior.txt
if [ -s diff-trees-posterior.txt ]
then
     echo "  trees-posterior.tre differs from reference (see diff-trees-posterior.txt for details)"
else
     echo "  trees-posterior.tre is identical to reference"
fi

diff params-prior.txt reference-output/params-prior.txt > diff-params-prior.txt
if [ -s diff-params-prior.txt ]
then
     echo "  params-prior.txt differs from reference (see diff-params-prior.txt for details)"
else
     echo "  params-prior.txt is identical to reference"
fi

diff trees-prior.tre reference-output/trees-prior.tre > diff-trees-prior.txt
if [ -s diff-trees-prior.txt ]
then
     echo "  trees-prior.tre differs from reference (see diff-trees-prior.txt for details)"
else
     echo "  trees-prior.tre is identical to reference"
fi

if [[ "$#" -ne 1 || "$1" != "keep" ]]
then
    echo
    echo "Cleaning up (specify \"keep\" to skip)..."
    rm -f rbcl10.nex
    rm -f rbcl10.tre
    rm -f output-prior.txt
    rm -f output-posterior.txt
    rm -f params-prior.txt
    rm -f trees-prior.tre
    rm -f params-posterior.txt
    rm -f trees-posterior.tre
    rm -f diff-trees-prior.txt
    rm -f diff-params-prior.txt
    rm -f diff-trees-posterior.txt
    rm -f diff-params-posterior.txt
fi
