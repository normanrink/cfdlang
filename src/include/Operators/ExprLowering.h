
#ifndef __OPERATORS_EXPR_LOWERING_H__
#define __OPERATORS_EXPR_LOWERING_H__

#include <assert.h>

#include <ostream>
#include <set>
#include <sstream>
#include <vector>


#include "AST/AST.h"
#include "Operators/ExprVisitor.h"


namespace Operators {

typedef std::vector<int> Indices;

class ExprLowering : public ExprVisitor<ExprLowering> {
  CFDlang::DeclList *decls;
  CFDlang::StmtList *stmts;

  std::set<std::string> declaredVars;

  bool isDeclared(const std::string &v) const {
    return declaredVars.count(v);
  }

  void append(const CFDlang::Decl *d) {
    const auto &name = d->getIdentifier()->getName();
    assert(!isDeclared(name));
    declaredVars.insert(name);
    decls->append(d);
  }
  void append(const CFDlang::Stmt *s) { stmts->append(s); }

  const CFDlang::Program *getCFDlangProgram() const {
    assert(decls && stmts);
    return CFDlang::Program::create(decls, stmts);
  }

  int count;

  const std::string getFreshName() {
    std::stringstream s;
    s << "T_" << (count++);
    return s.str();
  }

  const CFDlang::Expr *curResult;
  Indices curInIndices;
  Indices curOutIndices;

  const CFDlang::Expr *getCurResult() const { return curResult; }
  const Indices &getCurInIndices() const { return curInIndices; }
  const Indices &getCurOutIndices() const { return curOutIndices; }

  void setCurResult(const CFDlang::Expr *expr) { curResult = expr; }
  void setCurInIndices(const Indices &is) { curInIndices = is; }
  void setCurOutIndices(const Indices &is) { curOutIndices = is; }

  void emitVarDecl(const std::string &name,
                   const Dimensions &inDims,
                   const Dimensions &outDims,
                   CFDlang::Decl::IOSpecifier ios);

  template<typename S, typename T, typename Derived>
  void emitBinary(const Binary<S, T, Derived> &expr,
                  CFDlang::ASTNode::NodeType nt);

  template<typename DerivedS>
  void emitScalar(const Scalar<DerivedS> &s);

  template<typename DerivedM>
  void emitMatrix(const MatrixBase<DerivedM> &m);

public:
  ExprLowering() : decls(nullptr), stmts(nullptr), count(0) {}

  template<typename T>
  const CFDlang::Program *lowerExpr(const Expr<T> &expr,
                                    std::string input,
                                    std::string output);

  template<int d, typename... DerivedMs>
  const CFDlang::Program *lowerExpr(const Operator<d, DerivedMs...> &op,
                                    std::string input,
                                    std::string output);

  template<typename T>
  const CFDlang::Program *lowerExpr(const Expr<T> &expr,
                                    std::string output);

  template<int d, typename... DerivedMs>
  const CFDlang::Program *lowerExpr(const Operator<d, DerivedMs...> &op,
                                    std::string output);

  template<typename S, typename T>
  void visitAdd(const Add<S, T> &expr);

  template<typename S, typename T>
  void visitSub(const Sub<S, T> &expr);

  template<typename S, typename T>
  void visitSMul(const SMul<S, T> &expr);

  template<typename S, typename T>
  void visitApply(const Apply<S, T> &expr);

  void visitSConst(const SConst &expr);
  void visitSVar(const SVar &expr);

  void visitMatrix(const Matrix &expr);
  void visitSymmetric(const Symmetric &expr);
  void visitAntisymmetric(const Antisymmetric &expr);
  void visitDiagonal(const Diagonal &expr);
  void visitIdentity(const Identity &expr);

  template<typename DerivedM>
  void visitOperator(const Operator<1, DerivedM> &expr);

  template<int d, typename... DerivedMs>
  void visitOperator(const Operator<d, DerivedMs...> &expr);

  template<int r>
  void visitTensor(const Tensor<r> &expr);
};

};

#include "Operators/ExprLowering_impl.hpp"

#endif /* __OPERATORS_EXPR_LOWERING_H__ */
