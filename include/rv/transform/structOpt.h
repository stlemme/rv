#ifndef RV_TRANSFORM_STRUCTOPT_H
#define RV_TRANSFORM_STRUCTOPT_H

#include <llvm/IR/Value.h>
#include <llvm/Transforms/Utils/ValueMapper.h>

#include "rv/vectorShape.h"

namespace llvm {
  class AllocaInst;
  class DataLayout;
}

namespace rv {

class VectorizationInfo;

class StructOpt {
  VectorizationInfo & vecInfo;
  const llvm::DataLayout & layout;

  /// whether the alloca layout can be changed without breaking the IR
  /// I.e. not the case if the allocated object is passed to a call.
  bool mayChangeLayout(llvm::AllocaInst & allocInst);

  /// whether every address computation on this alloc is uniform
  /// the alloca can still be varying because of stored varying values
  bool allUniformGeps(llvm::AllocaInst & allocInst);

  /// try to optimize the layout of this alloca
  bool optimizeAlloca(llvm::AllocaInst & allocInst);

  /// return a vectorized type for this type
  /// this translates AoS to SoA
  /// returns nullptr if the struct can not be vectorized
  llvm::Type * vectorizeType(llvm::Type & scalarAllocaTy);

  VectorShape getVectorShape(llvm::Value & val) const;

  // execute the data layout transformation
  void transformLayout(llvm::AllocaInst & allocaInst, llvm::ValueToValueMapTy & transformMap);
public:
  StructOpt(VectorizationInfo & _vecInfo, const llvm::DataLayout & _layout);

  bool run();
};


} // namespace rv

#endif// RV_TRANSFORM_STRUCTOPT_H
