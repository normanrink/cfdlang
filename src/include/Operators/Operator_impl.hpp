
#ifndef __OPERATORS_OPERATOR_IMPL_HPP__
#define __OPERATORS_OPERATOR_IMPL_HPP__


using namespace Operators;

template<typename DerivedM>
const Operator<2, Matrix, DerivedM>
Operator<1, DerivedM>::operator*(const Matrix &m) const {
  return Operator<2, Matrix, DerivedM>(*this, m);
}

template<typename DerivedM>
const Operator<2, Symmetric, DerivedM>
Operator<1, DerivedM>::operator*(const Symmetric &m) const {
  return Operator<2, Symmetric, DerivedM>(*this, m);
}

template<typename DerivedM>
const Operator<2, Antisymmetric, DerivedM>
Operator<1, DerivedM>::operator*(const Antisymmetric &m) const {
  return Operator<2, Antisymmetric, DerivedM>(*this, m);
}

template<typename DerivedM>
const Operator<2, Diagonal, DerivedM>
Operator<1, DerivedM>::operator*(const Diagonal &m) const {
  return Operator<2, Diagonal, DerivedM>(*this, m);
}

template<typename DerivedM>
const Operator<2, Identity, DerivedM>
Operator<1, DerivedM>::operator*(const Identity &m) const {
  return Operator<2, Identity, DerivedM>(*this, m);
}

template<typename DerivedM>
const Expr<Neg<Operator<1, DerivedM>>>
Operator<1, DerivedM>::operator-() const {
  return Expr<Operator<1, DerivedM>>(*this).operator-();
}

template<typename DerivedM>
template<typename S>
const Expr<Add<Operator<1, DerivedM>, S>>
Operator<1, DerivedM>::operator+(const Expr<S> &rhs) const {
  return Expr<Operator<1, DerivedM>>(*this).operator+(rhs);
}

template<typename DerivedM>
template<typename DerivedOtherM>
const Expr<Add<Operator<1, DerivedM>, Operator<1, DerivedOtherM>>>
Operator<1, DerivedM>::operator+(const Operator<1, DerivedOtherM> &op) const {
  return this->operator+(Expr<Operator<1, DerivedOtherM>>(op));
}

template<typename DerivedM>
template<typename S>
const Expr<Sub<Operator<1, DerivedM>, S>>
Operator<1, DerivedM>::operator-(const Expr<S> &rhs) const {
  return Expr<Operator<1, DerivedM>>(*this).operator-(rhs);
}

template<typename DerivedM>
template<typename DerivedOtherM>
const Expr<Sub<Operator<1, DerivedM>, Operator<1, DerivedOtherM>>>
Operator<1, DerivedM>::operator-(const Operator<1, DerivedOtherM> &op) const {
  return this->operator-(Expr<Operator<1, DerivedOtherM>>(op));
}

template<typename DerivedM>
template<typename S>
const Expr<Apply<Operator<1, DerivedM>, S>>
Operator<1, DerivedM>::operator()(const Expr<S> &rhs) const {
    return Expr<Operator<1, DerivedM>>(*this).operator()(rhs);
}

template<typename DerivedM>
template<typename DerivedOtherM>
const Expr<Apply<Operator<1, DerivedM>, Operator<1, DerivedOtherM>>>
Operator<1, DerivedM>::operator()(const Operator<1, DerivedOtherM> &op) const {
  return this->operator()(Expr<Operator<1, DerivedOtherM>>(op));
}

template<typename DerivedM>
template<typename DerivedV>
void Operator<1, DerivedM>::visit(ExprVisitor<DerivedV> &v) const {
  v.visitOperator(*this);
}


template<int d, typename DerivedM, typename... DerivedMs>
const Operator<d+1, Matrix, DerivedM, DerivedMs...>
Operator<d, DerivedM, DerivedMs...>::operator*(const Matrix &m) const {
  return Operator<d+1, Matrix, DerivedM, DerivedMs...>(*this, m);
}

template<int d, typename DerivedM, typename... DerivedMs>
const Operator<d+1, Symmetric, DerivedM, DerivedMs...>
Operator<d, DerivedM, DerivedMs...>::operator*(const Symmetric &m) const {
  return Operator<d+1, Symmetric, DerivedM, DerivedMs...>(*this, m);
}

