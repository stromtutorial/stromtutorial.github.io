#!/bin/bash

# Usage: 
#   ./go.sh keep  <-- does not delete data and output files after running
#   ./go.sh clean <-- runs program and then deletes data and output files (default)

STROM=~/bin/step16
if [ ! -f "$STROM" ]; then
    echo "$STROM not found, aborting."
    exit 0
fi

DIR="$(pwd)"
cd $DIR

echo
echo "Copying data files to \"$DIR\"..."
cp ../../../assets/data/rbcl10.nex .
cp ../../../assets/data/rbcl10.tre .

echo
echo "Running $STROM using a partitioned nucleotide model..."
cp strom-nucleotide.conf strom.conf
$STROM &>output-nucleotide.txt
mv params.txt params-nucleotide.txt
mv trees.tre trees-nucleotide.tre

echo
echo "Running $STROM using a codon model..."
cp strom-codon.conf strom.conf
$STROM &>output-codon.txt
mv params.txt params-codon.txt
mv trees.tre trees-codon.tre

echo
echo "Comparing output files with reference..."
diff params-nucleotide.txt reference-output/params-nucleotide.txt > diff-params-nucleotide.txt
if [ -s diff-params-nucleotide.txt ]
then
     echo "  params-nucleotide.txt differs from reference (see diff-params-nucleotide.txt for details)"
else
     echo "  params-nucleotide.txt is identical to reference"
fi

diff trees-nucleotide.tre reference-output/trees-nucleotide.tre > diff-trees-nucleotide.txt
if [ -s diff-trees-nucleotide.txt ]
then
     echo "  trees-nucleotide.tre differs from reference (see diff-trees-nucleotide.txt for details)"
else
     echo "  trees-nucleotide.tre is identical to reference"
fi

diff params-codon.txt reference-output/params-codon.txt > diff-params-codon.txt
if [ -s diff-params-codon.txt ]
then
     echo "  params-codon.txt differs from reference (see diff-params-codon.txt for details)"
else
     echo "  params-codon.txt is identical to reference"
fi

diff trees-codon.tre reference-output/trees-codon.tre > diff-trees-codon.txt
if [ -s diff-trees-codon.txt ]
then
     echo "  trees-codon.tre differs from reference (see diff-trees-codon.txt for details)"
else
     echo "  trees-codon.tre is identical to reference"
fi

if [[ "$#" -ne 1 || "$1" != "keep" ]]
then
    echo
    echo "Cleaning up (specify \"keep\" to skip)..."
    rm -f rbcl10.nex
    rm -f rbcl10.tre

    rm -f output-nucleotide.txt
    rm -f params-nucleotide.txt

    rm -f output-codon.txt
    rm -f params-codon.txt

    rm -f diff-params-nucleotide.txt
    rm -f diff-params-codon.txt

    rm -f diff-trees-nucleotide.txt
    rm -f diff-trees-codon.txt

    rm -f trees-nucleotide.tre
    rm -f trees-codon.tre
    
    rm -f strom.conf
fi
