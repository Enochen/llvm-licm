## Summary

@rcplane, @Enochen, and @zachary-kent collaborated on this project.

[LICM pass](https://github.com/Enochen/llvm-licm/blob/master/licm/LICM.cpp)

<!-- TODO Summary -->
We implemented loop invariant code motion as an LLVM pass.

## Implementation

- We wrote a Loop pass in LLVM and connected it to a FunctionToLoopPassAdaptor
  to create a Function pass. This was necessary because the built-in `mem2reg`
  pass, which helps setup the IR in SSA form is a function pass. Therefore if we
  want to chain our pass after `mem2reg` via `opt` we must also make it a
  function pass.
- We initially had written code to generate a loop preheader, although we later
  found out that process was automatically performed by LLVM via LoopSimplify.
  Anyways, our custom implementation basically created a new basic block
  pointing to the existing loop header, and redirected non-backedges previously
  to the loop header into the preheader. We also had to shift and modify phi
  nodes in order to preserve SSA semantics.
- For the actual code motion, we used the good ol' loop till convergence
  pattern. In each loop iteration, we search through all instructions in the
  loop and check if they are loop invariant. To avoid modifying the IR while
  iterating, we collect a set of loop invariant instructions as we move along.
  This not only allows us to hoist these instructions at the end of iteration,
  but also allows for faster convergence via loop-invariant operand detection.
  One step to checking whether an instruction is loop invariant is that its
  operands are all loop invariant, so being able to identify LI instructions
  ahead of hoisting leads to potentially less loop iterations. 
- To check whether an instruction is loop invariant, we essentially need to check that the instruction
    1. is not effectful
        - we use LLVM helpers such as `isSafeToSpeculativelyExecute` and
          `mayReadOrWriteMemory` to make sure the instruction does not contain
          any side effects
    2. does not have loop variant operands (as mentioned above)
        - we can trivially walk through the instructions operands and check that
          they are not in the loop or they're just values. This is made possible
          by being in SSA since the operands are instructions if not straight up
          values, so we can see where they're from.

## Testing

<!-- TODO Testing -->

## Difficulties

<!-- TODO Difficulties -->

- We had trouble getting `clang` to run our function pass. It turned out that setting -O0 to disable other optimizations had led `clang` to put the `optnone` flag on all our functions. That led to the pass not running despite loading it up. We had to supply the `-Xclang -disable-O0-optnone` flags to get it to not do this.
- However, we also wanted the `mem2reg` pass to run. After more trouble with `clang`, we `opt`ed (haha) to use `opt` instead to apply our passes, using `clang` only for generating the initial unoptimized IR.
