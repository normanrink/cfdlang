
#ifndef __OPERATORS_EXPR_LOWERING_IMPL_HPP__
#define __OPERATORS_EXPR_LOWERING_IMPL_HPP__

#include "Operators/ExprLowering.h"
#include "Operators/Operators.h"


template<typename T>
const CFDlang::Program *Operators::ExprLowering::lowerExpr(
  const Operators::Expr<T> &expr,
  std::string input,
  std::string output) {
    decls = CFDlang::DeclList::create();
    stmts = CFDlang::StmtList::create();
    declaredVars.clear();
    count = 0;
    setCurResult(nullptr);
    setCurInIndices({});
    setCurOutIndices({});

    assert(expr.getInDims().size() == expr.getOutDims().size());

    expr.visit(*this);
    const auto *r = getCurResult();
    const auto exprIns = getCurInIndices();

    emitVarDecl(input, {}, expr.getInDims(), CFDlang::Decl::IO_Input);
    const auto inputOuts = getCurOutIndices();

    emitVarDecl(output, expr.getOutDims(), {}, CFDlang::Decl::IO_Output);

    const auto *e =
      CFDlang::BinaryExpr::create(CFDlang::ASTNode::NT_ProductExpr,
                                  r,
                                  CFDlang::Identifier::create(input));

    CFDlang::ExprList *contrPairs = CFDlang::ExprList::create();
    const int n = exprIns.size();
    for (int i = 0; i < n; i++) {
      CFDlang::ExprList *pair =
        CFDlang::ExprList::create(CFDlang::Integer::create(exprIns.at(i)));
      pair->append(CFDlang::Integer::create(2*n+i));

      contrPairs->append(CFDlang::BrackExpr::create(pair));
    }

    const auto *c =
      CFDlang::BinaryExpr::create(CFDlang::ASTNode::NT_ContractionExpr,
                                  e,
                                  CFDlang::BrackExpr::create(contrPairs));
    const auto *s =
      CFDlang::Stmt::create(CFDlang::Identifier::create(output), c);
    append(s);

    return getCFDlangProgram();
  }

template<int d, typename... DerivedMs>
const CFDlang::Program *Operators::ExprLowering::lowerExpr(
  const Operator<d, DerivedMs...> &op,
  std::string input,
  std::string output) {
    return lowerExpr(Expr<Operator<d, DerivedMs...>>(op), input, output);
  }

template<typename T>
const CFDlang::Program *Operators::ExprLowering::lowerExpr(
  const Operators::Expr<T> &expr,
  std::string output) {
    decls = CFDlang::DeclList::create();
    stmts = CFDlang::StmtList::create();
    declaredVars.clear();
    count = 0;
    setCurResult(nullptr);
    setCurInIndices({});
    setCurOutIndices({});

    expr.visit(*this);
    const auto *r = getCurResult();
    const auto exprIns = getCurOutIndices();

    emitVarDecl(output, expr.getOutDims(), {}, CFDlang::Decl::IO_Output);

    const auto *s =
      CFDlang::Stmt::create(CFDlang::Identifier::create(output), r);
    append(s);

    return getCFDlangProgram();
  }

template<int d, typename... DerivedMs>
const CFDlang::Program *Operators::ExprLowering::lowerExpr(
  const Operator<d, DerivedMs...> &op,
  std::string output) {
    return lowerExpr(Expr<Operator<d, DerivedMs...>>(op), output);
  }

