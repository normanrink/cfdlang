
#include "AST/AST.h"
#include "CodeGen/CEmitter.h"
#include "CodeGen/ExprTreeLifter.h"
#include "CodeGen/StackExprRemover.h"
#include "Sema/Sema.h"
#include "Sema/TensorType.h"


#define EMIT_INDENT(indent) { append(std::string((indent), ' ')); }
#define INDENT_PER_LEVEL (2)


void CEmitter::codeGen(const Program *p) {
  nestingLevel = initialNestingLevel = 0;

  if (EmitWrapper) {
    // if a wrapper will be emitted, mark the
    // wrapped function as "static inline":
    append("static inline\n");
  }
  //emit function signature:
  emitSignature();

  // open block for function body:
  append ("{\n");
  // the function signature are emitted at nesting level '0', the function
  // body (emission of which is about to follow) starts at level '1':
  nestingLevel = initialNestingLevel = 1;

  // construct the expression trees, one for each statement in the program:
  CG->visitProgram(p);

  StackExprRemover remover(CG);
  remover.transformAssignments();

  const auto &nodeLiftPredicate = [](const ExprNode *en) {
    return (en->isStackExpr() || en->isContractionExpr());
  };
  ExprTreeLifter lifter(CG, nodeLiftPredicate);
  lifter.transformAssignments();

  std::set<std::string> emittedNames;

  for (const auto &a: CG->getAssignments()) {
    const Sema &sema = *getSema();

    assert(a.lhs->isIdentifier() &&
           "internal error: left-hand side must be an identifier expression");

    const ExprNode *result = a.lhs;
    const std::string &resultName = result->getName();
    const std::vector<int> &resultDims = getDims(result);

    nestingLevel = initialNestingLevel;

    // emit defintion of 'result' if necessary:
    if (!sema.is_in_inputs(resultName) && !sema.is_in_outputs(resultName)
        && !emittedNames.count(resultName)) {
      auto elements = [](const std::vector<int> &ds) {
        unsigned long long es = 1;
        for (int i = 0; i < ds.size(); i++)
          es *= ds[i];
        return es;
      };

      EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
      append(getFPTypeName() + " " + resultName +
             "[" + std::to_string(elements(resultDims)) + "];\n");

      emittedNames.insert(resultName);
    }

    const ExprNode *en = a.rhs;
    const std::vector<int> &dims = getDims(en);

    // generate enough indices for the expression:
    exprIndices.clear();
    resultIndices.clear();
    for (int i = 0; i < dims.size(); i++) {
      const std::string index = getIndex();
      exprIndices.push_back(index);
      resultIndices.push_back(index);
    }

    loopedOverIndices.clear();

    setResultTemp(a.lhs);
    // we need this if-clause since code emission
    // for identifiers has been optimized out ...
    if (en->isIdentifier()) {
      // ... however, as a result of transforming stack expressions,
      // assignments with nothing but an identifier on the 'rhs' may
      // appear at the top level:
      visitTopLevelIdentifier(en);
    } else {
      en->visit(this);
    }

    assert((nestingLevel-initialNestingLevel) == loopedOverIndices.size());

    // close all for-loops:
    emitLoopFooterNest();
  }

  // close block of function body:
  append("}\n");

  if (EmitWrapper) {
    const int numArgs = getNumFunctionArguments();

    append("\n");
    append("void " + getFunctionName() + "(" +
           getFPTypeName() + " *args[" + std::to_string(numArgs) + "]){\n");

    const std::string &functionCall = getFunctionNameWrapped() + "(";
    // emit the call of 'FunctionName':
    EMIT_INDENT(INDENT_PER_LEVEL)
    append(functionCall);
    // emit the list of actual arguments of 'FunctionName':
    for (int i = 0; i < numArgs; i++) {
      if (i != 0) {
        append(",\n");
        EMIT_INDENT(INDENT_PER_LEVEL + functionCall.length())
      }

      append("args[" + std::to_string(i) + "]");
    }
    append(");\n");

    append("}\n");  
  }
}

std::string CEmitter::getIndex() {
  return "i" + std::to_string(IndexCounter++);
}

