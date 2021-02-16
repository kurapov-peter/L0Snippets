#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "LLVMSPIRVLib/LLVMSPIRVLib.h"

#include "llvm/Support/raw_ostream.h"

#include <memory>
#include <vector>
#include <sstream>
#include <fstream>

using namespace llvm;

int main() {
  LLVMContext ctx;
  std::unique_ptr<Module> module = std::make_unique<Module>("code_generated", ctx);
  module->setTargetTriple("spir64-unknown-unknown");
  IRBuilder<> builder(ctx);

  std::vector<Type *> args{Type::getFloatPtrTy(ctx, 1), Type::getFloatPtrTy(ctx, 1)};
  FunctionType *f_type = FunctionType::get(Type::getVoidTy(ctx), args, false);
  Function *f =
      Function::Create(f_type, GlobalValue::LinkageTypes::ExternalLinkage,
                       "plus1", module.get());
  f->setCallingConv(CallingConv::SPIR_KERNEL);

  // get_global_id
  FunctionType *ggi_type =
      FunctionType::get(Type::getInt32Ty(ctx), {Type::getInt32Ty(ctx)}, false);
  Function *get_global_idj =
      Function::Create(ggi_type, GlobalValue::LinkageTypes::ExternalLinkage,
                       "_Z13get_global_idj", module.get());
  get_global_idj->setCallingConv(CallingConv::SPIR_FUNC);

  BasicBlock *entry = BasicBlock::Create(ctx, "entry", f);

  builder.SetInsertPoint(entry);
  Constant *zero = ConstantInt::get(Type::getInt32Ty(ctx), 0);
  Constant *onef = ConstantFP::get(ctx, APFloat(1.f));
  Value *idx = builder.CreateCall(get_global_idj, zero, "idx");
  auto argit = f->args().begin();
  Value *firstElemSrc = builder.CreateGEP(f->args().begin(), idx, "src.idx");
  Value *firstElemDst = builder.CreateGEP(++argit, idx, "dst.idx");
  Value *ldSrc = builder.CreateLoad(Type::getFloatTy(ctx), firstElemSrc, "ld");
  Value *result = builder.CreateFAdd(ldSrc, onef, "foo");
  builder.CreateStore(result, firstElemDst);
  builder.CreateRetVoid();

  module->print(errs(), nullptr);

  SPIRV::TranslatorOpts opts;
  opts.enableAllExtensions();
  // opts.setDesiredBIsRepresentation(SPIRV::BIsRepresentation::OpenCL12);
  opts.setDebugInfoEIS(SPIRV::DebugInfoEIS::OpenCL_DebugInfo_100);
  opts.setDesiredBIsRepresentation(SPIRV::BIsRepresentation::SPIRVFriendlyIR);

  std::ostringstream ss;
  std::string err;
  auto success = writeSpirv(module.get(), opts, ss, err);
  if (!success) {
    errs() << "Spirv translation failed with error: " << err << "\n";
  } else {
    errs() << "Spirv tranlsation success.\n";
  }
  errs() << "Code size: " << ss.str().size() << "\n";

  std::ofstream out("spv_gen.spv", std::ios::binary);
  out.write(ss.str().c_str(), ss.str().size());

  return 0;
}

