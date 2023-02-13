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

int main()
{
  LLVMContext ctx;
  std::unique_ptr<Module> module = std::make_unique<Module>("code_generated", ctx);
  module->setTargetTriple("spir-unknown-unknown");
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
#if LLVM_VERSION_MAJOR > 12
  Value *firstElemSrc = builder.CreateGEP(argit->getType()->getPointerElementType(), argit, idx, "src.idx");
  ++argit;
  Value *firstElemDst = builder.CreateGEP(argit->getType()->getPointerElementType(), argit, idx, "dst.idx");
#else
  Value *firstElemSrc = builder.CreateGEP(f->args().begin(), idx, "src.idx");
  Value *firstElemDst = builder.CreateGEP(++argit, idx, "dst.idx");
#endif
  Value *ldSrc = builder.CreateLoad(Type::getFloatTy(ctx), firstElemSrc, "ld");
  Value *result = builder.CreateFAdd(ldSrc, onef, "foo");
  builder.CreateStore(result, firstElemDst);
  builder.CreateRetVoid();

  // set metadata -- pretend we're opencl (see
  // https://github.com/KhronosGroup/SPIRV-LLVM-Translator/blob/master/docs/SPIRVRepresentationInLLVM.rst#spir-v-instructions-mapped-to-llvm-metadata)
  Metadata *spirv_src_ops[] = {
      ConstantAsMetadata::get(
          ConstantInt::get(Type::getInt32Ty(ctx), 3 /*OpenCL_C*/)),
      ConstantAsMetadata::get(
          ConstantInt::get(Type::getInt32Ty(ctx), 102000 /*OpenCL ver 1.2*/))};
  NamedMDNode *spirv_src = module->getOrInsertNamedMetadata("spirv.Source");
  spirv_src->addOperand(MDNode::get(ctx, spirv_src_ops));

  // Not required, leaving for reference
  // Metadata *opencl_ver_ops[] = {
  //   ConstantAsMetadata::get(ConstantInt::get(Type::getInt32Ty(ctx), 1)),
  //   ConstantAsMetadata::get(ConstantInt::get(Type::getInt32Ty(ctx), 0))
  // };
  // NamedMDNode *opencl_ver =
  // module->getOrInsertNamedMetadata("opencl.ocl.version");
  // opencl_ver->addOperand(MDNode::get(ctx, opencl_ver_ops));

  // Metadata *spirv_ver_ops[] = {
  //   ConstantAsMetadata::get(ConstantInt::get(Type::getInt32Ty(ctx), 1)),
  //   ConstantAsMetadata::get(ConstantInt::get(Type::getInt32Ty(ctx), 2))
  // };
  // NamedMDNode *spirv_ver =
  // module->getOrInsertNamedMetadata("opencl.spir.version");
  // spirv_ver->addOperand(MDNode::get(ctx, spirv_ver_ops));

  // Metadata *spirv_gen_ops[] = {
  //   ConstantAsMetadata::get(ConstantInt::get(Type::getInt32Ty(ctx), 6)),
  //   ConstantAsMetadata::get(ConstantInt::get(Type::getInt32Ty(ctx), 14))
  // };
  // NamedMDNode *spirv_gen =
  // module->getOrInsertNamedMetadata("spirv.Generator");
  // spirv_gen->addOperand(MDNode::get(ctx, spirv_gen_ops));

  module->print(errs(), nullptr);

  SPIRV::TranslatorOpts opts;
  opts.enableAllExtensions();
  opts.setDesiredBIsRepresentation(SPIRV::BIsRepresentation::OpenCL12);
  opts.setDebugInfoEIS(SPIRV::DebugInfoEIS::OpenCL_DebugInfo_100);

  std::ostringstream ss;
  std::string err;
  auto success = writeSpirv(module.get(), opts, ss, err);
  if (!success)
  {
    errs() << "Spirv translation failed with error: " << err << "\n";
  }
  else
  {
    errs() << "Spirv tranlsation success.\n";
  }
  errs() << "Code size: " << ss.str().size() << "\n";

  std::ofstream out("spv_gen.spv", std::ios::binary);
  out.write(ss.str().c_str(), ss.str().size());

  return 0;
}
