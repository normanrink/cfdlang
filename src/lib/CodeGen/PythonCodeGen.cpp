
#include <vector>


#include "AST/AST.h"
#include "CodeGen/PythonCodeGen.h"
#include "Sema/Sema.h"
#include "Sema/TensorType.h"


NumpyDirectCodeGen::NumpyDirectCodeGen(const Sema *sema,
                                       const std::string &prefix)
  : DirectCodeGen(sema), ModulePrefix(prefix) {}

void NumpyDirectCodeGen::visitProgramPrologue(const Program *p) {
  append("import numpy as " + getModulePrefix() + "\n\n");
}

void NumpyDirectCodeGen::visitDeclEpilogue(const Decl *d) {
  if (d->getNodeType() == NT_TypeDecl)
    return;

  const std::string &name = d->getIdentifier()->getName();
  const Symbol *sym = getSema()->getSymbol(name);
  const TensorType &type = sym->getType();
  
  const std::string initializer = getModulePrefix() + ".random.rand("
                                  + type.getDimString() + ")\n";

  append(name + " = " + initializer);
}

void NumpyDirectCodeGen::visitStmtEpilogue(const Stmt *s) {
  const std::string &name = s->getIdentifier()->getName();
  const Expr *expr = s->getExpr();
  const std::string &temp = getTempForExpr(expr);

  append(name + " = " + temp + "\n");
}

const std::string NumpyDirectCodeGen::getTensorDotString(
  const std::string &r,
  const std::string &t0,
  const std::string &t1,
  const std::string axes) const
{
  const std::string &prefix = r + " = " + getModulePrefix()
                              + ".tensordot(" + t0 + ", " + t1 + ", ";
  if (!axes.length())
    return prefix + "axes=0)\n";
  else
    return prefix + "axes=" + axes + ")\n";
}

const std::string NumpyDirectCodeGen::visitContractionEpilogue(
  const Expr *e,
  const std::string &lhs,
  const std::string &rhs, 
  const TupleList &LeftAndRightInds)
{
  const std::string result = getTemp();

  if (LeftAndRightInds.empty())
    append(getTensorDotString(result, lhs, rhs));
  else
    append(getTensorDotString(result, lhs, rhs,
                              getTupleListString(LeftAndRightInds)));

  return result;
}

void NumpyDirectCodeGen::visitTensorExprEpilogue(const BinaryExpr *be) {
  const std::string &tempL = getTempForExpr(be->getLeft());
  const std::string &tempR = getTempForExpr(be->getRight());
  const std::string &result = getTempForExpr(be);

  append(getTensorDotString(result, tempL, tempR));
}

void NumpyDirectCodeGen::visitIntegerEpilogue(const Integer *i) {
  const std::string &temp = getTempForExpr(i);
  append(temp + " = " + std::to_string(i->getValue()));
}

void NumpyDirectCodeGen::visitBrackExprEpilogue(const BrackExpr *be) {
  const ExprList &exprs = *be->getExprs();
  std::string result = getTempForExpr(be);
  
  std::string stack = "[";
  for (unsigned i = 0; i < exprs.size(); i++) {
    const Expr *e = exprs[i];
    if (i > 0)  stack += ", ";
    stack += getTempForExpr(e);
  }
  stack += "]";

  append(result + " = "
                + getModulePrefix() + ".stack(" + stack + ", axis=0)\n");
}


TheanoDirectCodeGen::TheanoDirectCodeGen(const Sema *sema,
                                         const std::string &prefix)
  : NumpyDirectCodeGen(sema, prefix) {}

void TheanoDirectCodeGen::constructTypes() {
  const Sema *sema = getSema();

  for (auto i = sema->types_begin(), e = sema->types_end();
       i != e; i++) {
    const TensorType *type = *i;
    const std::string temp = getTemp();
    append(temp + " = " + getModulePrefix()
           + ".TensorType('float64', (False,)*"
           + std::to_string(type->getRank()) + ")\n");
    TypeTemps[type] = temp;
  }
}

void TheanoDirectCodeGen::visitProgramPrologue(const Program *p) {
  append("from theano import function as theano_function\n");
  append("import theano.tensor as " + getModulePrefix() + "\n\n");

  constructTypes();
  append("\n"); 
}

void TheanoDirectCodeGen::visitProgramEpilogue(const Program *p) {
  append("\n");

  const Sema *sema = getSema();
  if (sema->inputs_size() == 0 || sema->outputs_size() == 0)
    return;

  #define IO_SYMBOL_LIST(inout)              \
    std::string inout##List;                 \
    {                                        \
      inout##List = "[";                     \
      bool first = true;                     \
      for (auto i = sema->inout##_begin(),    \
                e = sema->inout##_end();      \
           i != e; i++) {                    \
        const Symbol *sym = *i;              \
        if (!first) inout##List += ", ";     \
        inout##List += sym->getName();       \
        first = false;                       \
      }                                      \
      inout##List += "]";                    \
    }
  
  IO_SYMBOL_LIST(inputs)

  std::string output;
  if (sema->outputs_size() == 1) {
    const Symbol *sym = *sema->outputs_begin();
    append("f = theano_function(" + inputsList + ", " + sym->getName() + ")\n");
  } else {
    IO_SYMBOL_LIST(outputs)
    append("f = theano_function(" + inputsList + ", " + outputsList + ")\n");
  }
}

void TheanoDirectCodeGen::visitDeclEpilogue(const Decl *d) {
  // FIXME: perhaps the generated Python code should
  // reflect the user-defined type names
  if (d->getNodeType() == NT_TypeDecl)
   return;

  const std::string &name = d->getIdentifier()->getName();
  const Symbol *sym = getSema()->getSymbol(name);
  const TensorType *type = &sym->getType();
  const std::string &temp = TypeTemps[type];

  append(name + " = " + getModulePrefix() + ".TensorVariable(" + temp + ")\n");
}

