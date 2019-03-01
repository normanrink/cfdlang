
#ifndef __OPERATORS_MATRIX_HPP__
#define __OPERATORS_MATRIX_HPP__

#include <ostream>
#include <string>

#include "Operators/Forwards.h"


namespace Operators {

template<typename Derived>
class MatrixBase {
  using This = MatrixBase<Derived>;

  const std::string name;

  const int m, n;

  const Derived &getAsDerivedM() const {
    auto cast = static_cast<const Derived *>(this);
    return *cast;
  }
public:
  MatrixBase() = delete;
  MatrixBase(const std::string &name, int m, int n) : name(name), m(m), n(n) {}

  const std::string &getName() const { return name; }

  int getM() const { return m; }
  int getN() const { return n; }

  Dimensions getInDims() const {
    Dimensions ds = { getN() };
    return ds;
  }
  Dimensions getOutDims() const {
    Dimensions ds = { getM() };
    return ds;
  }

  static ExprKind getKind() { return EK_Matrix; }

  static MatrKind getMKind() { return Derived::getMKind(); }

  void prettyPrint(int indent, std::ostream &os) const {
    static_cast<const Derived *>(this)->prettyPrint(indent, os);
  }

  const Operator<2, Matrix, Derived>
  operator*(const Matrix &m) const;

  const Operator<2, Symmetric, Derived>
  operator*(const Symmetric &m) const;

  const Operator<2, Antisymmetric, Derived>
  operator*(const Antisymmetric &m) const;

  const Operator<2, Diagonal, Derived>
  operator*(const Diagonal &m) const;

  const Operator<2, Identity, Derived>
  operator*(const Identity &m) const;

  const Expr<Neg<Operator<1, Derived>>> operator-() const;

  template<typename S>
  const Expr<Add<Operator<1, Derived>, S>>
  operator+(const Expr<S> &rhs) const;

  const Expr<Add<Operator<1, Derived>, Operator<1, Matrix>>>
  operator+(const Matrix &m) const;

  const Expr<Add<Operator<1, Derived>, Operator<1, Symmetric>>>
  operator+(const Symmetric &m) const;

  const Expr<Add<Operator<1, Derived>, Operator<1, Antisymmetric>>>
  operator+(const Antisymmetric &m) const;

  const Expr<Add<Operator<1, Derived>, Operator<1, Diagonal>>>
  operator+(const Diagonal &m) const;

  const Expr<Add<Operator<1, Derived>, Operator<1, Identity>>>
  operator+(const Identity &m) const;

  template<typename S>
  const Expr<Sub<Operator<1, Derived>, S>>
  operator-(const Expr<S> &rhs) const;

  const Expr<Sub<Operator<1, Derived>, Operator<1, Matrix>>>
  operator-(const Matrix &m) const;

  const Expr<Sub<Operator<1, Derived>, Operator<1, Symmetric>>>
  operator-(const Symmetric &m) const;

  const Expr<Sub<Operator<1, Derived>, Operator<1, Antisymmetric>>>
  operator-(const Antisymmetric &m) const;

  const Expr<Sub<Operator<1, Derived>, Operator<1, Diagonal>>>
  operator-(const Diagonal &m) const;

  const Expr<Sub<Operator<1, Derived>, Operator<1, Identity>>>
  operator-(const Identity &m) const;

  template<typename S>
  const Expr<Apply<Operator<1, Derived>, S>>
  operator()(const Expr<S> &rhs) const;

  const Expr<Apply<Operator<1, Derived>, Operator<1, Matrix>>>
  operator()(const Matrix &m) const;

  const Expr<Apply<Operator<1, Derived>, Operator<1, Symmetric>>>
  operator()(const Symmetric &m) const;

  const Expr<Apply<Operator<1, Derived>, Operator<1, Antisymmetric>>>
  operator()(const Antisymmetric &m) const;

  const Expr<Apply<Operator<1, Derived>, Operator<1, Diagonal>>>
  operator()(const Diagonal &m) const;

  const Expr<Apply<Operator<1, Derived>, Operator<1, Identity>>>
  operator()(const Identity &m) const;

  // Cannot use scalar-multiplication 'SMul' with a matrix as the 1st operand.

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const;
};

class Matrix : public MatrixBase<Matrix> {
public:
  Matrix(const std::string &name, int m, int n) : MatrixBase(name, m, n) {}

  static MatrKind getMKind() { return MK_Matrix; }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << MatrLabel.at(getMKind()) << "<" << getM() << "," << getN() << ">\n";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitMatrix(*this);
  }
};

class Symmetric : public MatrixBase<Symmetric> {
public:
  Symmetric(const std::string &name, int m) : MatrixBase(name, m, m) {}

  static MatrKind getMKind() { return MK_Symmetric; }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << MatrLabel.at(getMKind()) << "<" << getM() << ">\n";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitSymmetric(*this);
  }
};

class Antisymmetric : public MatrixBase<Antisymmetric> {
public:
  Antisymmetric(const std::string &name, int m) : MatrixBase(name, m, m) {}

  static MatrKind getMKind() { return MK_Antisymmetric; }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << MatrLabel.at(getMKind()) << "<" << getM() << ">\n";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitAntisymmetric(*this);
  }
};

class Diagonal : public MatrixBase<Diagonal> {
public:
  Diagonal(const std::string &name, int m) : MatrixBase(name, m, m) {}

  static MatrKind getMKind() { return MK_Diagonal; }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << MatrLabel.at(getMKind()) << "<" << getM() << ">\n";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitDiagonal(*this);
  }
};

class Identity : public MatrixBase<Identity> {
public:
  Identity(const std::string &name, int m) : MatrixBase(name, m, m) {}

  static MatrKind getMKind() { return MK_Identity; }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << MatrLabel.at(getMKind()) << "<" << getM() << ">\n";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitIdentity(*this);
  }
};

};

#include "Operators/Matrix_impl.hpp"

#endif /* __OPERATORS_MATRIX_HPP__ */
