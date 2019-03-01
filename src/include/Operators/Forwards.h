
#ifndef __OPERATORS_FORWARDS_HPP__
#define __OPERATORS_FORWARDS_HPP__

#include <map>
#include <vector>


#define EMIT_INDENT(indent, os) { os.width((indent)); \
                                  os << std::left << ""; \
                                  os.unsetf(std::ios::adjustfield); }
#define PER_LEVEL_INDENT (2)


namespace Operators {

template<int d, typename DerivedM, typename... DerivedMs> class Operator;

template<typename Derived> class MatrixBase;
class Matrix;
class Symmetric;
class Antisymmetric;
class Diagonal;
class Identity;

template<typename Derived> class Scalar;
class SConst;
class SVar;

template<typename S, typename T, typename Derived> class Binary;
template<typename S, typename T> class Add;
template<typename S, typename T> class Sub;
template<typename S, typename T> class SMul;
template<typename S, typename T> class Apply;

template<typename T, typename Derived> class Unary;
template<typename T> class Neg;

template<typename T> class Expr;

typedef std::vector<int> Dimensions;

enum ExprKind {
  EK_Scalar,
  EK_Matrix,
  EK_Operator,
  EK_Neg,
  EK_Add,
  EK_Sub,
  EK_Mul,
  EK_Apply,
  EK_COUNT
};

enum MatrKind {
  MK_Matrix,
  MK_Symmetric,
  MK_Antisymmetric,
  MK_Diagonal,
  MK_Identity,
  MK_COUNT
};

enum ScalKind {
  SK_Const,
  SK_Var,
  SK_COUNT
};

extern std::map<MatrKind, std::string> MatrLabel;
extern std::map<ScalKind, std::string> ScalLabel;
extern std::map<ExprKind, std::string> ExprLabel;

template<typename Derived> class ExprVisitor;

};

#endif /* __OPERATORS_FORWARDS_HPP__ */
