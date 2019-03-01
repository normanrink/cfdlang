
#include <map>
#include <string>


#include "Operators/Forwards.h"


using namespace Operators;

std::map<ExprKind, std::string> ExprLabel = {
  { EK_Scalar, "Scalar" },
  { EK_Matrix, "Matrix" },
  { EK_Operator, "Operator" },
  { EK_Neg, "Neg" },
  { EK_Add, "Add" },
  { EK_Sub, "Sub" },
  { EK_Mul, "Mul" },
  { EK_Apply, "Apply" }
};

std::map<MatrKind, std::string> MatrLabel = {
  { MK_Matrix, "Matrix" },
  { MK_Symmetric, "Symmetric" },
  { MK_Antisymmetric, "Antisymmetric" },
  { MK_Diagonal, "Diagonal" },
  { MK_Identity, "Identity" },
};

std::map<ScalKind, std::string> ScalLabel = {
  { SK_Const, "Const" },
  { SK_Var, "Var" }
};
