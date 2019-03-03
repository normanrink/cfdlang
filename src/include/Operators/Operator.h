
#ifndef __OPERATORS_OPERATOR_H__
#define __OPERATORS_OPERATOR_H__

#include <assert.h>

#include <ostream>

#include "Operators/Forwards.h"


namespace Operators {

template<typename DerivedM>
class Operator<1, DerivedM> {
  using This = Operator<1, DerivedM>;

  const DerivedM m;

public:
  Operator(const DerivedM &m) : m(m) {}

  Operator(const This &op) : m(op.getMatrix()) {}

  static int getSize() { return 1; }

  const DerivedM &getMatrix() const { return m; }
  const DerivedM &getEnd() const { return getMatrix(); }

  int getM(int i) const {
    assert(i == 0);
    return getMatrix().getM();
  }

  int getN(int i) const {
    assert(i == 0);
    return getMatrix().getN();
  }

  Dimensions getInDims() const {
    Dimensions ds = { getMatrix().getN() };
    return ds;
  }
  Dimensions getOutDims() const {
    Dimensions ds = { getMatrix().getM() };
    return ds;
  }

  static ExprKind getKind() { return EK_Operator; }

  MatrKind getMKind(int i) const {
    assert(i == 0);
    return getMatrix().getMKind();
  }

  void prettyPrintMatrices(std::ostream &os) const {
    os << MatrLabel.at(m.getMKind()) << "<";
    os << getMatrix().getM() << "," << getMatrix().getN() << ">";
  }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << "Operator<" << getSize() << "; ";
    prettyPrintMatrices(os);
    os << ">\n";
  }

  const Operator<2, Matrix, DerivedM>
  operator*(const Matrix &m) const;

  const Operator<2, Symmetric, DerivedM>
  operator*(const Symmetric &m) const;

  const Operator<2, Antisymmetric, DerivedM>
  operator*(const Antisymmetric &m) const;

  const Operator<2, Diagonal, DerivedM>
  operator*(const Diagonal &m) const;

  const Operator<2, Identity, DerivedM>
  operator*(const Identity &m) const;

  const Expr<Neg<This>> operator-() const;

  template<typename S>
  const Expr<Add<This, S>> operator+(const Expr<S> &rhs) const;

  template<typename DerivedOtherM>
  const Expr<Add<This, Operator<1, DerivedOtherM>>>
  operator+(const Operator<1, DerivedOtherM> &op) const;

  template<typename S>
  const Expr<Sub<This, S>> operator-(const Expr<S> &rhs) const;

  template<typename DerivedOtherM>
  const Expr<Sub<This, Operator<1, DerivedOtherM>>>
  operator-(const Operator<1, DerivedOtherM> &op) const;

  template<typename S>
  const Expr<Apply<This, S>> operator()(const Expr<S> &rhs) const;

  template<typename DerivedOtherM>
  const Expr<Apply<This, Operator<1, DerivedOtherM>>>
  operator()(const Operator<1, DerivedOtherM> &op) const;

  template<int r>
  const Expr<Apply<This, Tensor<r>>>
  operator()(const Tensor<r> &t) const;

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};


template<int d, typename DerivedM, typename... DerivedMs>
class Operator : public Operator<d-1, DerivedMs...> {
  using This = Operator<d, DerivedM, DerivedMs...>;
  using Base = Operator<d-1, DerivedMs...>;

  const DerivedM m;

  const Base &getAsBaseOp() const {
    auto cast = static_cast<const Base *>(this);
    return *cast;
  }

public:
  Operator(const Base &op, const DerivedM &m) : Base(op), m(m) {
    static_assert(d > 1, "");
  }
  Operator(const This &op) : Base(op.getAsBaseOp()), m(op.getMatrix()) {
    static_assert(d > 1, "");
  }

  static int getSize() { return d; }

  const DerivedM &getMatrix() const { return m; }
  const DerivedM &getEnd() const { return getMatrix(); }

  const Operator<d-1, DerivedMs...> &getFront() const {
    return getAsBaseOp();
  }

  int getM(int i) const {
    if (i == (d-1)) { return getMatrix().getM(); }
    else { return getAsBaseOp().getM(i); }
  }

  int getN(int i) const {
    if (i == (d-1)) { return getMatrix().getN(); }
    else { return getAsBaseOp().getN(i); }
  }

  Dimensions getInDims() const {
    Dimensions ds = getAsBaseOp().getInDims();
    ds.push_back(getMatrix().getN());
    return ds;
  }
  Dimensions getOutDims() const {
    Dimensions ds = getAsBaseOp().getOutDims();
    ds.push_back(getMatrix().getM());
    return ds;
  }

  MatrKind getMKind(int i) const {
    if (i == (d-1)) {
      return getMatrix().getMKind();
    } else {
      return getAsBaseOp().getMKind(i-1);
    }
  }

  static ExprKind getKind() { return EK_Operator; }

  void prettyPrintMatrices(std::ostream &os) const {
    getAsBaseOp().prettyPrintMatrices(os);
    os << ", ";
    os << MatrLabel.at(m.getMKind()) << "<";
    os << getMatrix().getM() << "," << getMatrix().getN() << ">";
  }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << "Operator<" << getSize() << "; ";
    prettyPrintMatrices(os);
    os << ">\n";
  }

  const Operator<d+1, Matrix, DerivedM, DerivedMs...>
  operator*(const Matrix &m) const;

  const Operator<d+1, Symmetric, DerivedM, DerivedMs...>
  operator*(const Symmetric &m) const;

  const Operator<d+1, Antisymmetric, DerivedM, DerivedMs...>
  operator*(const Antisymmetric &m) const;

  const Operator<d+1, Diagonal, DerivedM, DerivedMs...>
  operator*(const Diagonal &m) const;

  const Operator<d+1, Identity, DerivedM, DerivedMs...>
  operator*(const Identity &m) const;

  const Expr<Neg<This>> operator-() const;

  template<typename S>
  const Expr<Add<This, S>> operator+(const Expr<S> &rhs) const;

  template<typename... DerivedOtherMs>
  const Expr<Add<This, Operator<d, DerivedOtherMs...>>>
  operator+(const Operator<d, DerivedOtherMs...> &op) const;

  template<typename S>
  const Expr<Sub<This, S>> operator-(const Expr<S> &rhs) const;

  template<typename... DerivedOtherMs>
  const Expr<Sub<This, Operator<d, DerivedOtherMs...>>>
  operator-(const Operator<d, DerivedOtherMs...> &op) const;

  template<typename S>
  const Expr<Apply<This, S>> operator()(const Expr<S> &rhs) const;

  template<typename... DerivedOtherMs>
  const Expr<Apply<This, Operator<d, DerivedOtherMs...>>>
  operator()(const Operator<d, DerivedOtherMs...> &op) const;

  template<int r>
  const Expr<Apply<This, Tensor<r>>>
  operator()(const Tensor<r> &t) const;

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};

};

#include "Operators/Operator_impl.hpp"

#endif /* __OPERATORS_OPERATOR_H__ */
