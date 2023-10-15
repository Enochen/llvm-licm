#include "llvm/Analysis/DomTreeUpdater.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {

struct LICMPass : public PassInfoMixin<LICMPass> {
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U) {
    auto changed = false;
    // Create preheader
    if (!L.getLoopPreheader()) {
      AddPreheader(L, AR);
      changed = true;
    }

    auto iter_changed = true;
    while (iter_changed) {
      iter_changed = false;
      for (auto &BB : L.blocks()) {
        for (auto &Inst : *BB) {
          // makeLoopInvariant will set changed = true when appropriate
          // (pass by reference)
          if (L.makeLoopInvariant(&Inst, changed)) {
            iter_changed = true;
            break;
          }
        }
        // if (iter_changed) {
        //   break;
        // }
      }
    };

    // std::cout << changed;
    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  };

  void AddPreheader(Loop &loop, LoopStandardAnalysisResults &AR) {
    auto &LI = AR.LI;
    DomTreeUpdater DTU(AR.DT, DomTreeUpdater::UpdateStrategy::Eager);
    // MemorySSAUpdater MSSAU(AR.MSSA);
    auto *header = loop.getHeader();

    // Get all predecessors of header outside loop
    auto outside_preds = std::vector<BasicBlock *>();
    for (auto *pred : predecessors(header)) {
      if (!loop.contains(pred)) {
        outside_preds.push_back(pred);
        // pred->getTerminator()->replaceSuccessorWith(header, preheader);
      }
    }

    // Make preheader bb right before header
    auto preheader = SplitBlockPredecessors(header, outside_preds, ".preheader",
                                            &DTU, &AR.LI, nullptr, true);
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
                  if (name == "LICMPass") {
                    FPM.addPass(createFunctionToLoopPassAdaptor(LICMPass()));
                    return true;
                  }
                  return false;
                });
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                  MPM.addPass(createModuleToFunctionPassAdaptor(
                      createFunctionToLoopPassAdaptor(LICMPass())));
                });
          }};
}
