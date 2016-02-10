#!/bin/bash

# [1] presolve
# [2] gmi cuts
# [3] mir cuts
# [4] pp tech
# [5] fp heur

BIN=$1

for i in `ls -w1 samples`
do
    echo -n $i ' '
    ./$BIN 1 1 1 2 0 $i < samples/$i | tee -a mir_gmi_cut_times
done

