
#ifndef __OPERATORS_MATRIX_IMPL_HPP__
#define __OPERATORS_MATRIX_IMPL_HPP__


#include "Operators/Operator.h"


using namespace Operators;

template<typename Derived>
const Operator<2, Matrix, Derived>
MatrixBase<Derived>::operator*(const Matrix &m) const {
  return Operator<1, Derived>(getAsDerivedM()) * m;
}

template<typename Derived>
const Operator<2, Symmetric, Derived>
MatrixBase<Derived>::operator*(const Symmetric &m) const {
  return Operator<1, Derived>(getAsDerivedM()) * m;
}

template<typename Derived>
const Operator<2, Antisymmetric, Derived>
MatrixBase<Derived>::operator*(const Antisymmetric &m) const {
  return Operator<1, Derived>(getAsDerivedM()) * m;
}

template<typename Derived>
const Operator<2, Diagonal, Derived>
MatrixBase<Derived>::operator*(const Diagonal &m) const {
  return Operator<1, Derived>(getAsDerivedM()) * m;
}

template<typename Derived>
const Operator<2, Identity, Derived>
MatrixBase<Derived>::operator*(const Identity &m) const {
  return Operator<1, Derived>(getAsDerivedM()) * m;
}

template<typename Derived>
const Expr<Neg<Operator<1, Derived>>>
MatrixBase<Derived>::operator-() const {
  return Expr<Operator<1, Derived>>(getAsDerivedM()).operator+();
}

template<typename Derived>
template<typename S>
const Expr<Add<Operator<1, Derived>, S>>
MatrixBase<Derived>::operator+(const Expr<S> &rhs) const {
  return Expr<Operator<1, Derived>>(getAsDerivedM()).operator+(rhs);
}

template<typename Derived>
const Expr<Add<Operator<1, Derived>, Operator<1, Matrix>>>
MatrixBase<Derived>::operator+(const Matrix &m) const {
  return getAsDerivedM().operator+(Expr<Operator<1, Matrix>>(m));
}

template<typename Derived>
const Expr<Add<Operator<1, Derived>, Operator<1, Symmetric>>>
MatrixBase<Derived>::operator+(const Symmetric &m) const {
  return getAsDerivedM().operator+(Expr<Operator<1, Symmetric>>(m));
}

template<typename Derived>
const Expr<Add<Operator<1, Derived>, Operator<1, Antisymmetric>>>
MatrixBase<Derived>::operator+(const Antisymmetric &m) const {
  return getAsDerivedM().operator+(Expr<Operator<1, Antisymmetric>>(m));
}

template<typename Derived>
const Expr<Add<Operator<1, Derived>, Operator<1, Diagonal>>>
MatrixBase<Derived>::operator+(const Diagonal &m) const {
  return getAsDerivedM().operator+(Expr<Operator<1, Diagonal>>(m));
}

template<typename Derived>
const Expr<Add<Operator<1, Derived>, Operator<1, Identity>>>
MatrixBase<Derived>::operator+(const Identity &m) const {
  return getAsDerivedM().operator+(Expr<Operator<1, Identity>>(m));
}

template<typename Derived>
template<typename S>
const Expr<Sub<Operator<1, Derived>, S>>
MatrixBase<Derived>::operator-(const Expr<S> &rhs) const {
  return Expr<Operator<1, Derived>>(getAsDerivedM()).operator-(rhs);
}

template<typename Derived>
const Expr<Sub<Operator<1, Derived>, Operator<1, Matrix>>>
MatrixBase<Derived>::operator-(const Matrix &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Matrix>>(m));
}

template<typename Derived>
const Expr<Sub<Operator<1, Derived>, Operator<1, Symmetric>>>
MatrixBase<Derived>::operator-(const Symmetric &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Symmetric>>(m));
}

template<typename Derived>
const Expr<Sub<Operator<1, Derived>, Operator<1, Antisymmetric>>>
MatrixBase<Derived>::operator-(const Antisymmetric &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Antisymmetric>>(m));
}

template<typename Derived>
const Expr<Sub<Operator<1, Derived>, Operator<1, Diagonal>>>
MatrixBase<Derived>::operator-(const Diagonal &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Diagonal>>(m));
}

template<typename Derived>
const Expr<Sub<Operator<1, Derived>, Operator<1, Identity>>>
MatrixBase<Derived>::operator-(const Identity &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Identity>>(m));
}

template<typename Derived>
template<typename S>
const Expr<Apply<Operator<1, Derived>, S>>
MatrixBase<Derived>::operator()(const Expr<S> &rhs) const {
  return Expr<Operator<1, Derived>>(getAsDerivedM()).operator()(rhs);
}

template<typename Derived>
const Expr<Apply<Operator<1, Derived>, Operator<1, Matrix>>>
MatrixBase<Derived>::operator()(const Matrix &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Matrix>>(m));
}

template<typename Derived>
const Expr<Apply<Operator<1, Derived>, Operator<1, Symmetric>>>
MatrixBase<Derived>::operator()(const Symmetric &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Symmetric>>(m));
}

template<typename Derived>
const Expr<Apply<Operator<1, Derived>, Operator<1, Antisymmetric>>>
MatrixBase<Derived>::operator()(const Antisymmetric &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Antisymmetric>>(m));
}

template<typename Derived>
const Expr<Apply<Operator<1, Derived>, Operator<1, Diagonal>>>
MatrixBase<Derived>::operator()(const Diagonal &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Diagonal>>(m));
}

template<typename Derived>
const Expr<Apply<Operator<1, Derived>, Operator<1, Identity>>>
MatrixBase<Derived>::operator()(const Identity &m) const {
  return getAsDerivedM().operator-(Expr<Operator<1, Identity>>(m));
}

template<typename Derived>
template<typename DerivedV>
void MatrixBase<Derived>::visit(ExprVisitor<DerivedV> &v) const {
  getAsDerivedM().visit(v);
}

#endif /* __OPERATORS_MATRIX_IMPL_HPP__ */
