# clang is used to create ll file
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -S test/loop_sum.c -o loop_sum.ll

# opt is used to apply specific optimizations
opt -S -load-pass-plugin build/licm/LICMPass.so -passes=mem2reg loop_sum.ll -o output_mem2reg.ll
opt -S -load-pass-plugin build/licm/LICMPass.so -passes=LICMPass output_mem2reg.ll -o output_licm.ll