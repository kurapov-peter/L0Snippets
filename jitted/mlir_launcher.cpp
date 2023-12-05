// Take mlir input and submit it to the driver via zeCreateKernel call.
// llvm.emit_c_interface ?

#include <mlir/ExecutionEngine/ExecutionEngine.h>
#include <mlir/ExecutionEngine/JitRunner.h>
#include <mlir/IR/BuiltinOps.h>
#include <mlir/Parser/Parser.h>
#include <mlir/Support/FileUtilities.h>
#include <mlir/Support/LogicalResult.h>

#include "mlir/Dialect/AMX/AMXDialect.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Arith/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/ArmNeon/ArmNeonDialect.h"
#include "mlir/Dialect/Async/IR/Async.h"
#include "mlir/Dialect/Bufferization/IR/Bufferization.h"
#include "mlir/Dialect/Bufferization/Transforms/FuncBufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Complex/IR/Complex.h"
#include "mlir/Dialect/ControlFlow/IR/ControlFlow.h"
#include "mlir/Dialect/DLTI/DLTI.h"
#include "mlir/Dialect/EmitC/IR/EmitC.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/GPU/IR/GPUDialect.h"
#include "mlir/Dialect/Index/IR/IndexDialect.h"
#include "mlir/Dialect/LLVMIR/LLVMDialect.h"
#include "mlir/Dialect/LLVMIR/NVVMDialect.h"
#include "mlir/Dialect/LLVMIR/ROCDLDialect.h"
#include "mlir/Dialect/Linalg/IR/Linalg.h"
#include "mlir/Dialect/MLProgram/IR/MLProgram.h"
#include "mlir/Dialect/Math/IR/Math.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/NVGPU/IR/NVGPUDialect.h"
#include "mlir/Dialect/OpenACC/OpenACC.h"
#include "mlir/Dialect/OpenMP/OpenMPDialect.h"
#include "mlir/Dialect/PDL/IR/PDL.h"
#include "mlir/Dialect/PDLInterp/IR/PDLInterp.h"
#include "mlir/Dialect/Quant/QuantOps.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/SCF/TransformOps/SCFTransformOps.h"
#include "mlir/Dialect/SPIRV/IR/SPIRVDialect.h"
#include "mlir/Dialect/Shape/IR/Shape.h"
#include "mlir/Dialect/Shape/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/SparseTensor/IR/SparseTensor.h"
#include "mlir/Dialect/SparseTensor/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Tensor/IR/Tensor.h"
#include "mlir/Dialect/Tensor/IR/TensorInferTypeOpInterfaceImpl.h"
#include "mlir/Dialect/Tensor/IR/TensorTilingInterfaceImpl.h"
#include "mlir/Dialect/Tensor/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/Tosa/IR/TosaOps.h"
#include "mlir/Dialect/Transform/IR/TransformDialect.h"
#include "mlir/Dialect/Vector/IR/VectorOps.h"
#include "mlir/Dialect/Vector/Transforms/BufferizableOpInterfaceImpl.h"
#include "mlir/Dialect/X86Vector/X86VectorDialect.h"
#include "mlir/IR/Dialect.h"
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>

#include <mlir/InitAllDialects.h>
#include <mlir/InitAllPasses.h>
#include <mlir/InitAllTranslations.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/LLVMTranslationInterface.h>

#include <llvm/Support/CommandLine.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>

mlir::LogicalResult ReadFromFile(mlir::OwningOpRef<mlir::ModuleOp> &module,
                                 llvm::StringRef inputFilename,
                                 mlir::MLIRContext &context) {

  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> fileOrErr =
      llvm::MemoryBuffer::getFileOrSTDIN(inputFilename);
  if (std::error_code ec = fileOrErr.getError()) {
    llvm::errs() << "Could not open input file: " << ec.message() << "\n";
    return mlir::failure();
  }

  // Parse the input mlir.
  llvm::SourceMgr sourceMgr;
  sourceMgr.AddNewSourceBuffer(std::move(*fileOrErr), llvm::SMLoc());
  module = mlir::parseSourceFile<mlir::ModuleOp>(sourceMgr, &context);
  if (!module) {
    llvm::errs() << "Error can't load file " << inputFilename << "\n";
    return mlir::failure();
  }
  return mlir::success();
}

