
#ifndef __OPERATORS_SCALAR_IMPL_HPP__
#define __OPERATORS_SCALAR_IMPL_HPP__


template<typename Derived>
const Expr<Neg<Derived>>
Scalar<Derived>::operator-() const {
  return Expr<Derived>(getAsDerivedS()).operator-();
}

template<typename Derived>
template<typename S>
const Expr<Add<Derived, S>>
Scalar<Derived>::operator+(const Expr<S> &rhs) const {
  return Expr<Derived>(getAsDerivedS()).operator+(rhs);
}

template<typename Derived>
const Expr<Add<Derived, SConst>>
Scalar<Derived>::operator+(const SConst &s) const {
  return operator+(Expr<SConst>(s));
}

template<typename Derived>
const Expr<Add<Derived, SVar>>
Scalar<Derived>::operator+(const SVar &s) const {
  return operator+(Expr<SVar>(s));
}

template<typename Derived>
template<typename S>
const Expr<Sub<Derived, S>>
Scalar<Derived>::operator-(const Expr<S> &rhs) const {
  return Expr<Derived>(getAsDerivedS()).operator-(rhs);
}

template<typename Derived>
const Expr<Sub<Derived, SConst>>
Scalar<Derived>::operator-(const SConst &s) const {
  return operator-(Expr<SConst>(s));
}

template<typename Derived>
const Expr<Sub<Derived, SVar>>
Scalar<Derived>::operator-(const SVar &s) const {
  return operator-(Expr<SVar>(s));
}

template<typename Derived>
template<typename S>
const Expr<SMul<Derived, S>>
Scalar<Derived>::operator*(const Expr<S> &rhs) const {
  return Expr<Derived>(getAsDerivedS()).operator*(rhs);
}

template<typename Derived>
const Expr<SMul<Derived, SConst>>
Scalar<Derived>::operator*(const SConst &s) const {
  return operator*(Expr<SConst>(s));
}

template<typename Derived>
const Expr<SMul<Derived, SVar>>
Scalar<Derived>::operator*(const SVar &s) const {
  return operator*(Expr<SVar>(s));
}

template<typename Derived>
const Expr<SMul<Derived, Operator<1, Matrix>>>
Scalar<Derived>::operator*(const Matrix &m) const {
  return operator*(Expr<Operator<1, Matrix>>(m));
}

template<typename Derived>
const Expr<SMul<Derived, Operator<1, Symmetric>>>
Scalar<Derived>::operator*(const Symmetric &m) const {
  return operator*(Expr<Operator<1, Symmetric>>(m));
}

template<typename Derived>
const Expr<SMul<Derived, Operator<1, Antisymmetric>>>
Scalar<Derived>::operator*(const Antisymmetric &m) const {
  return operator*(Expr<Operator<1, Antisymmetric>>(m));
}

template<typename Derived>
const Expr<SMul<Derived, Operator<1, Diagonal>>>
Scalar<Derived>::operator*(const Diagonal &m) const  {
  return operator*(Expr<Operator<1, Diagonal>>(m));
}

template<typename Derived>
const Expr<SMul<Derived, Operator<1, Identity>>>
Scalar<Derived>::operator*(const Identity &m) const {
  return operator*(Expr<Operator<1, Identity>>(m));
}

template<typename Derived>
template<int d, typename... DerivedOtherMs>
const Expr<SMul<Derived, Operator<d, DerivedOtherMs...>>>
Scalar<Derived>::operator*(const Operator<d, DerivedOtherMs...> &op) const {
  return operator*(Expr<Operator<d, DerivedOtherMs...>>(op));
}

template<typename Derived>
template<typename DerivedV>
void Scalar<Derived>::visit(ExprVisitor<DerivedV> &v) const {
  getAsDerivedS().visit(v);
}

#endif /* __OPERATORS_SCALAR_IMPL_HPP__ */
