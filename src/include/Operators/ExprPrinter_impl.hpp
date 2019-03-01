
#ifndef __OPERATORS_EXPR_PRINTER_IMPL_HPP__
#define __OPERATORS_EXPR_PRINTER_IMPL_HPP__

#include "Operators/ExprPrinter.h"
#include "Operators/Operators.h"


using namespace Operators;

template<typename S, typename T>
void ExprPrinter::visitAdd(const Add<S, T> &expr) {
  expr.getLHS().visit(*this);
  os << " + ";
  expr.getRHS().visit(*this);
}

template<typename S, typename T>
void ExprPrinter::visitSub(const Sub<S, T> &expr)  {
  expr.getLHS().visit(*this);
  os << " - ";
  expr.getRHS().visit(*this);
}

template<typename S, typename T>
void ExprPrinter::visitSMul(const SMul<S, T> &expr) {
  const auto &lhs = expr.getLHS();
  const auto &rhs = expr.getRHS();

  if (lhs.getKind() == EK_Add || lhs.getKind() == EK_Sub) {
    os << "(";
    lhs.visit(*this);
    os << ")";
  } else {
    lhs.visit(*this);
  }

  os << "*";

  if (rhs.getKind() == EK_Add || rhs.getKind() == EK_Sub) {
    os << "(";
    rhs.visit(*this);
    os << ")";
  } else {
    rhs.visit(*this);
  }
}

template<typename S, typename T>
void ExprPrinter::visitApply(const Apply<S, T> &expr) {
  os << "(";
  expr.getLHS().visit(*this);
  os << ")(";
  expr.getRHS().visit(*this);
  os << ")";
}

void ExprPrinter::visitSConst(const SConst &expr) {
  os << expr.getC();
}

void ExprPrinter::visitSVar(const SVar &expr) {
  os << expr.getName();
}

void ExprPrinter::visitMatrix(const Matrix &expr)  {
  os << expr.getName();
}

void ExprPrinter::visitSymmetric(const Symmetric &expr) {
  os << expr.getName();
}

void ExprPrinter::visitAntisymmetric(const Antisymmetric &expr) {
  os << expr.getName();
}

void ExprPrinter::visitDiagonal(const Diagonal &expr)  {
  os << expr.getName();
}

void ExprPrinter::visitIdentity(const Identity &expr)  {
  os << expr.getName();
}

template<typename DerivedM>
void ExprPrinter::visitOperator(const Operator<1, DerivedM> &expr) {
  expr.getMatrix().visit(*this);
}

template<int d, typename... DerivedMs>
void ExprPrinter::visitOperator(const Operator<d, DerivedMs...> &expr) {
  expr.getFront().visit(*this);
  os << "*";
  expr.getEnd().visit(*this);
}

#endif /* __OPERATORS_EXPR_PRINTER_IMPL_HPP__ */