void Operators::ExprLowering::emitVarDecl(
  const std::string &name,
  const Operators::Dimensions &inDims,
  const Operators::Dimensions &outDims,
  CFDlang::Decl::IOSpecifier ios) {
     // Variable name should have been declared yet:
     assert(!isDeclared(name));

     // There should be an equal number of input and output dimensions ...
     //assert(inDims.size() == outDims.size());
     /// ... and the output dimensions should come first ...
     CFDlang::ExprList *dims = CFDlang::ExprList::create();
     Indices outs;
     for (int i = 0; i < outDims.size(); i++) {
       dims->append(CFDlang::Integer::create(outDims.at(i)));
       outs.push_back(i);
     }
     setCurOutIndices(outs);
     // ... followed by the input dimensions:
     Indices ins;
     for (int i = 0; i < inDims.size(); i++) {
       dims->append(CFDlang::Integer::create(inDims.at(i)));
       ins.push_back(outDims.size()+i);
     }
     setCurInIndices(ins);
     // Note that the only variables that are emitted are:
     //   (a) scalars, with no dimensions;
     //   (b) matrices, with one input and one output dimension;
     //   (c) the input tensor, with only output dimensions;
     //   (d) the output tensor, with only input dimensions.

     // Declare the new variable:
     const auto *d = CFDlang::Decl::create(CFDlang::ASTNode::NT_VarDecl,
                                           CFDlang::Identifier::create(name),
                                           CFDlang::BrackExpr::create(dims),
                                           ios);
     append(d);
   }


template<typename S, typename T, typename Derived>
void Operators::ExprLowering::emitBinary(
  const Operators::Binary<S, T, Derived> &expr,
  CFDlang::ASTNode::NodeType nt) {
    // Construct the binary expression:
    expr.getLHS().visit(*this);
    const auto *lhs = getCurResult();

    expr.getRHS().visit(*this);
    const auto *rhs = getCurResult();
    Indices rhsIns  = getCurInIndices();
    Indices rhsOuts = getCurOutIndices();


    const auto *e = CFDlang::BinaryExpr::create(nt, lhs, rhs);
    setCurResult(e);
    // For the binary expressions 'Add', 'Sub', 'SMul', the dimensions
    // of the rhs expression determine the dimensions of the result:
    setCurInIndices(rhsIns);
    setCurOutIndices(rhsOuts);
  }

template<typename S, typename T>
void Operators::ExprLowering::visitAdd(const Operators::Add<S, T> &expr) {
  emitBinary(expr, CFDlang::ASTNode::NT_AddExpr);
}

template<typename S, typename T>
void Operators::ExprLowering::visitSub(const Operators::Sub<S, T> &expr)  {
  emitBinary(expr, CFDlang::ASTNode::NT_SubExpr);
}

template<typename S, typename T>
void Operators::ExprLowering::visitSMul(const Operators::SMul<S, T> &expr) {
  emitBinary(expr, CFDlang::ASTNode::NT_MulExpr);
}

template<typename S, typename T>
void Operators::ExprLowering::visitApply(const Operators::Apply<S, T> &expr) {
  const auto &lhsExpr = expr.getLHS();
  const auto &rhsExpr = expr.getRHS();

  // Can only apply expressions of the same dimensionality:
  assert(lhsExpr.getInDims().size() == rhsExpr.getInDims().size());
  assert(lhsExpr.getOutDims().size() == rhsExpr.getOutDims().size());

  // Expressions can only be built up from operators/matrices and
  // therefore must have the same number of input and output dimensions:
  assert(lhsExpr.getInDims().size() == lhsExpr.getOutDims().size());
  assert(rhsExpr.getInDims().size() == rhsExpr.getOutDims().size());

  expr.getLHS().visit(*this);
  const auto *lhs = getCurResult();
  const Indices lhsIns  = getCurInIndices();
  const Indices lhsOuts = getCurOutIndices();

  expr.getRHS().visit(*this);
  const auto *rhs = getCurResult();
  const Indices rhsIns  = getCurInIndices();
  const Indices rhsOuts = getCurOutIndices();

  const auto *e =
    CFDlang::BinaryExpr::create(CFDlang::ASTNode::NT_ProductExpr, lhs, rhs);

  CFDlang::ExprList *contrPairs = CFDlang::ExprList::create();
  const int n = lhsIns.size();
  for (int i = 0; i < n; i++) {
    CFDlang::ExprList *pair =
      CFDlang::ExprList::create(CFDlang::Integer::create(lhsIns.at(i)));
    pair->append(CFDlang::Integer::create(rhsOuts.at(i) + 2*n));

    contrPairs->append(CFDlang::BrackExpr::create(pair));
  }

  const auto *c =
    CFDlang::BinaryExpr::create(CFDlang::ASTNode::NT_ContractionExpr,
                                e,
                                CFDlang::BrackExpr::create(contrPairs));
  setCurResult(c);
  setCurInIndices(rhsIns);
  setCurOutIndices(lhsOuts);
}

