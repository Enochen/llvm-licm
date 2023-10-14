# llvm-licm

based on CS6120 llvm-pass-skeleton

It implements loop invariant code motion for simple regions of C code.
It's for LLVM 17.


Initial Build:
    $ cd llvm-licm
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ cd ..

Recurring Build:

    $ cd llvm-licm
    $ ./build.sh


Run:

    Empty skeleton pass
    $ clang -fpass-plugin=`echo build/skeleton/SkeletonPass.*` something.c

    OurLICM Pass
    see build.sh

