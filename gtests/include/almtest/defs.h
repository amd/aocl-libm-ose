#ifndef __DEFS_H__
#define __DEFS_H__

/*
 * Copyright (C) 2019-2020, AMD. All rights are reserved
 *
 * Author: Prem Mallappa<pmallapp@amd.com>
 */

#pragma once

#include <iostream>

namespace ALM {

// IEEE 754-2008 Rounding Modes
enum class RoundingMode {
  E_NearestEven,  // round to nearest; TiesToEven
  E_NearestAway,  // round to nearest; TiesToAway
  E_Zero,         // round TowardZero
  E_NegativeInf,  // round TowardNegative
  E_PositiveInf,  // round TowardPositive

  E_MAX
};

enum class FloatType {
  E_Real,
  E_Complex,
  E_Other,

  E_MAX
};

inline std::ostream& operator<<(std::ostream& os, const FloatType& w) {
  switch (w) {
    case FloatType::E_Complex:
      os << "Complex ";
      break;
    case FloatType::E_Real:
      os << "Real";
      break;
    default:
      os << "<Invalid FloatType>";
      break;
  }
  return os;
}

enum class FloatWidth {
  E_F8 = 1 << 0,    // Byte precision
  E_F16 = 1 << 1,   // Half precision
  E_F32 = 1 << 2,   // Single precision
  E_F64 = 1 << 3,   // Double precision
  E_F80 = 1 << 4,   // Extended precision
  E_F128 = 1 << 5,  // Quad precision
  E_F256 = 1 << 6,  // octuple precision
  E_Other = 1 << 17,
  E_MAX,

  // TODO : Need to contain all possible FloatWidthes
  E_ALL = (E_F32 | E_F64)
};

inline std::ostream& operator<<(std::ostream& os, FloatWidth& w) {
  switch (w) {
    case FloatWidth::E_F8:
      break;
    case FloatWidth::E_F16:
      os << "F16";
      break;
    case FloatWidth::E_F32:
      os << "F32";
      break;
    case FloatWidth::E_F64:
      os << "F64";
      break;
    case FloatWidth::E_F128:
      os << "F128";
      break;
    case FloatWidth::E_F256:
      os << "F256";
      break;
    default:
      os << "ALL";
      break;
  }
  return os;
}

#define NUMOFDATATYPES 2
#define NUMOFFLOATTYPES 4

enum class FloatQuantity {
  E_Scalar = 1 << 0,     // Scalar
  E_Vector_2 = 1 << 1,   // Vector - 2 elem
  E_Vector_4 = 1 << 2,   // Vector - 4 elem
  E_Vector_8 = 1 << 3,   // Vector - 8 elem
  E_Vector_16 = 1 << 4,  // Vector - 16 elem
  E_Other = 1 << 5,
  E_MAX,

  // TODO : Need to contain all possible FloatQuantities
  E_All = (E_Scalar | E_Vector_2 | E_Vector_4 | E_Vector_8 | E_Vector_16)
};

inline std::ostream& operator<<(std::ostream& os, FloatQuantity& q) {
  switch (q) {
    case FloatQuantity::E_Scalar:
      os << "Scalar";
      break;
    case FloatQuantity::E_Vector_2:
      os << "Vector(2)";
      break;
    case FloatQuantity::E_Vector_4:
      os << "Vector(4)";
      break;
    case FloatQuantity::E_Vector_8:
      os << "Vector(8)";
      break;
    case FloatQuantity::E_Vector_16:
      os << "Vector(16)";
      break;
    default:
      os << "All";
      break;
  }
  return os;
}

enum class RangeType {
  E_Simple,
  E_Random,
  E_Linear,

  E_MAX
};

inline std::ostream& operator<<(std::ostream& os, RangeType& rt) {
  switch (rt) {
    case RangeType::E_Simple:
      os << "Simple";
      break;
    case RangeType::E_Random:
      os << "Random";
      break;
    case RangeType::E_Linear:
      os << "Linear";
      break;
    default:
      os << "Unknown";
      break;
  }
  return os;
}

struct Range {
  std::pair<double, double> r_range;
  RangeType r_type;
};

enum class TestType {
  E_Accuracy    = 1 << 1,
  E_Conformance = 1 << 2,
  E_SpecialCase = 1 << 3,
  E_CornerCase  = 1 << 4,  
  E_Performance = 1 << 5,

  E_MAX
};

inline std::ostream& operator<<(std::ostream& os, TestType& cat) {
  switch (cat) {
    case TestType::E_Accuracy:
      os << "ACCURACY";
      break;
    case TestType::E_Conformance:
      os << "CONFORMANCE";
      break;
    case TestType::E_SpecialCase:
      os << "SPECIAL";
      break;
    case TestType::E_Performance:
      os << "PERFORMANCE";
      break;      
    default:
      os << "UKNOWN";
      break;
  }
  return os;
}

enum class TestFailType {
  E_None = (1 << 1),                   //
  E_AssertFailure = (1 << 2),          // an assertion has failed
  E_Exception = (1 << 3),              //
  E_Crash = (1 << 4),                  // a crash...
  E_TooManyAssertFailures = (1 << 5),  //
  E_Timedout = (1 << 6),               //
  E_ExpectedToPass = (1 << 7),         //
  E_ExpectedToFail = (1 << 8),         //

  E_MAX
};
} // namespace ALM
#endif
