
#ifndef __OPERATORS_EXPR_VISITOR_H__
#define __OPERATORS_EXPR_VISITOR_H__

#include "Operators/Forwards.h"


template<typename Derived>
class ExprVisitor {
  Derived &getAsDerived() {
    auto cast = static_cast<Derived *>(this);
    return *cast;
  }
public:
  template<typename S, typename T>
  void visitAdd(const Add<S, T> &expr) {
    getAsDerived().visitAdd(expr);
  }
  template<typename S, typename T>
  void visitSub(const Sub<S, T> &expr) {
    getAsDerived().visitSub(expr);
  }
  template<typename S, typename T>
  void visitSMul(const SMul<S, T> &expr) {
    getAsDerived().visitSMul(expr);
  }
  template<typename S, typename T>
  void visitApply(const Apply<S, T> &expr) {
    getAsDerived().visitApply(expr);
  }

  void visitSConst(const SConst &expr) {
    getAsDerived().visitSConst(expr);
  }
  void visitSVar(const SVar &expr) {
    getAsDerived().visitSVar(expr);
  }

  void visitMatrix(const Matrix &expr) {
    getAsDerived().visitMatrix(expr);
  }
  void visitSymmetric(const Symmetric &expr) {
    getAsDerived().visitSymmetric(expr);
  }
  void visitAntisymmetric(const Antisymmetric &expr) {
    getAsDerived().visitAntisymmetric(expr);
  }
  void visitDiagonal(const Diagonal &expr) {
    getAsDerived().visitDiagonal(expr);
  }

  void visitIdentity(const Identity &expr) {
    getAsDerived().visitIdentity(expr);
  }

  template<int d, typename... DerivedMs>
  void visitOperator(const Operator<d, DerivedMs...> &expr) {
    getAsDerived().visitOperator(expr);
  }
};

#endif /* __OPERATORS_EXPR_VISITOR_H__ */
