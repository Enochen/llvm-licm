set -x

mkdir -p build
cd build && make && cd ..
#cd build && `brew --prefix llvm`/bin/clang -O0 -emit-llvm -S -loop-simplify -fpass-plugin=`ls OurLICM/OurLICMPass.*` ../test/loop_sum.c
cd build && `brew --prefix llvm`/bin/clang -O0 -fpass-plugin=`ls OurLICM/OurLICMPass.*` ../test/loop_sum.c -o loop_sum.pass
#echo "execute with ./build/loop_sum.pass"
echo "expect 24"
./build/loop_sum.pass