template<typename DerivedS>
void Operators::ExprLowering::emitScalar(const Operators::Scalar<DerivedS> &s) {
  const auto &name = s.getName();

  if (!isDeclared(name)) {
    emitVarDecl(name, s.getInDims(), s.getOutDims(), CFDlang::Decl::IO_Input);
  }
  setCurResult(CFDlang::Identifier::create(name));
  setCurInIndices({});
  setCurOutIndices({});
}

void Operators::ExprLowering::visitSConst(const Operators::SConst &expr) {
  assert(0 && "constant literals not supported by 'CFDlang'");
}

void Operators::ExprLowering::visitSVar(const Operators::SVar &expr) {
  emitScalar(expr);
}

template<typename DerivedM>
void Operators::ExprLowering::emitMatrix(
  const Operators::MatrixBase<DerivedM> &m) {
    const auto &name = m.getName();
    const auto inDim = m.getN();
    const auto outDim = m.getM();

    if (!isDeclared(name)) {
      emitVarDecl(name, m.getInDims(), m.getOutDims(), CFDlang::Decl::IO_Input);
    }
    setCurResult(CFDlang::Identifier::create(name));
    setCurInIndices({1});
    setCurOutIndices({0});
  }

void Operators::ExprLowering::visitMatrix(
  const Operators::Matrix &expr)  {
    emitMatrix(expr);
}

void Operators::ExprLowering::visitSymmetric(
  const Operators::Symmetric &expr) {
    emitMatrix(expr);
}

void Operators::ExprLowering::visitAntisymmetric(
  const Operators::Antisymmetric &expr) {
    emitMatrix(expr);
}

void Operators::ExprLowering::visitDiagonal(
  const Operators::Diagonal &expr)  {
    emitMatrix(expr);
}

void Operators::ExprLowering::visitIdentity(
  const Operators::Identity &expr)  {
    emitMatrix(expr);
}

template<typename DerivedM>
void Operators::ExprLowering::visitOperator(
  const Operators::Operator<1, DerivedM> &expr) {
    expr.getMatrix().visit(*this);
}

template<int d, typename... DerivedMs>
void Operators::ExprLowering::visitOperator(
  const Operator<d, DerivedMs...> &expr) {
    expr.getFront().visit(*this);
    const auto *front = getCurResult();
    Indices frontIns  = getCurInIndices();
    Indices frontOuts = getCurOutIndices();

    expr.getEnd().visit(*this);
    const auto *end = getCurResult();
    Indices endIns  = getCurInIndices();
    assert(endIns.size() == 1);
    Indices endOuts = getCurOutIndices();
    assert(endOuts.size() == 1);

    const auto *e =
      CFDlang::BinaryExpr::create(CFDlang::ASTNode::NT_ProductExpr, front, end);

    setCurResult(e);
    frontIns.push_back(2*d-1);
    setCurInIndices(frontIns);
    frontOuts.push_back(2*d-2);
    setCurOutIndices(frontOuts);

}

template<int r>
void Operators::ExprLowering::visitTensor(const Tensor<r> &expr) {
  const auto &name = expr.getName();

  if (!isDeclared(name)) {
    emitVarDecl(name, {}, expr.getDims(), CFDlang::Decl::IO_Input);
  }
  setCurResult(CFDlang::Identifier::create(name));
  setCurInIndices({});

  Indices outs;
  for (int i = 0; i < expr.getDims().size(); i++) { outs.push_back(i); }
  setCurOutIndices(outs);
}

#endif /* __OPERATORS_EXPR_LOWERING_IMPL_HPP__ */
