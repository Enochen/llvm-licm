#include "llvm/Analysis/DomTreeUpdater.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {

/**
 * Return true if an instruction has loop invariant operands given the loop it resides in and
 * a set of values already marked LI 
 */
bool hasLoopInvariantOperands(const Loop &L, const DenseSet<Value *> &LI, const Instruction &Inst) {
  for (const auto &Use : Inst.operands()) {
    if (LI.contains(&*Use))
      // Operand already marked LI
      continue;

    if (auto *UseInst = dyn_cast<Instruction>(&*Use)) {
      if (L.contains(UseInst->getParent())) {
        // The def of this operand is inside the loop
        return false;
      }
    }
  }
  return true;
}

struct LICMPass : public PassInfoMixin<LICMPass> {
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U) {
    auto *Preheader = L.getLoopPreheader();
    assert(Preheader && "Loop does not have preheader");
    SmallVector<Instruction *> ToHoist;
    DenseSet<Value *> LI;
    bool Converged = false;
    while (!Converged) {
      Converged = true;
      for (auto *BB : L.blocks()) {
        for (auto &Inst : *BB) {
          if (LI.contains(&Inst) || // Already have marked LI 
              Inst.mayHaveSideEffects() || // Cannot hoist effectful instruction
              !hasLoopInvariantOperands(L, LI, Inst) || // Operands not LI
              Inst.getOpcode() == Instruction::Br) continue; // Cannot hoist branch
          
          // Otherwise, we mark this instruction as LI
          LI.insert(&Inst);
          ToHoist.push_back(&Inst);
          Converged = false;
        }
      }
    }

    // Hoist all LI instructions to preheader
    for (auto *Inst : ToHoist) {
      Inst->moveBefore(Preheader->getTerminator());
    }
    return PreservedAnalyses::none();
  };

  static StringRef name() {
    return "LICMPass";
  }
};

} // namespace 

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {.APIVersion = LLVM_PLUGIN_API_VERSION,
          .PluginName = "LICMPass",
          .PluginVersion = "v0.1",
          .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
              [](StringRef name, FunctionPassManager &FPM,
                ArrayRef<PassBuilder::PipelineElement>) {
                if (name != "LICMPass") return false;
                FPM.addPass(createFunctionToLoopPassAdaptor(LICMPass()));
                return true;
              });
          }};
}
