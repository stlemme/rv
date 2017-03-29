//===- vectorShape.cpp -----------------------------===//
//
//                     The Region Vectorizer
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
// @author kloessner, simon

#include <iostream>
#include <sstream>
#include <cmath>

#include "rv/utils/mathUtils.h"

#include "rv/vectorShape.h"
#include "rv/vectorizationInfo.h"

namespace rv {

VectorShape::VectorShape()
    : stride(0), hasConstantStride(false), alignment(0), defined(false) {}

VectorShape::VectorShape(unsigned _alignment)
    : stride(0), hasConstantStride(false), alignment(_alignment),
      defined(true) {}

// constant stride constructor
VectorShape::VectorShape(int _stride, unsigned _alignment)
    : stride(_stride), hasConstantStride(true), alignment(_alignment),
      defined(true) {}

unsigned VectorShape::getAlignmentGeneral() const {
  assert(defined && "alignment function called on undef value");

  if (hasConstantStride) {
    if (stride == 0)
      return alignment;
    else
      return gcd(alignment, (unsigned) std::abs(stride));
  }
  else
    return alignment; // General alignment in case of varying shape
}

bool VectorShape::operator==(const VectorShape &a) const {
  return
      // both undef
      (!defined && !a.defined) ||

      // both are defined shapes
      (defined && a.defined && alignment == a.alignment && (
           // either both shapes are varying (with same alignment)
           (!hasConstantStride && !a.hasConstantStride) ||
           // both shapes are strided with same alignment
           (hasConstantStride && a.hasConstantStride && stride == a.stride)
        )
      );
}

bool VectorShape::operator!=(const VectorShape &a) const {
  return !(*this == a);
}

bool VectorShape::operator<(const VectorShape &a) const {
  if (!a.isDefined())
    return false; // Cannot be more precise than bottom
  if (!isDefined())
    return true; // Bottom is more precise then any defined shape

  if (hasConstantStride && !a.hasConstantStride)
    return true; // strided < varying

  // If both are of the same shape, decide by alignment
  if ((!hasConstantStride && !a.hasConstantStride) ||
      (hasConstantStride && a.hasConstantStride && stride == a.stride))
    return alignment % a.alignment == 0 && alignment > a.alignment;

  return false;
}

VectorShape VectorShape::join(VectorShape a, VectorShape b) {
  if (!a.isDefined())
    return b;
  if (!b.isDefined())
    return a;

  if (a.hasConstantStride && b.hasConstantStride && a.getStride() == b.getStride()) {
    return strided(a.stride, gcd<>(a.alignment, b.alignment));
  } else {
    return varying(gcd(a.getAlignmentGeneral(), b.getAlignmentGeneral()));
  }
}

std::string VectorShape::str() const {
  if (!isDefined()) {
    return "undef_shape";
  }

  std::stringstream ss;
  if (isVarying()) {
    ss << "varying";
  } else if (isUniform()) {
    ss << "uni";
  } else if (isContiguous()) {
    ss << "cont";
  } else {
    ss << "stride(" << stride << ")";
  }

  if (alignment > 1) {
    ss << ", alignment(" << alignment << ", " << getAlignmentGeneral() << ")";
  }

  return ss.str();
}
}
