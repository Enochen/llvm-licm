#include "llvm/Analysis/DomTreeUpdater.h"
#include "llvm/Analysis/MemorySSAUpdater.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace llvm;

namespace {

struct LICMPass : public PassInfoMixin<LICMPass> {
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR, LPMUpdater &U) {
    DomTreeUpdater DTU(AR.DT, DomTreeUpdater::UpdateStrategy::Eager);
    MemorySSAUpdater MSSAU(AR.MSSA);
    auto *Preheader = L.getLoopPreheader();
    if (!Preheader) {
      auto *Header = L.getHeader();
      Preheader = splitBlockBefore(Header, &*Header->begin(), &DTU, &AR.LI, &MSSAU);
    }
    for (auto *BB : L.blocks()) {
      for (auto &Inst : *BB) {
        bool Hoisted = false;
        L.makeLoopInvariant(&Inst, Hoisted);
      }
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
