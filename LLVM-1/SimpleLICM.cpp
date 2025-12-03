/* SimpleLICM.cpp
 *
 * This pass hoists loop-invariant code before the loop when it is safe to do so.
 *
 * Compatible with New Pass Manage
*/

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/CFG.h"

#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

#include "llvm/Analysis/LoopAnalysisManager.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/ValueTracking.h"

#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/ValueMapper.h"

using namespace llvm;

struct SimpleLICM : public PassInfoMixin<SimpleLICM> {
  PreservedAnalyses run(Loop &L, LoopAnalysisManager &AM,
                        LoopStandardAnalysisResults &AR,
                        LPMUpdater &) {
    DominatorTree &DT = AR.DT;

    BasicBlock *Preheader = L.getLoopPreheader();
    if (!Preheader) {
      errs() << "No preheader, skipping loop\n";
      return PreservedAnalyses::all();
    }

    SmallPtrSet<Instruction *, 8> InvariantSet;
    bool Change = true;

    // Worklist algorithm to identify loop invariant instructions
    /*************************************/
    /* Your code goes here */
    
    // Check for preheader (although the skeleton already checks this)
    if (!Preheader) {
      errs() << "No preheader, skipping loop\n";
      return PreservedAnalyses::all();
    }

    SmallVector<Instruction *, 8> Worklist;

    // --- 1. Initial Candidate Sweep ---
    // Find instructions whose operands are ALL defined outside the loop.
    for (BasicBlock *BB : L.getBlocks()) {
      for (Instruction &I : *BB) {
        
        // CONSTRAINTS CHECK: Skip Phi, memory read/write, and void instructions [cite: 11]
        if (isa<PHINode>(&I) || I.mayReadOrWriteMemory() || I.getType()->isVoidTy()) {
            continue;
        }

        bool allOperandsOutsideLoop = true;
        
        // Check if all operands are loop-invariant (i.e., defined outside the loop)
        for (Value *Operand : I.operands()) {
            
            // If the operand is an instruction *within* the loop, it's not initially invariant.
            if (Instruction *OpInst = dyn_cast<Instruction>(Operand)) {
                if (L.contains(OpInst->getParent())) {
                    allOperandsOutsideLoop = false; 
                    break;
                }
            }
        }

        // If all operands are defined outside the loop, I is an initial invariant
        if (allOperandsOutsideLoop) {
            InvariantSet.insert(&I);
            Worklist.push_back(&I);
        }
      }
    }

    // --- 2. Iterative Analysis (Worklist Algorithm) ---
    // Propagate invariance: Check users of confirmed invariant instructions.
    while (!Worklist.empty()) {
        Instruction *I = Worklist.pop_back_val();
        // Check all users of I (instructions that use I as an operand)
        for (User *U : I->users()) {
            Instruction *UserInst = dyn_cast<Instruction>(U);

            // A. Skip if not an instruction, not in the loop, or already confirmed invariant
            if (!UserInst || !L.contains(UserInst->getParent()) || InvariantSet.count(UserInst)) {
                continue;
            }

            // B. CONSTRAINTS CHECK: Skip Phi, memory, and void instructions [cite: 11]
            if (isa<PHINode>(UserInst) || UserInst->mayReadOrWriteMemory() || UserInst->getType()->isVoidTy()) {
                continue;
            }

            // C. Check if ALL operands of the UserInst are now loop-invariant
            bool allOperandsInvariant = true;
            for (Value *Operand : UserInst->operands()) {
                if (Instruction *OpInst = dyn_cast<Instruction>(Operand)) {
                    // If the operand is inside the loop AND it's NOT in the InvariantSet, UserInst is not yet invariant.
                    if (L.contains(OpInst->getParent()) && !InvariantSet.count(OpInst)) {
                        allOperandsInvariant = false;
                        break;
                    }
                }
            }

            // D. If all operands are invariant, UserInst itself is now loop-invariant
            if (allOperandsInvariant) {
                InvariantSet.insert(UserInst);
                Worklist.push_back(UserInst);
            }
        }
    }

    // Actually hoist the instructions
    for (Instruction *I : InvariantSet) {
      if (isSafeToSpeculativelyExecute(I) && dominatesAllLoopExits(I, &L, DT)) {
        errs() << "Hoisting: " << *I << "\n";
        I->moveBefore(Preheader->getTerminator());
      }
    }

    return PreservedAnalyses::none();
  }

  bool dominatesAllLoopExits(Instruction *I, Loop *L, DominatorTree &DT) {
    SmallVector<BasicBlock *, 8> ExitBlocks;
    L->getExitBlocks(ExitBlocks);
    for (BasicBlock *EB : ExitBlocks) {
      if (!DT.dominates(I, EB))
        return false;
    }
    return true;
  }
};

llvm::PassPluginLibraryInfo getSimpleLICMPluginInfo() {
  errs() << "SimpleLICM plugin: getSimpleLICMPluginInfo() called\n";
  return {LLVM_PLUGIN_API_VERSION, "simple-licm", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, LoopPassManager &LPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                  if (Name == "simple-licm") {
                    LPM.addPass(SimpleLICM());
                    return true;
                  }                  
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  errs() << "SimpleLICM plugin: llvmGetPassPluginInfo() called\n";
  return getSimpleLICMPluginInfo();
}
