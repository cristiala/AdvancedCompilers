# LLVM Assignment 1
This repository contains the completed assignments for implementing basic versions of Loop Invariant Code Motion (LICM) and Induction Variable Elimination (IVE) within the LLVM Compiler Infrastructure, using the LLVM Tutor framework.

The environment is configured to build against LLVM 21 (built from source from the release/21.x branch).


## 1. Installation and Setup Guide (WSL2)
I used WSL2 (Kali Linux/Ubuntu) on Windows.
Ensure you have the necessary development tools installed in WSL environment:

```bash
sudo apt update
sudo apt install build-essential cmake git ninja-build
```

## 2. Clone the Repository
Replace 'your-repo-link' with the actual clone URL
git clone <your-repo-link> llvm-tutor

Build and install LLVM 21 from source since the assignment requires LLVM 21. 
Clone the LLVM source code llvm-project.

```bash
git clone https://github.com/llvm/llvm-project.git
cd llvm-project
git checkout release/21.x

# Create build directory
mkdir build_21 && cd build_21

# Configure the build with Ninja and set the installation path
cmake -G Ninja ../llvm \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_TARGETS_TO_BUILD=host \
  -DLLVM_ENABLE_PROJECTS="clang" \
  -DCMAKE_INSTALL_PREFIX=/usr/local/llvm-21

# Build (This will take 1-3 hours depending on your hardware)
ninja -j4
```

Install the binaries and libraries
```bash
sudo ninja install
```
## 3. Configure and Build the Passes
Navigate to the tutor's build directory and configure it to link against the new LLVM 21 installation.
```bash
/home/clalarcon/llvm-tutor/build_21
```

Configure CMake, pointing to the LLVM 21 installation root
NOTE: I modified the root CMakeLists.txt to use /usr/local/llvm-21 as the root path.
```bash
cmake .. -DLLVM_DIR=/usr/local/llvm-21
```

Build all custom passes (including SimpleLICM and DerivedInductionVar)
```bash
make
```

## 4. Files modified and implementation
- Simple LICM Pass: (llvm-tutor/lib/SimpleLICM.cpp)
  - Implemented Worklist Algorithm to iteratively find loop-invariant instructions. 
- Derived IV Pass: (llvm-tutor/lib/DerivedInductionVar.cpp)
  - Transformation Pass for Induction Variable Elimination (IVE).
  - Targets inner loops of loop nests.
  - Uses ScalarEvolution (SCEVUtils::generateRateLimitInstruction) to identify affine recurrences and replaces the derived IV with equivalent instructions. 
- Build Configuration: (llvm-tutor/lib/CMakeLists.txt)
  - Modified to add the new pass targets (SimpleLICM, AffineRecurrence, DerivedInductionVar) and their corresponding source file lists (*_SOURCES).
  - Root CMakeLists: (llvm-tutor/CMakeLists.txt)
    - Modified around line 13 to manually set the LLVM root path, resolving dependency checking errors.


## Test the Completed Passes
Test commands reference the opt tool and the compiled .so libraries in the build directory.

A. Simple LICM (Loop Invariant Code Motion)
```bash 
loop-invariant code.Bash/usr/local/llvm-21/bin/opt \
-load-pass-plugin ./lib/libSimpleLICM.so \
-passes=simple-licm \
-S \
-o outputs/optimized_licm.ll \
/home/clalarcon/llvm-tutor/test/StaticCallCounter_Loop.ll
```
Verify the output for Hoisting: ... messages.

B. Test Derived IV Elimination
```bash 
/usr/local/llvm-21/bin/opt \
-load-pass-plugin ./lib/libDerivedInductionVar.so \
-passes=derived-iv \
-S \
-o outputs/optimized_ive.ll \
/home/clalarcon/llvm-tutor/test/riv_integer.ll
```

## Test cases under /test
- LICM_Chain.ll	--> Worklist Propagation
- IVE_GEP.ll --> Pointer IV Elimination (Address)