
#ifndef __OPERATORS_SCALAR_H__
#define __OPERATORS_SCALAR_H__

#include <ostream>
#include <string>

#include "Operators/Forwards.h"


template<typename Derived>
class Scalar {
  using This = Scalar<Derived>;

  const std::string name;

  const Derived &getAsDerivedS() const {
    auto cast = static_cast<const Derived *>(this);
    return *cast;
  }
public:
  Scalar() = delete;
  Scalar(const std::string &name) : name(name) {}
  Scalar(const std::string &name, double c) : name(name) {}
  Scalar(const This &s) : name(s.getName()) {}

  const std::string &getName() const { return name; }

  Dimensions getInDims() const { return Dimensions(); }
  Dimensions getOutDims() const { return Dimensions(); }

  static ExprKind getKind() { return EK_Scalar; }

  static ScalKind getSKind() { return Derived::getSKind(); }

  void prettyPrint(int indent, std::ostream &os) const {
    getAsDerivedS().prettyPrint(indent, os);
  }

  const Expr<Neg<Derived>> operator-() const;

  template<typename S>
  const Expr<Add<Derived, S>> operator+(const Expr<S> &rhs) const;

  const Expr<Add<Derived, SConst>>
  operator+(const SConst &s) const;

  const Expr<Add<Derived, SVar>>
  operator+(const SVar &s) const;

  template<typename S>
  const Expr<Sub<Derived, S>> operator-(const Expr<S> &rhs) const;

  const Expr<Sub<Derived, SConst>>
  operator-(const SConst &s) const;

  const Expr<Sub<Derived, SVar>>
  operator-(const SVar &s) const;

  template<typename S>
  const Expr<SMul<Derived, S>> operator*(const Expr<S> &rhs) const;

  const Expr<SMul<Derived, SConst>>
  operator*(const SConst &s) const;

  const Expr<SMul<Derived, SVar>>
  operator*(const SVar &s) const;

  const Expr<SMul<Derived, Operator<1, Matrix>>>
  operator*(const Matrix &m) const;

  const Expr<SMul<Derived, Operator<1, Symmetric>>>
  operator*(const Symmetric &m) const;

  const Expr<SMul<Derived, Operator<1, Antisymmetric>>>
  operator*(const Antisymmetric &m) const;

  const Expr<SMul<Derived, Operator<1, Diagonal>>>
  operator*(const Diagonal &m) const;

  const Expr<SMul<Derived, Operator<1, Identity>>>
  operator*(const Identity &m) const;

  template<int d, typename... DerivedOtherMs>
  const Expr<SMul<Derived, Operator<d, DerivedOtherMs...>>>
  operator*(const Operator<d, DerivedOtherMs...> &op) const;

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};

class SConst : public Scalar<SConst> {
  using Base = Scalar<SConst>;

  const double c_;

public:
  SConst() = delete;
  SConst(const std::string &name, double c) : Base(name), c_(c) {}
  SConst(double c) : SConst("", c) {}
  SConst(const SConst &c) : SConst(c.getName(), c.getC()) {}

  double getC() const { return c_; }

  static ScalKind getSKind() { return SK_Const; }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << ScalLabel.at(getSKind()) << "<" << getC() << ">\n";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitSConst(*this);
  }
};

class SVar : public Scalar<SVar> {
  using Base = Scalar<SVar>;

public:
  SVar() = delete;
  SVar(const std::string &name) : Base(name) {}
  SVar(const SVar &s) : Base(s) {}

  static ScalKind getSKind() { return SK_Var; }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << ScalLabel.at(getSKind()) << "\n";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitSVar(*this);
  }
};

#include "Operators/Scalar_impl.hpp"

#endif /* __OPERATORS_SCALAR_H__ */
