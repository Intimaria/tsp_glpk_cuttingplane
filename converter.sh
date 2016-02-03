#!/bin/bash

for i in w_*.svg
do
    echo $i
    rsvg-convert $i > $i.png
done
