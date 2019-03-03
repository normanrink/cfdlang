
#ifndef __OPERATORS_TENSOR_H__
#define __OPERATORS_TENSOR_H__

#include <assert.h>

#include <ostream>

#include "Operators/ExprVisitor.h"
#include "Operators/Forwards.h"


namespace Operators {

template<>
class Tensor<1> {
  using This = Tensor<1>;

  const std::string name;

  const int d;

public:
  Tensor(const std::string &name, const int &d) : name(name), d(d) {}

  Tensor(const This &t) : name(t.getName()), d(t.getD()) {}

  const std::string &getName() const { return name; }

  static int getSize() { return 1; }

  const int &getD() const { return d; }

  Dimensions getDims() const {
    Dimensions ds = { getD() };
    return ds;
  }

  // for compatibility only:
  Dimensions getInDims() const { return getDims(); }
  // for compatibility only:
  Dimensions getOutDims() const { return getDims(); }

  static ExprKind getKind() { return EK_Tensor; }

  void prettyPrintDims(std::ostream &os) const {
    os << getD();
  }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << "Tensor<" << getSize() << "; ";
    prettyPrintDims(os);
    os << ">";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitTensor(*this);
  }
};


template<int r>
class Tensor : public Tensor<r-1> {
  using This = Tensor<r>;
  using Base = Tensor<r-1>;

  const int d;

  const Base &getAsBase() const {
    auto cast = static_cast<const Base *>(this);
    return *cast;
  }

public:
  template<typename... Ints>
  Tensor(const std::string &name, const int &d, const Ints &...ds)
  : Base(name, ds...)
  , d(d) {
    static_assert(r > 1, "");
  }
  Tensor(const This &t) : Base(t.getAsBase()), d(t.getD()) {
    static_assert(r > 1, "");
  }

  static int getSize() { return r; }

  const int &getD() const { return d; }

  Dimensions getDims() const {
    Dimensions base_ds = getAsBase().getDims();
    Dimensions ds = { getD() };
    for (const auto &d : base_ds) { ds.push_back(d); }
    return ds;
  }

  // for compatibility only:
  Dimensions getInDims() const { return getDims(); }
  // for compatibility only:
  Dimensions getOutDims() const { return getDims(); }

  static ExprKind getKind() { return EK_Tensor; }

  void prettyPrintDims(std::ostream &os) const {
    os << getD() << ", ";
    getAsBase().prettyPrintDims(os);
  }

  void prettyPrint(int indent, std::ostream &os) const {
    EMIT_INDENT(indent, os);
    os << "Tensor<" << getSize() << "; ";
    prettyPrintDims(os);
    os << ">";
  }

  template<typename DerivedV>
  void visit(ExprVisitor<DerivedV> &v) const {
    v.visitTensor(*this);
  }
};

};

#endif /* __OPERATORS_TENSOR_H__ */
