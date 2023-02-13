#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/Bitcode/BitcodeWriter.h"

#include <memory>
#include <vector>

using namespace llvm;

int main()
{
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

  // module->print(errs(), nullptr);

  std::error_code ec;
  raw_fd_ostream os("gen.bc", ec);
  WriteBitcodeToFile(*module.get(), os);
  os.flush();
  if (ec.value())
    errs() << "Err code is " << ec.value() << "\n";

  return 0;
}
