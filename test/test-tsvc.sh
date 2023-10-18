#!/bin/bash

set -x
echo "Debug mode on"

# usage
# from repo root
#   cd llvm-licm
# build llvm ir and compile executables
#   ./test/test-tsvc.sh
# or build and compile leaving only executables
#   ./test/test-tsvc.sh --obj
# then run the benchmarks, about 40 minutes each full run
#   ./test/tsvc_mem2reg.exe
#   ./test/tsvc_licm.exe


# clang is used to create ll file
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S test/tsvc.c -o test/tsvc.ll
#clang -Xclang -O3 -emit-llvm -S test/tsvc.c -o test/tsvc.ll

# opt is used to apply specific optimizations
LLVM_BIN_DIR=""
if [[ "$OSTYPE" == "darwin"* ]]; then 
  LLVM_BIN_DIR="`brew --prefix llvm`/bin"
fi

# first pass mem2reg
${LLVM_BIN_DIR}/opt -S -passes=mem2reg test/tsvc.ll -o test/tsvc_mem2reg.ll

# check that pass was built according to README
ls_output="$(ls build/licm/LICMPass.*)"
if [ -z "$ls_output" ]; then
    echo "Error: No pass built"
    echo "  cd llvm-licm"
    echo "  mkdir build"
    echo "  cd build"
    echo "  cmake .."
    echo "  make"
    echo "  cd .."
    echo "  ./build.sh"
    exit 1
fi

# refine with our LICM pass
${LLVM_BIN_DIR}/opt -S -load-pass-plugin=`ls build/licm/LICMPass.*` -passes=LICMPass test/tsvc_mem2reg.ll -o test/tsvc_licm.ll


# more passes attempt
# PASSES='verify,annotation2metadata,forceattrs,inferattrs,function(lower-expect,simplifycfg<bonus-inst-threshold=1;no-forward-switch-cond;no-switch-to-lookup;keep-loops;no-hoist-common-insts;no-sink-common-insts>,sroa,early-cse,callsite-splitting),openmp-opt,ipsccp,called-value-propagation,globalopt,function(mem2reg),deadargelim,function(instcombine,simplifycfg<bonus-inst-threshold=1;no-forward-switch-cond;no-switch-to-lookup;keep-loops;no-hoist-common-insts;no-sink-common-insts>),inliner-wrapper,globalopt,globaldce,elim-avail-extern,rpo-function-attrs,require<globals-aa>,function(float2int,lower-constant-intrinsics,loop(loop-rotate),loop-distribute,inject-tli-mappings,loop-vectorize,loop-load-elim,instcombine,simplifycfg<bonus-inst-threshold=1;forward-switch-cond;switch-to-lookup;no-keep-loops;hoist-common-insts;sink-common-insts>,slp-vectorizer,vector-combine,instcombine,loop-unroll,transform-warning,instcombine,require<opt-remark-emit>,alignment-from-assumptions,loop-sink,instsimplify,div-rem-pairs,simplifycfg<bonus-inst-threshold=1;no-forward-switch-cond;no-switch-to-lookup;keep-loops;no-hoist-common-insts;no-sink-common-insts>),cg-profile,globaldce,constmerge,rel-lookup-table-converter,function(annotation-remarks),verify'
#
#${LLVM_BIN_DIR}opt -S -passes=$PASSES test/tsvc.ll -o test/tsvc_mem2reg.ll
#${LLVM_BIN_DIR}opt -S -load-pass-plugin=`ls build/licm/LICMPass.*` -passes=LICMPass test/tsvc_mem2reg.ll -o test/tsvc_licm.ll

if [[ $1 == "--obj" ]]; then 
  ${LLVM_BIN_DIR}/clang -O0 -lm -x ir test/tsvc_mem2reg.ll -o test/tsvc_mem2reg.exe
  ${LLVM_BIN_DIR}/clang -O0 -lm -x ir test/tsvc_licm.ll -o test/tsvc_licm.exe
  rm -f test/tsvc_mem2reg.ll test/tsvc_licm.ll test/tsvc.ll
else
  ${LLVM_BIN_DIR}/clang -lm -x ir test/tsvc_mem2reg.ll -o test/tsvc_mem2reg.exe
  ${LLVM_BIN_DIR}/clang -lm -x ir test/tsvc_licm.ll -o test/tsvc_licm.exe
fi
