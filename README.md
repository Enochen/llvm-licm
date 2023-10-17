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

Test:

    $ ./test/test-tsvc.sh

Run:

    LICM Pass
    $ clang -fpass-plugin=`echo build/licm/LICMPass.*` something.c
