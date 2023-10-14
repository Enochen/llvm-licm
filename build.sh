set -x

mkdir -p build
BASE_DIR="$(pwd)"
cd build && make
cd ${BASE_DIR}
#cd build && `brew --prefix llvm`/bin/clang -O0 -emit-llvm -S -loop-simplify -fpass-plugin=`ls OurLICM/OurLICMPass.*` ../test/loop_sum.c
cd ${BASE_DIR}
cd build && `brew --prefix llvm`/bin/clang -O0 -fpass-plugin=`ls OurLICM/OurLICMPass.*` ../test/loop_sum.c -o loop_sum.pass
cd ${BASE_DIR}
#echo "execute with ./build/loop_sum.pass"
echo "expect 24"
./build/loop_sum.pass
