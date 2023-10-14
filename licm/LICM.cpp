#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct LICMPass : public PassInfoMixin<LICMPass> {
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U) {
    bool changed = false;
    for (auto &BB : L.blocks()) {
      for (auto I = BB->begin(), E = BB->end(); I != E;) {
        Instruction &Inst = *I;
        // makeLoopInvariant will set changed = true when appropriate
        // (pass by reference)
        if (L.makeLoopInvariant(&Inst, changed)) {
          Inst.moveBefore(L.getHeader()->getFirstNonPHI());
          I = Inst.eraseFromParent();
        } else {
          ++I;
        }
      }
    }
    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  };
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {.APIVersion = LLVM_PLUGIN_API_VERSION,
          .PluginName = "LICM pass",
          .PluginVersion = "v0.1",
          .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerLateLoopOptimizationsEPCallback(
                [](LoopPassManager &LPM, OptimizationLevel Level) {
                  LPM.addPass(LICMPass());
                });
          }};
}