void CEmitter::emitSignature() { // emit function signature
  const Sema &sema = *getSema();

  bool isFirstArgument = true;
  append("void " + getFunctionNameWrapped() + "(\n");

  // emit inputs as function arguments:
  for (auto in = sema.inputs_begin(); in != sema.inputs_end(); in++) {
    const Symbol *sym = *in;

    if (isFirstArgument) isFirstArgument = false;
    else append(",\n");

    const std::string &ArgName = sym->getName();
    // indent each argument:
    EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL)
    append(getFPTypeName() + " *" + ArgName);

    addFunctionArgument(ArgName);
  }

  // emit outputs as function arguments:
  for (auto out = sema.outputs_begin(); out != sema.outputs_end(); out++) {
    const Symbol *sym = *out;

    // do not re-emit input arguments:
    if (sema.is_in_inputs(sym->getName()))
      continue;

    if (isFirstArgument) isFirstArgument = false;
    else append(",\n");

    const std::string &ArgName = sym->getName();
    // indent each argument:
    EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL)
    append(getFPTypeName() + " *" + ArgName);

    addFunctionArgument(ArgName);
  }

  // finish function signature:
  append(")\n");
}

void CEmitter::emitForLoopHeader(unsigned indent,
                                 const std::string &indexVar,
                                 const std::string &bound) {
  EMIT_INDENT(indent)
  append("for (unsigned " + indexVar + " = 0; "
         + indexVar + " < " + bound + "; "
         + indexVar + "++) {\n");
}

void CEmitter::emitForLoopHeader(unsigned indent,
                                 const std::string &indexVar,
                                 int intBound) {
  emitForLoopHeader(indent, indexVar, std::to_string(intBound));
}

void CEmitter::emitForLoopFooter(unsigned indent) {
  EMIT_INDENT(indent)
  append("}\n");
}

void CEmitter::emitTempDefinition(unsigned indent,
                                  const std::string &temp) {
  EMIT_INDENT(indent)
  append(getFPTypeName() + " " + temp + "[1];\n");
}

std::string CEmitter::subscriptString(const std::vector<std::string> &indices,
                                      const std::vector<int> &dims) const {
  assert(indices.size() == dims.size());

  const int rank = dims.size();
  if(rank == 0)
    return "[0]";

  std::string result = "(";

  if (RowMajor) {
    for (int i = (rank-1); i >= 0; i--) {
      result += indices[i];
      if (i != 0)
        result += " + " + std::to_string(dims[i]) + "*(";
    }
  } else {
    for (int i = 0; i < rank; i++) {
      result += indices[i];
      if (i != (rank-1))
        result += " + " + std::to_string(dims[i]) + "*(";
    }
  }

  for (int i = 0; i < rank; i++) {
    result += ")";
  }

  return "[" + result + "]";
}

std::string
CEmitter::subscriptedIdentifier(const ExprNode *en,
                                const std::vector<std::string> &indices) const {
  assert(en->isIdentifier());

  std::vector<std::string> allIndices;
  for (unsigned i = 0; i < en->getNumIndices(); i++)
    allIndices.push_back(en->getIndex(i));
  for (unsigned i = 0; i < indices.size(); i++)
    allIndices.push_back(indices.at(i));

  return (en->getName() + subscriptString(allIndices, en->getDims()));
}

void CEmitter::emitLoopHeaderNest(const std::vector<int> &exprDims) {
  const int rank = exprIndices.size();

  if (RowMajor) {
    for (int i = 0; i < rank; i++) {
      const std::string &index = exprIndices[i];
      if (loopedOverIndices.find(index) != loopedOverIndices.end()) {
        // 'index' is already iterated over in a for-loop:
        continue;
      }
      emitForLoopHeader(nestingLevel*INDENT_PER_LEVEL, index, exprDims[i]);
      loopedOverIndices.insert(index);
      ++nestingLevel;
    }
  } else {
    for (int i = (rank-1); i >= 0; i--) {
      const std::string &index = exprIndices[i];
      if (loopedOverIndices.find(index) != loopedOverIndices.end()) {
        // 'index' is already iterated over in a for-loop:
        continue;
      }
      emitForLoopHeader(nestingLevel*INDENT_PER_LEVEL, index, exprDims[i]);
      loopedOverIndices.insert(index);
      ++nestingLevel;
    }
  }
}

void CEmitter::emitLoopFooterNest() {
  while(nestingLevel > initialNestingLevel) {
    --nestingLevel;
    emitForLoopFooter(nestingLevel*INDENT_PER_LEVEL);
  }
}

std::string CEmitter::visitChildExpr(const ExprNode *en) {
  if (en->isIdentifier()) {
    return subscriptedIdentifier(en, exprIndices);
  } else {
    const std::string temp = getTemp();
    emitTempDefinition(nestingLevel*INDENT_PER_LEVEL, temp);
    ExprNode *tempNode =
      CG->getENBuilder()->createIdentifierExpr(temp, {});

    std::vector<std::string> savedResultIndices = resultIndices;
    setResultTemp(tempNode);
    resultIndices = {};
    en->visit(this);
    resultIndices = savedResultIndices;

    return subscriptedIdentifier(tempNode);
  }
}