template<int d, typename DerivedM, typename... DerivedMs>
const Operator<d+1, Antisymmetric, DerivedM, DerivedMs...>
Operator<d, DerivedM, DerivedMs...>::operator*(const Antisymmetric &m) const {
  return Operator<d+1, Antisymmetric, DerivedM, DerivedMs...>(*this, m);
}

template<int d, typename DerivedM, typename... DerivedMs>
const Operator<d+1, Diagonal, DerivedM, DerivedMs...>
Operator<d, DerivedM, DerivedMs...>::operator*(const Diagonal &m) const {
  return Operator<d+1, Diagonal, DerivedM, DerivedMs...>(*this, m);
}

template<int d, typename DerivedM, typename... DerivedMs>
const Operator<d+1, Identity, DerivedM, DerivedMs...>
Operator<d, DerivedM, DerivedMs...>::operator*(const Identity &m) const {
  return Operator<d+1, Identity, DerivedM, DerivedMs...>(*this, m);
}

template<int d, typename DerivedM, typename... DerivedMs>
const Expr<Neg<Operator<d, DerivedM, DerivedMs...>>>
Operator<d, DerivedM, DerivedMs...>::operator-() const {
  return Expr<Operator<d, DerivedM, DerivedMs...>>(*this).operator-();
}

template<int d, typename DerivedM, typename... DerivedMs>
template<typename S>
const Expr<Add<Operator<d, DerivedM, DerivedMs...>, S>>
Operator<d, DerivedM, DerivedMs...>::operator+(const Expr<S> &rhs) const {
  return Expr<Operator<d, DerivedM, DerivedMs...>>(*this).operator+(rhs);
}

template<int d, typename DerivedM, typename... DerivedMs>
template<typename... DerivedOtherMs>
const Expr<Add<Operator<d, DerivedM, DerivedMs...>,
               Operator<d, DerivedOtherMs...>>>
Operator<d, DerivedM, DerivedMs...>::operator+(
  const Operator<d, DerivedOtherMs...> &op) const {
    return operator+(Expr<Operator<d, DerivedOtherMs...>>(op));
  }

template<int d, typename DerivedM, typename... DerivedMs>
template<typename S>
const Expr<Sub<Operator<d, DerivedM, DerivedMs...>, S>>
Operator<d, DerivedM, DerivedMs...>::operator-(const Expr<S> &rhs) const {
    return Expr<Operator<d, DerivedM, DerivedMs...>>(*this).operator-(rhs);
  }

template<int d, typename DerivedM, typename... DerivedMs>
template<typename... DerivedOtherMs>
const Expr<Sub<Operator<d, DerivedM, DerivedMs...>,
               Operator<d, DerivedOtherMs...>>>
Operator<d, DerivedM, DerivedMs...>::operator-(
  const Operator<d, DerivedOtherMs...> &op) const {
    return operator-(Expr<Operator<d, DerivedOtherMs...>>(op));
  }

template<int d, typename DerivedM, typename... DerivedMs>
template<typename S>
const Expr<Apply<Operator<d, DerivedM, DerivedMs...>, S>>
Operator<d, DerivedM, DerivedMs...>::operator()(const Expr<S> &rhs) const {
  return Expr<Operator<d, DerivedM, DerivedMs...>>(*this).operator()(rhs);
}

template<int d, typename DerivedM, typename... DerivedMs>
template<typename... DerivedOtherMs>
const Expr<Apply<Operator<d, DerivedM, DerivedMs...>,
                 Operator<d, DerivedOtherMs...>>>
Operator<d, DerivedM, DerivedMs...>::operator()(
  const Operator<d, DerivedOtherMs...> &op) const {
    return operator()(Expr<Operator<d, DerivedOtherMs...>>(op));
  }

template<int d, typename DerivedM, typename... DerivedMs>
template<typename DerivedV>
void
Operator<d, DerivedM, DerivedMs...>::visit(ExprVisitor<DerivedV> &v) const {
  v.visitOperator(*this);
}

#endif /* __OPERATORS_OPERATOR_IMPL_HPP__ */
