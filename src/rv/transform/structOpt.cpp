#include <rv/transform/structOpt.h>

#include <vector>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/ADT/SmallSet.h>
#include <llvm/Transforms/Utils/ValueMapper.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DataLayout.h>

#include <rv/vectorizationInfo.h>


#include <rvConfig.h>

using namespace rv;
using namespace llvm;

#if 1
#define IF_DEBUG_SO IF_DEBUG
#else
#define IF_DEBUG_SO if (false)
#endif

// TODO move this into vecInfo
VectorShape
rv::StructOpt::getVectorShape(llvm::Value & val) const {
  if (vecInfo.hasKnownShape(val)) return vecInfo.getVectorShape(val);
  else if (isa<Constant>(val)) return VectorShape::uni();
  else return VectorShape::undef();
}

rv::StructOpt::StructOpt(VectorizationInfo & _vecInfo, const DataLayout & _layout)
: vecInfo(_vecInfo)
, layout(_layout)
{}

void
rv::StructOpt::transformLayout(llvm::AllocaInst & allocaInst, ValueToValueMapTy & transformMap) {
  SmallSet<Value*, 16> seen;
  std::vector<Instruction*> allocaUsers;
  allocaUsers.push_back(&allocaInst);
  SmallSet<PHINode*,4> postProcessPhis;

  IF_DEBUG_SO { errs() << "-- transforming: " << allocaInst << "\n"; }
  while (!allocaUsers.empty()) {
    auto * inst = allocaUsers.back();
    allocaUsers.pop_back();

  // have seen this user
    if (!seen.insert(inst).second) continue;

    auto * allocaInst = dyn_cast<AllocaInst>(inst);
    auto * store = dyn_cast<StoreInst>(inst);
    auto * load = dyn_cast<LoadInst>(inst);
    auto * gep = dyn_cast<GetElementPtrInst>(inst);
    auto * phi = dyn_cast<PHINode>(inst);

  // transform this (final) gep
    if (load || store) {
      IF_DEBUG_SO { errs() << "\t- transform load/store " << *inst << "\n"; }
      IRBuilder<> builder(inst->getParent(), inst->getIterator());

      auto * ptrVal = load ? load->getPointerOperand() : store->getPointerOperand();

      assert (transformMap.count(ptrVal));
      Value * vecPtrVal = transformMap[ptrVal];

      // cast *<8 x float> to * float
      auto * vecElemTy = cast<VectorType>(vecPtrVal->getType()->getPointerElementType());
      auto * plainElemTy = vecElemTy->getElementType();

      auto * castElemTy = builder.CreatePointerCast(vecPtrVal, PointerType::getUnqual(plainElemTy));

      const unsigned alignment = layout.getTypeStoreSize(plainElemTy) * vecInfo.getVectorWidth();
      vecInfo.setVectorShape(*castElemTy, VectorShape::cont(alignment));


      if (load)  {
        auto * vecLoad = builder.CreateLoad(castElemTy, load->getName());
        vecInfo.setVectorShape(*vecLoad, vecInfo.getVectorShape(*load));
        vecLoad->setAlignment(alignment);

        load->replaceAllUsesWith(vecLoad);

        IF_DEBUG_SO { errs() << "\t\t result: " << *vecLoad << "\n"; }

      } else {
        auto * vecStore = builder.CreateStore(store->getValueOperand(), castElemTy, store->isVolatile());
        vecStore->setAlignment(alignment);
        vecInfo.setVectorShape(*vecStore, vecInfo.getVectorShape(*store));
        vecInfo.dropVectorShape(*store);

        IF_DEBUG_SO { errs() << "\t\t result: " << *vecStore << "\n"; }
      }

      continue; // don't step across load/store

    } else if (gep) {
      IF_DEBUG_SO { errs() << "\t- transform gep " << *gep << "\n"; }
      auto vecBasePtr = transformMap[gep->getOperand(0)];

      std::vector<Value*> indexVec;
      for (unsigned i = 1; i < gep->getNumOperands(); ++i) {
        indexVec.push_back(gep->getOperand(i));
      }

      auto * vecGep = GetElementPtrInst::Create(nullptr, vecBasePtr, indexVec, gep->getName(), gep);

      vecInfo.setVectorShape(*vecGep, VectorShape::uni());

      IF_DEBUG_SO { errs() << "\t\t result: " << *vecGep << "\n\t T:" << *vecGep->getType() << "\n"; }
      transformMap[gep] = vecGep;

    } else if (phi) {
      IF_DEBUG_SO { errs() << "\t- transform phi " << *phi << "\n"; }
      postProcessPhis.insert(phi);
      auto * vecPhiTy = cast<PHINode>(transformMap[phi])->getIncomingValue(0)->getType();
      auto * vecPhi = PHINode::Create(vecPhiTy, phi->getNumIncomingValues(), phi->getName(), phi);
      IF_DEBUG_SO { errs() << "\t\t result: " << *vecPhi << "\n"; }

      vecInfo.setVectorShape(*vecPhi, VectorShape::uni());
      vecInfo.dropVectorShape(*phi);
      transformMap[phi] = vecPhi;

    } else {
      assert(allocaInst && "unexpected instruction in alloca transformation");
    }

  // update users users
    for (auto user : inst->users()) {
      auto * userInst = dyn_cast<Instruction>(user);

      if (!userInst) continue;

      allocaUsers.push_back(userInst);
    }
  }

// repair phi nodes
  for (auto * phi : postProcessPhis) {
    auto * vecPhi = cast<PHINode>(transformMap[phi]);
    for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i) {
      vecPhi->addIncoming(transformMap[phi->getIncomingValue(i)], phi->getIncomingBlock(i));
    }
  }