mlir::LogicalResult RunJit(mlir::ModuleOp module) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();

  // Register the translation from MLIR to LLVM IR, which must happen before we
  // can JIT-compile.
  //   mlir::registerBuiltinDialectTranslation(*module->getContext());
  mlir::registerLLVMDialectTranslation(*module->getContext());

  mlir::ExecutionEngineOptions opts{
      .sharedLibPaths = {"/localdisk/pakurapo/torch-mlir/build/lib/"
                         "liblevel-zero-runtime.so.18git"}};
  auto result = mlir::ExecutionEngine::create(module, opts);
  if (auto error = result.takeError()) {
    std::cerr << "Could not create an execution engine ("
              << toString(std::move(error)) << ")" << std::endl;
    return mlir::failure();
  }
  llvm::Error err = (*result)->invoke("my_add", 5, 4);
  return mlir::success();
}

int main(int argc, char *argv[]) {
  namespace cl = llvm::cl;
  static cl::opt<std::string> mlirFilename(
      cl::Positional, cl::desc("<input mlir file>"), cl::init("-"));

  mlir::registerAsmPrinterCLOptions();
  mlir::registerMLIRContextCLOptions();
  cl::ParseCommandLineOptions(argc, argv);

  if (mlirFilename == "-" &&
      llvm::sys::Process::FileDescriptorIsDisplayed(fileno(stdin)))
    llvm::errs() << "(processing input from stdin now, hit ctrl-c/ctrl-d to"
                    "interrupt)\n";

  std::string errorMessage;
  auto file = mlir::openInputFile(mlirFilename, &errorMessage);
  if (!file) {
    llvm::errs() << errorMessage << "\n";
    return -1;
  }

  mlir::registerAllPasses();
  mlir::registerAllTranslations();
  mlir::DialectRegistry registry;
  mlir::registerAllDialects(registry);
  //   registry.insert<mlir::func::FuncDialect, mlir::LLVM::LLVMDialect,
  //                   mlir::index::IndexDialect, mlir::spirv::SPIRVDialect,
  //                   mlir::arith::ArithDialect, mlir::memref::MemRefDialect,
  //                   mlir::gpu::GPUDialect, mlir::math::MathDialect>();
  using namespace mlir;
  registry.insert<
      acc::OpenACCDialect, /*affine::AffineDialect,*/ amdgpu::AMDGPUDialect,
      amx::AMXDialect, arith::ArithDialect, arm_neon::ArmNeonDialect,
      arm_sve::ArmSVEDialect, async::AsyncDialect,
      bufferization::BufferizationDialect, cf::ControlFlowDialect,
      complex::ComplexDialect, DLTIDialect, emitc::EmitCDialect,
      func::FuncDialect, gpu::GPUDialect, index::IndexDialect,
      linalg::LinalgDialect, LLVM::LLVMDialect, math::MathDialect,
      memref::MemRefDialect, ml_program::MLProgramDialect, nvgpu::NVGPUDialect,
      NVVM::NVVMDialect, omp::OpenMPDialect, pdl::PDLDialect,
      pdl_interp::PDLInterpDialect, quant::QuantizationDialect,
      ROCDL::ROCDLDialect, scf::SCFDialect, shape::ShapeDialect,
      sparse_tensor::SparseTensorDialect, spirv::SPIRVDialect,
      tensor::TensorDialect, tosa::TosaDialect, transform::TransformDialect,
      vector::VectorDialect, x86vector::X86VectorDialect>();
  mlir::MLIRContext context(registry);

  mlir::OwningOpRef<mlir::ModuleOp> module;
  auto result = ReadFromFile(module, mlirFilename, context);
  if (failed(result)) {
    llvm::errs() << "Could not read mlir module from file " << mlirFilename
                 << "\n";
    return -1;
  }

  if (mlir::failed(RunJit(*module))) {
    llvm::errs() << "Execution failed.\n";
    return -1;
  }

  return 0;
}
