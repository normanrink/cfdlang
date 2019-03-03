
#ifndef __OPERATORS_EXPR_H__
#define __OPERATORS_EXPR_H__

#include <assert.h>

#include <ostream>

#include "Operators/Forwards.h"


namespace Operators {

template<typename S, typename T, typename Derived>
class Binary {
  const S lhs;
  const T rhs;

  const Derived &getAsDerived() const {
    auto cast = static_cast<const Derived *>(this);
    return *cast;
  }
protected:
  Binary() = delete;
  Binary(const S &lhs, const T &rhs) : lhs(lhs), rhs(rhs) {}

public:
  const S &getLHS() const { return lhs; }
  const T &getRHS() const { return rhs; }

  Dimensions getInDims() const { return getAsDerived().getInDims(); }
  Dimensions getOutDims() const { return getAsDerived().getOutDims(); }

  static ExprKind getKind() { return Derived::getKind(); }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << ExprLabel.at(getKind()) << "\n";
    getLHS().prettyPrint(indent+PER_LEVEL_INDENT, os);
    getRHS().prettyPrint(indent+PER_LEVEL_INDENT, os);
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};

template<typename S, typename T>
class Add : public Binary<S, T, Add<S, T>> {
  using Base = Binary<S, T, Add<S, T>>;

public:
  Add(const S &lhs, const T &rhs) : Base(lhs, rhs) {}

  Dimensions getInDims() const {
    assert(Base::getLHS().getInDims() == Base::getLHS().getInDims());
    return Base::getLHS().getInDims();
  }
  Dimensions getOutDims() const {
    assert(Base::getLHS().getOutDims() == Base::getRHS().getOutDims());
    return Base::getLHS().getOutDims();
  }

  static ExprKind getKind() { return EK_Add; }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};

template<typename S, typename T>
class Sub : public Binary<S, T, Sub<S, T>> {
  using Base = Binary<S, T, Sub<S, T>>;

public:
  Sub(const S &lhs, const T &rhs) : Base(lhs, rhs) {}

  Dimensions getInDims() const {
    assert(Base::getLHS().getInDims() == Base::getLHS().getInDims());
    return Base::getLHS().getInDims();
  }
  Dimensions getOutDims() const {
    assert(Base::getLHS().getOutDims() == Base::getRHS().getOutDims());
    return Base::getLHS().getOutDims();
  }

  static ExprKind getKind() { return EK_Sub; }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};

template<typename S, typename T>
class SMul : public Binary<S, T, SMul<S, T>> {
  using Base = Binary<S, T, SMul<S, T>>;

public:
  SMul(const S &lhs, const T &rhs) : Base(lhs, rhs) {}

  Dimensions getInDims() const {
    assert(Base::getLHS().getInDims().empty());
    return Base::getRHS().getInDims();
  }
  Dimensions getOutDims() const {
    assert(Base::getLHS().getOutDims().empty());
    return Base::getRHS().getOutDims();
  }

  static ExprKind getKind() { return EK_Mul; }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};


template<typename S, typename T>
class Apply : public Binary<S, T, Apply<S, T>> {
  using Base = Binary<S, T, Apply<S, T>>;

public:
  Apply(const S &lhs, const T &rhs) : Base(lhs, rhs) {}

  Dimensions getInDims() const {
    assert(Base::getLHS().getInDims() == Base::getRHS().getOutDims());
    return Base::getRHS().getInDims();;
  }
  Dimensions getOutDims() const {
    assert(Base::getLHS().getInDims() == Base::getRHS().getOutDims());
    return Base::getLHS().getOutDims();
  }

  static ExprKind getKind() { return EK_Apply; }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};


template<typename T, typename Derived>
class Unary {
  const T arg;

  const Derived &getAsDerived() const {
    auto cast = static_cast<const Derived *>(this);
    return *cast;
  }
public:
  Unary() = delete;
  Unary(const T &arg) : arg(arg) {}

  const T &getArg() const { return arg; }

  Dimensions getInDims() const { return getAsDerived().getInDims(); }
  Dimensions getOutDims() const { return getAsDerived().getOutDims(); }

  static ExprKind getKind() { return Derived::getKind(); }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << ExprLabel.at(getKind()) << "\n";
    getArg().prettyPrint(indent+PER_LEVEL_INDENT, os);
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};

template<typename T>
class Neg : public Unary<T, Neg<T>> {
  using Base = Unary<T, Neg<T>>;

public:
  Neg(const T &arg) : Base(arg) {}

  Dimensions getInDims() const { return Base::getArg().getInDims(); }
  Dimensions getOutDims() const { return Base::getArg().getOutDims(); }