// remove old code
  for (auto deadVal : seen) {
    auto *deadInst = cast<Instruction>(deadVal);
    assert(deadInst);
    if (!deadInst->getType()->isVoidTy()) {
      deadInst->replaceAllUsesWith(UndefValue::get(deadInst->getType()));
    }
    // the old alloca should be obsolete now
    vecInfo.dropVectorShape(*deadInst);
    deadInst->eraseFromParent();
  }

  transformMap.clear();
}

static bool VectorizableType(Type & type) {
  return type.isIntegerTy() || type.isFloatingPointTy();
}

/// whether any address computation on this alloc is uniform
/// the alloca can still be varying because of stored varying values
bool
rv::StructOpt::allUniformGeps(llvm::AllocaInst & allocaInst) {
  SmallSet<Value*, 16> seen;
  std::vector<Instruction*> allocaUsers;
  allocaUsers.push_back(&allocaInst);

  while (!allocaUsers.empty()) {
    auto * inst = allocaUsers.back();
    allocaUsers.pop_back();

  // have seen this user
    if (!seen.insert(inst).second) continue;

  // dont touch this alloca if its used on the outside
    if (!vecInfo.inRegion(*inst)) {
      IF_DEBUG_SO { errs() << "skip: has user outside of region: " << *inst << "\n";  }
      return false;
    }

  // inspect users
    for (auto user : inst->users()) {
      auto * userInst = dyn_cast<Instruction>(user);

      if (!userInst) continue;

      if (isa<GetElementPtrInst>(userInst)) allocaUsers.push_back(userInst);
      //
      // skip if the an alloca derived value is stored somewhere
      else if (isa<StoreInst>(userInst)) {
        if (userInst->getOperand(0) == inst) { // leaking the value!
          return false;
        }
        if (!VectorizableType(*cast<StoreInst>(userInst)->getValueOperand()->getType())) {
          IF_DEBUG_SO { errs() << "skip: accessing non-leaf element : " << *userInst << "\n"; }
          return false;
        }
      }

      // we dont care about (indirect) alloc loads
      else if (isa<LoadInst>(userInst)) {
        if (!VectorizableType(*cast<LoadInst>(userInst)->getType())) {
          IF_DEBUG_SO { errs() << "skip: accessing non-leaf element : " << *userInst << "\n"; }
          return false;
        }
        continue;
      }

      // skip unforeseen users
      else if (!isa<PHINode>(userInst)) return false;

      // otw, descend into user
      allocaUsers.push_back(userInst);
    }

  // verify uniform address criterion
    auto * gep = dyn_cast<GetElementPtrInst>(inst);
    if (gep) {
      // check whether all gep operands are uniform (except the baseptr)
      for (unsigned i = 1; i < gep->getNumOperands(); ++i) {
        if (!getVectorShape(*gep->getOperand(i)).isUniform()) {
          IF_DEBUG_SO { errs() << "skip: non uniform gep: " << *gep << " at index " << i << " : " << *gep->getOperand(i) << "\n"; }
          return false;
        }
      }
    }
  }

// TODO verify that no unseen value sneaks in through phis
  for (auto * allocaUser : seen) {
    auto * phi = dyn_cast<PHINode>(allocaUser);
    if (!phi) continue;

    for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i) {
      auto * inVal = phi->getIncomingValue(i);
      if (isa<Instruction>(inVal) && !seen.count(inVal)) {
        IF_DEBUG_SO { errs() << "skip: alloca mixes with non-derived value " << *inVal << " at phi " << *phi << "\n"; }
        return false;
      }
    }
  }

  return true;
}

