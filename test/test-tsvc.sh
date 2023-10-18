#!/bin/bash

# clang is used to create ll file
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S test/tsvc.c -o test/tsvc.ll

# opt is used to apply specific optimizations
OPT_PATH=opt
if [[ "$OSTYPE" == "darwin"* ]]; then 
  OPT_PATH=`brew --prefix llvm`/bin/opt
fi

$OPT_PATH -S -passes=mem2reg test/tsvc.ll -o test/tsvc_mem2reg.ll
$OPT_PATH -S -load-pass-plugin=`ls build/licm/LICMPass.*` -passes=LICMPass test/tsvc_mem2reg.ll -o test/tsvc_licm.ll

clang -lm -x ir test/tsvc_mem2reg.ll -o test/tsvc_mem2reg
clang -lm -x ir test/tsvc_licm.ll -o test/tsvc_licm