  static ExprKind getKind() { return EK_Neg; }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};


template<typename T>
class Expr {
  const T t;

public:
  Expr(const Expr<T> &e) : t(e.get()) {}

  template<typename... Ts>
  Expr(const Ts &...args) : t(args...) {}

  const T &get() const { return t; }

  Dimensions getInDims() const { return get().getInDims(); }

  Dimensions getOutDims() const { return get().getOutDims(); }

  static ExprKind getKind() { return T::getKind(); }

  void prettyPrint(int indent, std::ostream &os) const {
    get().prettyPrint(indent, os);
  }

  const Expr<Neg<T>> operator-() const;

  template<typename S>
  const Expr<Add<T, S>> operator+(const Expr<S> &rhs) const ;

  const Expr<Add<T, SConst>>
  operator+(const SConst &s) const;

  const Expr<Add<T, SVar>>
  operator+(const SVar &s) const;

  const Expr<Add<T, Operator<1, Matrix>>>
  operator+(const Matrix &m) const;

  const Expr<Add<T, Operator<1, Symmetric>>>
  operator+(const Symmetric &m) const;

  const Expr<Add<T, Operator<1, Antisymmetric>>>
  operator+(const Antisymmetric &m) const;

  const Expr<Add<T, Operator<1, Diagonal>>>
  operator+(const Diagonal &m) const;

  const Expr<Add<T, Operator<1, Identity>>>
  operator+(const Identity &m) const;

  template<int d, typename... DerivedMs>
  const Expr<Add<T, Operator<d, DerivedMs...>>>
  operator+(const Operator<d, DerivedMs...> &op) const;

  template<typename S>
  const Expr<Sub<T, S>> operator-(const Expr<S> &rhs) const;

  const Expr<Sub<T, SConst>>
  operator-(const SConst &s) const;

  const Expr<Sub<T, SVar>>
  operator-(const SVar &s) const;

  const Expr<Sub<T, Operator<1, Matrix>>>
  operator-(const Matrix &m) const;

  const Expr<Sub<T, Operator<1, Symmetric>>>
  operator-(const Symmetric &m) const;

  const Expr<Sub<T, Operator<1, Antisymmetric>>>
  operator-(const Antisymmetric &m) const;

  const Expr<Sub<T, Operator<1, Diagonal>>>
  operator-(const Diagonal &m) const;

  const Expr<Sub<T, Operator<1, Identity>>>
  operator-(const Identity &m) const;

  template<int d, typename... DerivedMs>
  const Expr<Sub<T, Operator<d, DerivedMs...>>>
  operator-(const Operator<d, DerivedMs...> &op) const;

  template<typename S>
  const Expr<Apply<T, S>> operator()(const Expr<S> &rhs) const;

  // Cannot apply any expression to a scalar.

  const Expr<Apply<T, Operator<1, Matrix>>>
  operator()(const Matrix &m) const;

  const Expr<Apply<T, Operator<1, Symmetric>>>
  operator()(const Symmetric &m) const;

  const Expr<Apply<T, Operator<1, Antisymmetric>>>
  operator()(const Antisymmetric &m) const;

  const Expr<Apply<T, Operator<1, Diagonal>>>
  operator()(const Diagonal &m) const;

  const Expr<Apply<T, Operator<1, Identity>>>
  operator()(const Identity &m) const;

  template<int d, typename... DerivedMs>
  const Expr<Apply<T, Operator<d, DerivedMs...>>>
  operator()(const Operator<d, DerivedMs...> &op) const;

  template<int r>
  const Expr<Apply<T, Tensor<r>>>
  operator()(const Tensor<r> &t) const;

  template<typename S>
  const Expr<SMul<T, S>> operator*(const Expr<S> &rhs) const;

  const Expr<SMul<T, SConst>>
  operator*(const SConst &s) const;

  const Expr<SMul<T, SVar>>
  operator*(const SVar &s) const;

  const Expr<SMul<T, Operator<1, Matrix>>>
  operator*(const Matrix &m) const;

  const Expr<SMul<T, Operator<1, Symmetric>>>
  operator*(const Symmetric &m) const;

  const Expr<SMul<T, Operator<1, Antisymmetric>>>
  operator*(const Antisymmetric &m) const;

  const Expr<SMul<T, Operator<1, Diagonal>>>
  operator*(const Diagonal &m) const;

  const Expr<SMul<T, Operator<1, Identity>>>
  operator*(const Identity &m) const;

  template<int d, typename... DerivedMs>
  const Expr<SMul<T, Operator<d, DerivedMs...>>>
  operator*(const Operator<d, DerivedMs...> &op) const;

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};

};

#include "Operators/Expr_impl.hpp"

#endif /* __OPERATORS_EXPR_H__ */