/// try to optimize the layout of this alloca
bool
rv::StructOpt::optimizeAlloca(llvm::AllocaInst & allocaInst) {
  IF_DEBUG_SO {errs() << "\n# trying to optimize Alloca " << allocaInst << "\n"; }

// legality checks
  if (getVectorShape(allocaInst).isUniform()) {
    IF_DEBUG_SO {errs() << "skip: uniform.n"; }
    return false;
  }

  // does this alloca have a vectorizable type?
  auto * vecAllocTy = vectorizeType(*allocaInst.getAllocatedType());
  if (!vecAllocTy) {
    IF_DEBUG_SO { errs() << "skip: non-vectorizable type.\n"; }
    return false;
  }
  IF_DEBUG_SO { errs() << "vectorized type: " << *vecAllocTy << "\n"; }
  //
  // this alloca may only be:
  // loaded from, stored to (must note store the pointer) use to derive addresses (with uniform indicies) or passsed through phi nodes
  if (!allUniformGeps(allocaInst)) return false;
  IF_DEBUG_SO { errs() << "vectorizable uses!\n"; }


// we may transorm the alloc

  // replace alloca
  auto * vecAlloc = new AllocaInst(vecAllocTy, allocaInst.getName(), &allocaInst);

  const unsigned alignment = layout.getPrefTypeAlignment(vecAllocTy); // TODO should enfore a stricter alignment at this point
  vecInfo.setVectorShape(*vecAlloc, VectorShape::uni(alignment));

  ValueToValueMapTy transformMap;

  transformMap[&allocaInst] = vecAlloc;

// update all gep/phi shapes
  transformLayout(allocaInst, transformMap);

  return false;
}

llvm::Type *
rv::StructOpt::vectorizeType(llvm::Type & scalarAllocaTy) {
// primite type -> vector
  if (scalarAllocaTy.isIntegerTy() ||
      scalarAllocaTy.isFloatingPointTy())
    return VectorType::get(&scalarAllocaTy, vecInfo.getVectorWidth());

// finite aggrgate -> aggrgate of vectorized elemnts
  if (scalarAllocaTy.isStructTy()) {
    std::vector<Type*> elemTyVec;
    for (unsigned i = 0; i < scalarAllocaTy.getStructNumElements(); ++i) {
      auto * vecElem = vectorizeType(*scalarAllocaTy.getStructElementType(i));
      if (!vecElem) return nullptr;
      elemTyVec.push_back(vecElem);
    }
    return StructType::get(scalarAllocaTy.getContext(), elemTyVec, false); // TODO packed?
  }

  bool isVectorTy = isa<VectorType>(scalarAllocaTy);
  bool isArrayTy = isa<ArrayType>(scalarAllocaTy);

  if (isVectorTy || isArrayTy) {
    auto * elemTy = scalarAllocaTy.getSequentialElementType();
    auto numElements = isArrayTy ? scalarAllocaTy.getArrayNumElements() : scalarAllocaTy.getVectorNumElements();

    auto * vecElem = vectorizeType(*elemTy);
    if (!vecElem) return nullptr;
    // we create an arraytype in any case since we may promote scalar types (ints) to vectors
    return ArrayType::get(vecElem, numElements);
  }

  // unforeseen
  return nullptr;
}

bool
rv::StructOpt::run() {
  IF_DEBUG_SO { errs() << "-- struct opt log --\n"; }

  bool change = false;
  for (auto & bb : vecInfo.getScalarFunction()) {
    auto itBegin = bb.begin(), itEnd = bb.end();
    for (auto it = itBegin; it != itEnd; ) {
      auto * allocaInst = dyn_cast<AllocaInst>(it++);
      if (!allocaInst) continue;

      change |= optimizeAlloca(*allocaInst);
    }
  }

  IF_DEBUG_SO { errs() << "-- end of struct opt log --\n"; }

  return change;
}

