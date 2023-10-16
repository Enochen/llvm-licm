# clang is used to create ll file
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S test/loop_sum.c -o loop_sum.ll

# opt is used to apply specific optimizations
OPT_PATH=opt
if [[ "$OSTYPE" == "darwin"* ]]; then 
  OPT_PATH=`brew --prefix llvm`/bin/opt
fi

$OPT_PATH -S -passes=mem2reg loop_sum.ll -o output_mem2reg.ll
$OPT_PATH -S -load-pass-plugin=`ls build/licm/LICMPass.*` -passes=LICMPass output_mem2reg.ll -o output_licm.ll
