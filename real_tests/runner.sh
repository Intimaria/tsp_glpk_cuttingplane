#!/bin/bash

# [1] presolve
# [2] gmi cuts
# [3] mir cuts
# [4] pp tech
# [5] fp heur

BIN=$1
PROB=$2

for i in '1 0 0 2 0' '1 1 1 2 0' '1 0 1 2 0' '1 1 1 0 2 0'
do
    ./$BIN $i $i < $2
done

