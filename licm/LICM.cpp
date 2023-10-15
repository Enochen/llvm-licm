#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <iostream>

using namespace llvm;

namespace {

struct LICMPass : public PassInfoMixin<LICMPass> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    auto changed = false;

    LoopInfo &loop_info = AM.getResult<LoopAnalysis>(F);

    for (auto *loop : loop_info) {

      // Create preheader
      AddPreheader(loop, loop_info);

      for (auto &BB : loop->blocks()) {
        for (auto I = BB->begin(), E = BB->end(); I != E;) {
          Instruction &Inst = *I;
          // makeLoopInvariant will set changed = true when appropriate
          // (pass by reference)
          if (loop->makeLoopInvariant(&Inst, changed)) {
            Inst.moveBefore(loop->getHeader()->getFirstNonPHI());
            I = Inst.eraseFromParent();
          } else {
            ++I;
          }
        }
      }
    }

    // std::cout << changed;
    return changed ? PreservedAnalyses::none() : PreservedAnalyses::all();
  };

  void AddPreheader(Loop *loop, LoopInfo &LI) {
    auto *header = loop->getHeader();

    // Make preheader bb right before header
    auto preheader = BasicBlock::Create(header->getContext(),
                                        header->getName() = "_preheader",
                                        header->getParent(), header);
    auto jmp = BranchInst::Create(header, preheader);
    loop->addBasicBlockToLoop(preheader, LI);
    loop->moveToHeader(preheader);

    // Get all predecessors of header outside loop
    auto outside_preds = std::vector<BasicBlock *>();
    for (auto *pred : predecessors(header)) {
      if (!loop->contains(pred)) {
        outside_preds.push_back(pred);
        pred->getTerminator()->replaceSuccessorWith(header, preheader);
      }
    }
    // probably need to do something with outside_preds to fix phi nodes
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
                    FPM.addPass(LICMPass());
                    return true;
                  }
                  return false;
                });
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                  MPM.addPass(createModuleToFunctionPassAdaptor(LICMPass()));
                });
          }};
}
