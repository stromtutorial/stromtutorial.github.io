#!/bin/bash

DIR="$( cd "$( dirname "$0" )" && pwd )"
cd $DIR

rm m.bak
mv m.js m.bak
cp ../m.js .
open spectrum.html