void CEmitter::visitBinOpExpr(const ExprNode *en, const std::string &op) {
  // NOTE that 'BinOp' includes elementwise and scalar operations
  assert(en->getNumChildren() == 2);

  const ExprNode *result = getResultTemp();

  std::string lhsTemp, rhsTemp;

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &exprDims = getDims(en);
  const std::vector<int> &lhsDims = getDims(en->getChild(0));
  const std::vector<int> &rhsDims = getDims(en->getChild(1));

  std::vector<std::string> lhsIndices, rhsIndices;
  if (lhsDims.size() == 0 || rhsDims.size() == 0) {
    // one of the arguments of this 'BinOp' is a scalar;
    // hence, split the 'exprIndices' accordingly:
    for (int i = 0; i < lhsDims.size(); i++) {
      const std::string &index = exprIndices[i];
      lhsIndices.push_back(index);
    }
    for (int i = 0; i < rhsDims.size(); i++) {
      const std::string &index = exprIndices[lhsDims.size() + i];
      rhsIndices.push_back(index);
    }
  } else {
    assert(lhsDims.size() == rhsDims.size());
    // both arguments of this 'BinOps' are tensors;
    // hence, the same 'exprIndices' must be used for both arguments:
    lhsIndices = rhsIndices = exprIndices;
  }

  // emit the 'lhs':
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr);

  // emit the 'rhs':
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr);

  // emit for-loop nest as appropriate:
  exprIndices = savedExprIndices;
  emitLoopHeaderNest(exprDims);

  EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, resultIndices)
         + " = " + lhsTemp + " " + op + " " + rhsTemp + ";\n");
}

void CEmitter::visitAddExpr(const AddExpr *en) {
  visitBinOpExpr(en, "+");
}

void CEmitter::visitSubExpr(const SubExpr *en) {
  visitBinOpExpr(en, "-");
}

void CEmitter::visitMulExpr(const MulExpr *en) {
  visitBinOpExpr(en, "*");
}

void CEmitter::visitScalarMulExpr(const ScalarMulExpr *en) {
  visitBinOpExpr(en, "*");
}

void CEmitter::visitDivExpr(const DivExpr *en) {
  visitBinOpExpr(en, "/");
}

void CEmitter::visitScalarDivExpr(const ScalarDivExpr *en) {
  visitBinOpExpr(en, "/");
}


void CEmitter::visitProductExpr(const ProductExpr *en) {
  assert(en->getNumChildren() == 2);

  const ExprNode *result = getResultTemp();
  std::string lhsTemp, rhsTemp;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &exprDims = getDims(en);
  const std::vector<int> &lhsDims = getDims(en->getChild(0));
  const std::vector<int> &rhsDims = getDims(en->getChild(1));

  std::vector<std::string> lhsIndices, rhsIndices;
  // determine which indices belong to the 'lhs':
  for (int i = 0; i < lhsDims.size(); i++) {
    const std::string &index = exprIndices[i];
    lhsIndices.push_back(index);
  }
  // determine which indices belong to the 'rhs':
  for (int i = 0; i < rhsDims.size(); i++) {
    const std::string &index = exprIndices[lhsDims.size() + i];
    rhsIndices.push_back(index);
  }

  const std::vector<std::string> savedExprIndices = exprIndices;

  // visit the 'lhs':
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr);

  // visit the 'rhs', emit loop headers as necessary:
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr);

  exprIndices = savedExprIndices;
  emitLoopHeaderNest(exprDims);

  EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, resultIndices)
         + " = " + lhsTemp + " * " + rhsTemp + ";\n");
}

