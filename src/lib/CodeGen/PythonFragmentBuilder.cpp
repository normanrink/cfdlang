
#include <string>


#include "AST/AST.h"
#include "CodeGen/PythonCodeGen.h"
#include "Sema/Sema.h"
#include "Sema/TensorType.h"


PythonFragBuilder::PythonFragBuilder(const std::string &prefix)
  : ModulePrefix(prefix) {}

void PythonFragBuilder::buildNumpyProgramPrologue(const CodeGen *cg) {
  Fragment = "import numpy as " + getModulePrefix() + "\n\n";
}

void PythonFragBuilder::buildTheanoProgramPrologue(const CodeGen *cg) {
  Fragment = ("from theano import function as theano_function\n");
  Fragment += ("import theano.tensor as " + getModulePrefix() + "\n\n");
  Fragment += "\n"; 
}

void PythonFragBuilder::buildNumpyDeclEpilogue(const Decl *d,
                                               const CodeGen *cg) {
  Fragment = "";
  if (d->getNodeType() == ASTNode::NT_TypeDecl)
    return;

  const std::string &name = d->getIdentifier()->getName();
  const Symbol *sym = cg->getSema()->getSymbol(name);
  const TensorType &type = sym->getType();
  
  Fragment = (name + " = " + getModulePrefix() + ".random.rand("
                           + type.getDimString() + ")\n");
}

void PythonFragBuilder::buildTheanoDeclEpilogue(const Decl *d, CodeGen *cg) {
  // FIXME: perhaps the generated Python code should
  // reflect the user-defined type names
  Fragment = "";
  if (d->getNodeType() == ASTNode::NT_TypeDecl)
   return;
  
  const Sema *sema = cg->getSema();

  const std::string &name = d->getIdentifier()->getName();
  const Symbol *sym = sema->getSymbol(name);
  const TensorType *type = &sym->getType();

  std::string typeName;
  if (cg->typeEmitted(type)) {
    typeName = cg->getEmittedTypeName(type);
  } else { 
    typeName = (sema->isNamedType(type)) ? sema->getTypeSymbol(type)->getName()
                                         : cg->getTemp();

    Fragment += (typeName + " = " + getModulePrefix()
                                  + ".TensorType('float64', (False,)*"
                                  + std::to_string(type->getRank()) + ")\n");
    cg->addEmittedType(type, typeName);
  }

  Fragment += (name + " = " + getModulePrefix() + ".TensorVariable("
                            + typeName + ")\n");
}

void PythonFragBuilder::buildStmtEpilogue(const Stmt *s,
                                          const DirectCodeGen *cg) {
  const std::string &name = s->getIdentifier()->getName();
  const Expr *expr = s->getExpr();
  const std::string &temp = cg->getTempForExpr(expr);

  Fragment = (name + " = " + temp + "\n");
}

void PythonFragBuilder::buildTensorDotString(const std::string &result,
                                             const std::string &lhs,
                                             const std::string &rhs,
                                             const std::string &axes) {
  Fragment = (result + " = " + getModulePrefix() + ".tensordot("
                             + lhs + ", " + lhs + ", ");
  if (!axes.length())
    Fragment += ("axes=0)\n");
  else
    Fragment += ("axes=" + axes + ")\n");
}

void PythonFragBuilder::buildContractionEpilogue(
  const std::string &result,
  const std::string &lhs,
  const std::string &rhs, 
  const CodeGen::TupleList &LeftAndRightIndices)
{
  if (LeftAndRightIndices.empty())
    buildTensorDotString(result, lhs, rhs);
  else
    buildTensorDotString(result, lhs, rhs,
                         CodeGen::getTupleListString(LeftAndRightIndices));
}

void PythonFragBuilder::buildTensorExprEpilogue(const BinaryExpr *be,
                                                const DirectCodeGen *cg) {
  const std::string &tempL = cg->getTempForExpr(be->getLeft());
  const std::string &tempR = cg->getTempForExpr(be->getRight());
  const std::string &result = cg->getTempForExpr(be);

  buildTensorDotString(result, tempL, tempR);
}

// Is this needed:
/*
void NumpyDirectCodeGen::buildIntegerEpilogue(const Integer *i,
                                              ) {
  const std::string &temp = getTempForExpr(i);
  Fragment = (temp + " = " + std::to_string(i->getValue()));
}
*/

void PythonFragBuilder::buildBrackExprEpilogue(const BrackExpr *be,
                                               const DirectCodeGen *cg) {
  const ExprList &exprs = *be->getExprs();
  std::string result = cg->getTempForExpr(be);
  
  std::string stack = "[";
  for (unsigned i = 0; i < exprs.size(); i++) {
    const Expr *e = exprs[i];
    if (i > 0)  stack += ", ";
    stack += cg->getTempForExpr(e);
  }
  stack += "]";

  Fragment = (result + " = " + getModulePrefix() + ".stack("
                             + stack + ", axis=0)\n");
}


void PythonFragBuilder::buildTheanoProgramEpilogue(CodeGen *cg) {
  Fragment = "\n";

  const Sema *sema = cg->getSema();
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
    Fragment += (cg->getTemp() + " = theano_function(" + inputsList + ", "
                                                 + sym->getName() + ")\n");
  } else {
    IO_SYMBOL_LIST(outputs)
    Fragment += (cg->getTemp() + " = theano_function(" + inputsList + ", "
                                                       + outputsList + ")\n");
  }
}

void PythonFragBuilder::buildContraction(const std::string &result,
                                         const std::string &lhs,
                                         const CodeGen::List &lhsIndices,
                                         const std::string &rhs,
                                         const CodeGen::List &rhsIndices) {
  CodeGen::TupleList indices;
  indices.push_back(lhsIndices);
  indices.push_back(rhsIndices);

  Fragment = (result + " = " + getModulePrefix() + ".tensordot("
                             + lhs + ", " + rhs + ", " + "axes="
                             + CodeGen::getTupleListString(indices) + ")\n");
}

void PythonFragBuilder::buildTensorProduct(const std::string &result,
                                           const std::string &lhs,
                                           const std::string &rhs) {
  Fragment = (result + " = " + getModulePrefix() + ".tensordot("
                             + lhs + ", " + rhs + ")\n");
}

void PythonFragBuilder::buildTensorStack(const std::string &result,
                                         const std::list<std::string> &temps) {
  Fragment = (result + " = " + getModulePrefix() + ".stack([");
  for (auto it = temps.begin(); it != temps.end(); it++) {
    Fragment += (*it);
    if (std::next(it) != temps.end())
      Fragment += (", ");
  }
  Fragment += ("])\n");
}

void PythonFragBuilder::buildAssignment(const std::string &result,
                                        const std::string &expr) {
  Fragment = (result + " = " + expr + "\n");
}

