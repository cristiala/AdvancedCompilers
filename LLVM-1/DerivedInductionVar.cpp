/* DerivedInductionVar.cpp 
 *
 * This pass detects derived induction variables using ScalarEvolution.
 *
 * Compatible with New Pass Manager
*/

#include "llvm/IR/PassManager.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Analysis/ScalarEvolutionUtils.h"
#include "llvm/Analysis/InstructionSimplify.h"


using namespace llvm;

namespace {

class DerivedInductionVar
    : public PassInfoMixin<DerivedInductionVar> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    auto &LI = AM.getResult<LoopAnalysis>(F);
    auto &SE = AM.getResult<ScalarEvolutionAnalysis>(F);

    bool Changed = false;

    // Iterate over all loops in the function
    for (Loop *L : LI.getLoopsInPreorder()) {

        // Assignment Goal: Target inner loops (those with no subloops)
        if (!L->getSubLoops().empty()) {
            continue;
        }

        errs() << "Analyzing INNER loop in function " << F.getName() << ":\n";

        BasicBlock *Preheader = L->getLoopPreheader();
        if (!Preheader) continue;

        // Collect instructions to delete later
        SmallVector<Instruction*, 8> InstructionsToDelete;

        // Iterate over all instructions in the inner loop
        for (auto *BB : L->getBlocks()) {
            for (auto &I : *BB) {
                
                // 1. Filter: Skip PHI nodes (Basic IVs) and instructions that aren't used for data
                if (isa<PHINode>(&I) || I.getType()->isVoidTy()) {
                    continue;
                }
                
                // 2. Analysis: Get the SCEV for the instruction
                const SCEV *S = SE.getSCEV(&I);

                // 3. Identification: Detect affine AddRec expressions (Derived Induction Variables)
                if (auto *AR = dyn_cast<SCEVAddRecExpr>(S)) {
                    
                    // Check if it's affine (linear recurrence)
                    if (AR->isAffine()) {
                        errs() << "  Affine Derived IV found: " << I.getName() << "\n";
                        
                        // 4. Transformation: Eliminate the IV
                        
                        // use the SCEV expression to generate a replacement instruction.
                        // This uses instructions placed in the Preheader for loop-invariant parts,
                        // and uses the Basic IV within the loop.
                        
                        // Builder starts inserting before the Preheader's terminator, 
                        // ensuring all loop-invariant parts are placed safely outside.
                        IRBuilder<> Builder(Preheader->getTerminator());
                        
                        // Generate the instruction(s) that compute the value of the DIV (S)
                        Value *ReplacementValue = SCEVUtils::generateRateLimitInstruction(
                            S, Builder, &SE);
                        
                        // Check if a valid replacement was created and is different from the original instruction
                        if (ReplacementValue && ReplacementValue != &I) {
                            
                            // Check if the replacement calculation must be inside the loop
                            // If the replacement still depends on the BIV, it should be moved inside the loop body.
                            if (Instruction *ReplacementInst = dyn_cast<Instruction>(ReplacementValue)) {
                                if (L->contains(ReplacementInst->getParent())) {
                                     // The instruction is created inside the loop by generateRateLimitInstruction, 
                                     // often right before the current instruction I.
                                } else {
                                     // If generated outside, leave it in the preheader.
                                }
                            }
                            
                            // Replace all uses of the old derived IV instruction (I) with the new value
                            I.replaceAllUsesWith(ReplacementValue);
                            
                            // Mark the old instruction for deletion
                            InstructionsToDelete.push_back(&I);
                            
                            Changed = true;
                            errs() << "  Eliminated and replaced derived IV: " << I.getName() << "\n";
                        }
                    }
                }
            }
        }
        
        // Safely delete all marked instructions after iteration is complete
        for (Instruction *I : InstructionsToDelete) {
            // Use simplifyInstruction or eraseFromParent after replacing uses
            I->eraseFromParent();
        }
    }

    if (Changed) {
        return PreservedAnalyses::none();
    }
    return PreservedAnalyses::all();
  }
};

} // namespace

// Register the pass
llvm::PassPluginLibraryInfo getDerivedInductionVarPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "DerivedInductionVar", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "derived-iv") {
                    FPM.addPass(DerivedInductionVar());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getDerivedInductionVarPluginInfo();
}
