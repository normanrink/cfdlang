
#ifndef __OPERATORS_EXPR_IMPL_HPP__
#define __OPERATORS_EXPR_IMPL_HPP__

#include <assert.h>

#include "Operators/ExprVisitor.h"
#include "Operators/Matrix.h"
#include "Operators/Operator.h"
#include "Operators/Scalar.h"


using namespace Operators;

template<typename S, typename T, typename Derived>
template<typename DerivedV>
void Binary<S, T, Derived>::visit(ExprVisitor<DerivedV> &v) const {
  getAsDerived().visit(v);
}

template<typename S, typename T>
template<typename DerivedV>
void Add<S, T>::visit(ExprVisitor<DerivedV> &v) const {
  v.visitAdd(*this);
}

template<typename S, typename T>
template<typename DerivedV>
void Sub<S, T>::visit(ExprVisitor<DerivedV> &v) const {
  v.visitSub(*this);
}

template<typename S, typename T>
template<typename DerivedV>
void SMul<S, T>::visit(ExprVisitor<DerivedV> &v) const {
  v.visitSMul(*this);
}

template<typename S, typename T>
template<typename DerivedV>
void Apply<S, T>::visit(ExprVisitor<DerivedV> &v) const {
  v.visitApply(*this);
}

template<typename T, typename Derived>
template<typename DerivedV>
void Unary<T, Derived>::visit(ExprVisitor<DerivedV> &v) const {
  getAsDerived().visit(v);
}

template<typename T>
template<typename DerivedV>
void Neg<T>::visit(ExprVisitor<DerivedV> &v) const {
  v.visitNeg(*this);
}


template<typename T>
const Expr<Neg<T>> Expr<T>::operator-() const {
  return Expr<Neg<T>>(Neg<T>(get()));
}

template<typename T>
template<typename S>
const Expr<Add<T, S>> Expr<T>::operator+(const Expr<S> &rhs) const {
  assert(this->getInDims() == rhs.getInDims()
         && this->getOutDims() == rhs.getOutDims());
  return Expr<Add<T, S>>(Add<T, S>(get(), rhs.get()));
}

template<typename T>
const Expr<Add<T, SConst>>
Expr<T>::operator+(const SConst &s) const {
  return operator+(Expr<SConst>(s));
}

template<typename T>
const Expr<Add<T, SVar>>
Expr<T>::operator+(const SVar &s) const {
  return operator+(Expr<SVar>(s));
}

template<typename T>
const Expr<Add<T, Operator<1, Matrix>>>
Expr<T>::operator+(const Matrix &m) const {
  return operator+(Expr<Operator<1, Matrix>>(m));
}

template<typename T>
const Expr<Add<T, Operator<1, Symmetric>>>
Expr<T>::operator+(const Symmetric &m) const {
  return operator+(Expr<Operator<1, Symmetric>>(m));
}

template<typename T>
const Expr<Add<T, Operator<1, Antisymmetric>>>
Expr<T>::operator+(const Antisymmetric &m) const {
  return operator+(Expr<Operator<1, Antisymmetric>>(m));
}

template<typename T>
const Expr<Add<T, Operator<1, Diagonal>>>
Expr<T>::operator+(const Diagonal &m) const {
  return operator+(Expr<Operator<1, Diagonal>>(m));
}

template<typename T>
const Expr<Add<T, Operator<1, Identity>>>
Expr<T>::operator+(const Identity &m) const {
  return operator+(Expr<Operator<1, Identity>>(m));
}

template<typename T>
template<int d, typename... DerivedMs>
const Expr<Add<T, Operator<d, DerivedMs...>>>
Expr<T>::operator+(const Operator<d, DerivedMs...> &op) const {
  return operator+(Expr<Operator<d, DerivedMs...>>(op));
}

template<typename T>
template<typename S>
const Expr<Sub<T, S>> Expr<T>::operator-(const Expr<S> &rhs) const {
  assert(this->getInDims() == rhs.getInDims()
         && this->getOutDims() == rhs.getOutDims());
  return Expr<Sub<T, S>>(Sub<T, S>(get(), rhs.get()));
}

template<typename T>
const Expr<Sub<T, SConst>>
Expr<T>::operator-(const SConst &s) const {
  return operator-(Expr<SConst>(s));
}

template<typename T>
const Expr<Sub<T, SVar>>
Expr<T>::operator-(const SVar &s) const {
  return operator-(Expr<SVar>(s));
}

template<typename T>
const Expr<Sub<T, Operator<1, Matrix>>>
Expr<T>::operator-(const Matrix &m) const {
  return operator-(Expr<Operator<1, Matrix>>(m));
}

template<typename T>
const Expr<Sub<T, Operator<1, Symmetric>>>
Expr<T>::operator-(const Symmetric &m) const {
  return operator-(Expr<Operator<1, Symmetric>>(m));
}