void CEmitter::visitContractionExpr(const ContractionExpr *en) {
  assert(en->getNumChildren() == 2);

  const ExprNode *result = getResultTemp();
  std::string lhsTemp, rhsTemp;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &exprDims = getDims(en);
  const std::vector<int> &lhsDims = getDims(en->getChild(0));
  const std::vector<int> &rhsDims = getDims(en->getChild(1));

  std::vector<std::string> contrIndices;
  for (int i = 0; i < en->getLeftIndices().size(); i++) {
    const std::string &index = getIndex() + "_contr";
    contrIndices.push_back(index);
  }

  std::vector<std::string> lhsIndices, rhsIndices;
  int exprIdxCounter = 0;
  // determine which indices belong to the 'lhs':
  {
    int contrIdxCounter = 0;

    for (int i = 0; i < lhsDims.size(); i++) {
      std::string index;

      if (i == en->getLeftIndices()[contrIdxCounter])
        index = contrIndices[contrIdxCounter++];
      else
        index = exprIndices[exprIdxCounter++];

      lhsIndices.push_back(index);
    }
  }
  // determine which indices belong to the 'rhs':
  {
    int contrIdxCounter = 0;

    for (int i = 0; i < rhsDims.size(); i++) {
      std::string index;

      if (i == en->getRightIndices()[contrIdxCounter])
        index = contrIndices[contrIdxCounter++];
      else
        index = exprIndices[exprIdxCounter++];

      rhsIndices.push_back(index);
    }
  }

  // emit for-loop nest for the result:
  emitLoopHeaderNest(exprDims);
  EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, resultIndices) + " = 0.0;\n");

  const std::vector<std::string> savedExprIndices = exprIndices;

  // visit the 'lhs':
  exprIndices = lhsIndices;
  lhsTemp = visitChildExpr(lhsExpr);
  // emit for-loop nest for the 'lhs' early:
  emitLoopHeaderNest(lhsDims);

  // visit the 'rhs', emit loop headers as necessary:
  exprIndices = rhsIndices;
  rhsTemp = visitChildExpr(rhsExpr);
  // emit for-loop nest for the 'rhs':
  emitLoopHeaderNest(rhsDims);

  EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, resultIndices)
         + " += " + lhsTemp + " * " + rhsTemp + ";\n");

  // close the for-loops that iterate over the contracted indices ...
  for (int i = 0; i < contrIndices.size(); i++) {
    --nestingLevel;
    emitForLoopFooter(nestingLevel*INDENT_PER_LEVEL);
  }
  // ... and remove the contracted indices from 'loopedOverIndices':
  for (int i = 0; i < contrIndices.size(); i++)
    loopedOverIndices.erase(contrIndices[i]);

  exprIndices = savedExprIndices;
}

void CEmitter::visitStackExpr(const StackExpr *en) {
  // the current implementation of this function yields correct results
  // ONLY if it emits code at the top level (no nestinf in any for loops):
  assert(nestingLevel == initialNestingLevel);
  // since this method has been called from the top level, 'exprIndices'
  // and 'resultIndices' must agree:
  assert(exprIndices == resultIndices);

  const ExprNode *result = getResultTemp();

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

  const std::vector<int> &dims = getDims(en);

  const std::string &firstResultIndex = savedExprIndices[0];
  std::vector<std::string> childExprIndices;
  // skip the first index:
  for (int i = 1/* The '1' is intended! */; i < savedExprIndices.size(); i++)
    childExprIndices.push_back(savedExprIndices[i]);

  exprIndices = childExprIndices;

  for (int e = 0; e < en->getNumChildren(); e++) {
    // replace the first index in 'result' with the constant 'e':
    IdentifierExpr *id =
      CG->getENBuilder()->createIdentifierExpr(result->getName(),
                                               result->getDims());
    for (unsigned i = 0; i < result->getNumIndices(); i++)
      id->addIndex(result->getIndex(i));
    id->addIndex(std::to_string(e));

    const ExprNode *child = en->getChild(e);
    const std::vector<int> childDims = getDims(child);

    exprIndices = childExprIndices;
    std::string temp = visitChildExpr(child);
    emitLoopHeaderNest(childDims);

    EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
    append(subscriptedIdentifier(id, childExprIndices) + " = " + temp + ";\n");

    // close the for-loops over the 'childExprIndices' ...
    while(nestingLevel > savedNestingLevel) {
      --nestingLevel;
      emitForLoopFooter(nestingLevel*INDENT_PER_LEVEL);
    }
    // ... and remove the loop indices from 'loopedOverIndices':
    for (int i = 0; i < childExprIndices.size(); i++)
      loopedOverIndices.erase(exprIndices[i]);
  }

  exprIndices = savedExprIndices;
}

void CEmitter::visitTopLevelIdentifier(const ExprNode *en) {
  // this function is only allowed to be called from the top level:
  assert(nestingLevel == initialNestingLevel);
  const ExprNode *result = getResultTemp();

  const int savedNestingLevel = nestingLevel;

  const std::vector<int> &dims = getDims(en);

  emitLoopHeaderNest(dims);

  EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
  append(subscriptedIdentifier(result, exprIndices)
         + " = " + subscriptedIdentifier(en, exprIndices) + ";\n");
}

void CEmitter::visitIdentifierExpr(const IdentifierExpr *en) {
  assert(0 &&
         "internal error: code emission for identifier has been optimized out");
}
