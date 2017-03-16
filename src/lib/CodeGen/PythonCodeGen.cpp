
#include <vector>


#include "AST/AST.h"
#include "CodeGen/PythonCodeGen.h"
#include "Sema/Sema.h"
#include "Sema/TensorType.h"


NumpyDirectCG::NumpyDirectCG(const Sema *sema, const std::string &prefix)
  : DirectCodeGen(sema), Builder(prefix) {}

void NumpyDirectCG::visitProgramPrologue(const Program *p) {
  Builder.buildNumpyProgramPrologue(this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitDeclEpilogue(const Decl *d) {
  Builder.buildNumpyDeclEpilogue(d, this);
  append(Builder.getFragment());
}

void NumpyDirectCG::visitStmtEpilogue(const Stmt *s) {
  Builder.buildStmtEpilogue(s, this);
  append(Builder.getFragment());
}

const std::string NumpyDirectCG::visitContractionEpilogue(
  const Expr *e,
  const std::string &lhs,
  const std::string &rhs, 
  const TupleList &LeftAndRightIndices)
{
  const std::string result = getTemp();
  Builder.buildContractionEpilogue(result, lhs, rhs, LeftAndRightIndices);
  append(Builder.getFragment());
  return result;
}

void NumpyDirectCG::visitTensorExprEpilogue(const BinaryExpr *be) {
  Builder.buildTensorExprEpilogue(be, this);
  append(Builder.getFragment());
}

// Needed??:
/*
void NumpyDirectCodeGen::visitIntegerEpilogue(const Integer *i) {
  const std::string &temp = getTempForExpr(i);
  append(temp + " = " + std::to_string(i->getValue()));
}
*/

void NumpyDirectCG::visitBrackExprEpilogue(const BrackExpr *be) {
  Builder.buildBrackExprEpilogue(be, this);
  append(Builder.getFragment());
}


TheanoDirectCG::TheanoDirectCG(const Sema *sema, const std::string &prefix)
  : NumpyDirectCG(sema, prefix) {}

void TheanoDirectCG::visitProgramPrologue(const Program *p) {
  Builder.buildTheanoProgramPrologue(this);
  append(Builder.getFragment());
}

void TheanoDirectCG::visitProgramEpilogue(const Program *p) {
  Builder.buildTheanoProgramEpilogue(this);
  append(Builder.getFragment());
}

void TheanoDirectCG::visitDeclEpilogue(const Decl *d) {
  Builder.buildTheanoDeclEpilogue(d, this);
  append(Builder.getFragment());
}


NumpyGraphCG::NumpyGraphCG(const Sema *sema, const std::string &prefix)
  : GraphCodeGen(sema), Builder(prefix) {}

void NumpyGraphCG::visitProgramPrologue(const Program *p) {
  Builder.buildNumpyProgramPrologue(this);
  append(Builder.getFragment());
}

void NumpyGraphCG::visitDeclEpilogue(const Decl *d) {
  Builder.buildNumpyDeclEpilogue(d, this);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitContraction(const std::string &result,
                                   const std::string &lhs,
                                   const List &lhsIndices,
                                   const std::string &rhs,
                                   const List &rhsIndices) {
  Builder.buildContraction(result, lhs, lhsIndices, rhs, rhsIndices);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitTensorProduct(const std::string &result,
                                     const std::string &lhs,
                                     const std::string &rhs) {
  Builder.buildTensorProduct(result, lhs, rhs);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitTensorStack(const std::string &result,
                                  const std::list<std::string> &temps) {
  Builder.buildTensorStack(result, temps);
  append(Builder.getFragment());
}

void NumpyGraphCG::emitAssignment(const std::string &result,
                                  const std::string &expr) {
  Builder.buildAssignment(result, expr);
  append(Builder.getFragment());
}


TheanoGraphCG::TheanoGraphCG(const Sema *sema, const std::string &prefix)
  : NumpyGraphCG(sema, prefix) {}

void TheanoGraphCG::visitProgramPrologue(const Program *p) {
  Builder.buildTheanoProgramPrologue(this);
  append(Builder.getFragment());
}

void TheanoGraphCG::visitProgramEpilogue(const Program *p) {
  Builder.buildTheanoProgramEpilogue(this);
  append(Builder.getFragment());
}

void TheanoGraphCG::visitDeclEpilogue(const Decl *d) {
  Builder.buildTheanoDeclEpilogue(d, this);
  append(Builder.getFragment());
}