template<typename T>
const Expr<Sub<T, Operator<1, Antisymmetric>>>
Expr<T>::operator-(const Antisymmetric &m) const {
  return operator-(Expr<Operator<1, Antisymmetric>>(m));
}

template<typename T>
const Expr<Sub<T, Operator<1, Diagonal>>>
Expr<T>::operator-(const Diagonal &m) const {
  return operator-(Expr<Operator<1, Diagonal>>(m));
}

template<typename T>
const Expr<Sub<T, Operator<1, Identity>>>
Expr<T>::operator-(const Identity &m) const {
  return operator-(Expr<Operator<1, Identity>>(m));
}

template<typename T>
template<int d, typename... DerivedMs>
const Expr<Sub<T, Operator<d, DerivedMs...>>>
Expr<T>::operator-(const Operator<d, DerivedMs...> &op) const {
  return operator-(Expr<Operator<d, DerivedMs...>>(op));
}

template<typename T>
template<typename S>
const Expr<Apply<T, S>> Expr<T>::operator()(const Expr<S> &rhs) const {
  assert(this->getInDims() == rhs.getOutDims()
         // Cannot apply scalars:
         && !this->getInDims().empty());
  return Expr<Apply<T, S>>(Apply<T, S>(get(), rhs.get()));
}

template<typename T>
const Expr<Apply<T, Operator<1, Matrix>>>
Expr<T>::operator()(const Matrix &m) const {
  return operator()(Expr<Operator<1, Matrix>>(m));
}

template<typename T>
const Expr<Apply<T, Operator<1, Symmetric>>>
Expr<T>::operator()(const Symmetric &m) const {
  return operator()(Expr<Operator<1, Symmetric>>(m));
}

template<typename T>
const Expr<Apply<T, Operator<1, Antisymmetric>>>
Expr<T>::operator()(const Antisymmetric &m) const {
  return operator()(Expr<Operator<1, Antisymmetric>>(m));
}

template<typename T>
const Expr<Apply<T, Operator<1, Diagonal>>>
Expr<T>::operator()(const Diagonal &m) const {
  return operator()(Expr<Operator<1, Diagonal>>(m));
}

template<typename T>
const Expr<Apply<T, Operator<1, Identity>>>
Expr<T>::operator()(const Identity &m) const {
  return operator()(Expr<Operator<1, Identity>>(m));
}

template<typename T>
template<int d, typename... DerivedMs>
const Expr<Apply<T, Operator<d, DerivedMs...>>>
Expr<T>::operator()(const Operator<d, DerivedMs...> &op) const {
  return operator()(Expr<Operator<d, DerivedMs...>>(op));
}

template<typename T>
template<typename S>
const Expr<SMul<T, S>> Expr<T>::operator*(const Expr<S> &rhs) const {
  assert(this->getInDims().empty());
  return Expr<SMul<T, S>>(SMul<T, S>(get(), rhs.get()));
}

template<typename T>
const Expr<SMul<T, SConst>>
Expr<T>::operator*(const SConst &s) const {
  return operator*(Expr<SConst>(s));
}

template<typename T>
const Expr<SMul<T, SVar>>
Expr<T>::operator*(const SVar &s) const {
  return operator*(Expr<SVar>(s));
}

template<typename T>
const Expr<SMul<T, Operator<1, Matrix>>>
Expr<T>::operator*(const Matrix &m) const {
  return operator*(Expr<Operator<1, Matrix>>(m));
}

template<typename T>
const Expr<SMul<T, Operator<1, Symmetric>>>
Expr<T>::operator*(const Symmetric &m) const {
  return operator*(Expr<Operator<1, Symmetric>>(m));
}

template<typename T>
const Expr<SMul<T, Operator<1, Antisymmetric>>>
Expr<T>::operator*(const Antisymmetric &m) const {
  return operator*(Expr<Operator<1, Antisymmetric>>(m));
}

template<typename T>
const Expr<SMul<T, Operator<1, Diagonal>>>
Expr<T>::operator*(const Diagonal &m) const {
  return operator*(Expr<Operator<1, Diagonal>>(m));
}

template<typename T>
const Expr<SMul<T, Operator<1, Identity>>>
Expr<T>::operator*(const Identity &m) const {
  return operator*(Expr<Operator<1, Identity>>(m));
}

template<typename T>
template<int d, typename... DerivedMs>
const Expr<SMul<T, Operator<d, DerivedMs...>>>
Expr<T>::operator*(const Operator<d, DerivedMs...> &op) const {
  return operator*(Expr<Operator<d, DerivedMs...>>(op));
}

template<typename T>
template<typename DerivedV>
void Expr<T>::visit(ExprVisitor<DerivedV> &v) const {
  get().visit(v);
}

#endif /* __OPERATORS_EXPR_IMPL_HPP__ */
