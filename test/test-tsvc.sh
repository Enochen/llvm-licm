#!/bin/bash

# clang is used to create ll file
clang -Xclang -O3 -emit-llvm -S test/tsvc.c -o test/tsvc.ll

# opt is used to apply specific optimizations
LLVM_BIN_DIR=""
if [[ "$OSTYPE" == "darwin"* ]]; then 
  LLVM_BIN_DIR=`brew --prefix llvm`/bin/
fi

PASSES='verify,annotation2metadata,forceattrs,inferattrs,function(lower-expect,simplifycfg<bonus-inst-threshold=1;no-forward-switch-cond;no-switch-to-lookup;keep-loops;no-hoist-common-insts;no-sink-common-insts>,sroa,early-cse,callsite-splitting),openmp-opt,ipsccp,called-value-propagation,globalopt,function(mem2reg),deadargelim,function(instcombine,simplifycfg<bonus-inst-threshold=1;no-forward-switch-cond;no-switch-to-lookup;keep-loops;no-hoist-common-insts;no-sink-common-insts>),inliner-wrapper,globalopt,globaldce,elim-avail-extern,rpo-function-attrs,require<globals-aa>,function(float2int,lower-constant-intrinsics,loop(loop-rotate),loop-distribute,inject-tli-mappings,loop-vectorize,loop-load-elim,instcombine,simplifycfg<bonus-inst-threshold=1;forward-switch-cond;switch-to-lookup;no-keep-loops;hoist-common-insts;sink-common-insts>,slp-vectorizer,vector-combine,instcombine,loop-unroll,transform-warning,instcombine,require<opt-remark-emit>,alignment-from-assumptions,loop-sink,instsimplify,div-rem-pairs,simplifycfg<bonus-inst-threshold=1;no-forward-switch-cond;no-switch-to-lookup;keep-loops;no-hoist-common-insts;no-sink-common-insts>),cg-profile,globaldce,constmerge,rel-lookup-table-converter,function(annotation-remarks),verify'

${LLVM_BIN_DIR}opt -S -passes=$PASSES test/tsvc.ll -o test/tsvc_mem2reg.ll
${LLVM_BIN_DIR}opt -S -load-pass-plugin=`ls build/licm/LICMPass.*` -passes=LICMPass test/tsvc_mem2reg.ll -o test/tsvc_licm.ll

if [[ $1 == "--obj" ]]; then 
  ${LLVM_BIN_DIR}clang -O0 -lm -x ir test/tsvc_mem2reg.ll -o test/tsvc_mem2reg.exe
  ${LLVM_BIN_DIR}clang -O0 -lm -x ir test/tsvc_licm.ll -o test/tsvc_licm.exe
  rm test/tsvc_mem2reg.ll test/tsvc_licm.ll test/tsvc.ll
fi
