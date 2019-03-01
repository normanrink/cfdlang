
#ifndef __OPERATORS_EXPR_PRINTER_H__
#define __OPERATORS_EXPR_PRINTER_H__

#include <ostream>

#include "Operators/ExprVisitor.h"


class ExprPrinter : public ExprVisitor<ExprPrinter> {
  std::ostream &os;

public:
  ExprPrinter(std::ostream &os) : os(os) {}

  template<typename S, typename T>
  void visitAdd(const Add<S, T> &expr);

  template<typename S, typename T>
  void visitSub(const Sub<S, T> &expr);

  template<typename S, typename T>
  void visitSMul(const SMul<S, T> &expr);

  template<typename S, typename T>
  void visitApply(const Apply<S, T> &expr);

  void visitSConst(const SConst &expr);
  void visitSVar(const SVar &expr);

  void visitMatrix(const Matrix &expr);
  void visitSymmetric(const Symmetric &expr);
  void visitAntisymmetric(const Antisymmetric &expr);
  void visitDiagonal(const Diagonal &expr);
  void visitIdentity(const Identity &expr);

  template<typename DerivedM>
  void visitOperator(const Operator<1, DerivedM> &expr);

  template<int d, typename... DerivedMs>
  void visitOperator(const Operator<d, DerivedMs...> &expr);
};

#include "Operators/ExprPrinter_impl.hpp"

#endif /* __OPERATORS_EXPR_PRINTER_H__ */
