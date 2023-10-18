#!/bin/zsh
awk '{gsub(/^[ \t]+|[ \t]+$/, ""); gsub(/[ \t]+/, ","); print}' run08_tsvc_mem2reg_licm.out > run08.csv
