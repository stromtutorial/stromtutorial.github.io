#!/bin/bash

DIR="$( cd "$( dirname "$0" )" && pwd )"
cd $DIR

rm params.bak
mv params.txt params.bak
rm -f resolution-class-data.js
cp ../params.txt .
python p2js.py resclass params.txt
open histogram.html
