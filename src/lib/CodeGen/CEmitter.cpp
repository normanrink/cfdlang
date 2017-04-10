
#include "AST/AST.h"
#include "CodeGen/CEmitter.h"
#include "CodeGen/ExprTreeLifter.h"
#include "Sema/Sema.h"
#include "Sema/TensorType.h"


#define EMIT_INDENT(indent) { append(std::string((indent), ' ')); }
#define INDENT_PER_LEVEL (2)


void CEmitter::codeGen(const Program *p) {
  nestingLevel = initialNestingLevel = 0;
  //emit function signature:
  emitSignature();

  // open block for function body:
  append ("{\n");
  // the function signature are emitted at nesting level '0', the function
  // body (emission of which is about to follow) starts at level '1':
  nestingLevel = initialNestingLevel = 1;

  // construct the expression trees, one for each statement in the program:
  CG->visitProgram(p);

  for (const auto *s: CG->getStatements()) {
    std::list<CodeGen::Assignment> Assignments;

    // lift contractions and tensor stacks out of the expression tree
    // on the RHS of the statement 's':
    {
      ExprNode *en = getExprNode(s->getExpr());

      const auto &nodeLiftPredicate = [](const ExprNode *en) {
        return (en->isStackExpr() || en->isContractionExpr());
      };
      ExprTreeLifter lifter(CG, nodeLiftPredicate);

      en->transform(&lifter);

      for (const auto &ass : lifter.getAssignments())
        Assignments.push_back(ass);

      // the result of the expression tree that remains as 'en' after lifting
      // must be assigned to the variable on the LHS of the statement 's':
      Assignments.push_back({s->getIdentifier()->getName(), en});
    }

    for (const auto &ass: Assignments) {
      const Sema &sema = *getSema();

      const std::string result = ass.variable;
      const ExprNode *en = ass.en;
      const std::vector<int> &dims = getDims(en);

      nestingLevel = initialNestingLevel;

      // emit defintion of 'result' if necessary:
      if (!sema.is_in_inputs(result) && !sema.is_in_outputs(result)) {
        auto elements = [](const std::vector<int> &ds) {
          int es = 1;
          for (int i = 0; i < ds.size(); i++)
            es *= ds[i];
          return es;
        };

        EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
        append(getFPTypeName() + " " + result +
               "[" + std::to_string(elements(dims)) + "];\n");
      }

      // generate enough indices for the expression:
      exprIndices.clear();
      for (int i = 0; i < dims.size(); i++) {
        const std::string index = getIndex();
        exprIndices.push_back(index);
      }

      loopedOverIndices.clear();

      // we need this if-clause since code emission
      // for identifiers has been optimized out:
      if (en->isIdentifier()) {
        assert(0 && "internal error: assigning from identifier at top level");
      } else {
        setResultTemp(result + subscriptString(exprIndices, dims));
        en->visit(this);
      }

      assert((nestingLevel-initialNestingLevel) == loopedOverIndices.size());

      // close all for-loops:
      emitLoopFooterNest();
    }
  }

  // close block of function body:
  append("}\n");
}

std::string CEmitter::getIndex() {
  return "i" + std::to_string(IndexCounter++);
}

void CEmitter::emitSignature() { // emit function signature
  const Sema &sema = *getSema();

  bool isFirstArgument = true;
  append("void C_function(\n");

  // emit inputs as function arguments:
  for (auto in = sema.inputs_begin(); in != sema.inputs_end(); in++) {
    const Symbol *sym = *in;

    if (isFirstArgument) isFirstArgument = false;
    else append(",\n");

    // indent each argument:
    EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL)
    append(getFPTypeName() + " *" + sym->getName());
  }

  // emit outputs as function arguments:
  for (auto out = sema.outputs_begin(); out != sema.outputs_end(); out++) {
    const Symbol *sym = *out;

    // do not re-emit input arguments:
    if (sema.is_in_inputs(sym->getName()))
      continue;

    if (isFirstArgument) isFirstArgument = false;
    else append(",\n");

    // indent each argument:
    EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL)
    append(getFPTypeName() + " *" + sym->getName());
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
                                  const std::string &temp,
                                  const std::string &init) {
  EMIT_INDENT(indent)
  append(getFPTypeName() + " " + temp);
  if (init.length())
    append(" = " + init);
  append(";\n");
}

std::string CEmitter::subscriptString(const std::vector<std::string> &indices,
                                      const std::vector<int> &dims) const {
  assert(indices.size() == dims.size());

  const int rank = dims.size();
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

void CEmitter::visitBinOpExpr(const ExprNode *en, const std::string &op) {
  // here 'BinOp' means elementwise operation, i.e. the two operands have
  // the same ranks and dimensions
  assert(en->getNumChildren() == 2);

  const std::string result = getResultTemp();
  std::string temps[2];

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

  // type checking ensures that the dimensions of 'en' and its children agree:
  const std::vector<int> &dims = getDims(en);

  for (int i = 0; i < 2; i++) {
    if (en->getChild(i)->isIdentifier()) {
      temps[i] = en->getChild(i)->getName() +
                 subscriptString(exprIndices, dims);
    } else {
      temps[i] = getTemp();
      emitTempDefinition(nestingLevel*INDENT_PER_LEVEL, temps[i]);
      setResultTemp(temps[i]);
      en->getChild(i)->visit(this);
    }
  }

  // emit for-loop nest as appropriate:
  emitLoopHeaderNest(dims);

  EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
  append(result + " = " + temps[0] + " " + op + " " + temps[1] + ";\n");

  exprIndices = savedExprIndices;
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

void CEmitter::visitDivExpr(const DivExpr *en) {
  visitBinOpExpr(en, "/");
}

void CEmitter::visitProductExpr(const ProductExpr *en) {
  assert(en->getNumChildren() == 2);

  const std::string result = getResultTemp();
  std::string lhsTemp, rhsTemp;

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

  const ExprNode *lhsExpr = en->getChild(0);
  const ExprNode *rhsExpr = en->getChild(1);

  const std::vector<int> &exprDims = getDims(en);
  const std::vector<int> &lhsDims = getDims(en->getChild(0));
  const std::vector<int> &rhsDims = getDims(en->getChild(1));

  std::vector<std::string> lhsIndices, rhsIndices;
  for (int i = 0; i < lhsDims.size(); i++) {
    const std::string &index = exprIndices[i];
    lhsIndices.push_back(index);
  }
  for (int i = 0; i < rhsDims.size(); i++) {
    const std::string &index = exprIndices[lhsDims.size() + i];
    rhsIndices.push_back(index);
  }

  // visit the 'lhs':
  {
    exprIndices = lhsIndices;

    if (lhsExpr->isIdentifier()) {
      lhsTemp = lhsExpr->getName() + subscriptString(lhsIndices, lhsDims);
    } else {
      lhsTemp = getTemp();
      emitTempDefinition(nestingLevel*INDENT_PER_LEVEL, lhsTemp);
      setResultTemp(lhsTemp);
      lhsExpr->visit(this);
    }

    // emit for-loop nest for the 'lhs' early:
    emitLoopHeaderNest(lhsDims);
  }

  // visit the 'rhs', emit loop headers as necessary:
  {
    exprIndices = rhsIndices;

    if (rhsExpr->isIdentifier()) {
      rhsTemp = rhsExpr->getName() + subscriptString(rhsIndices, rhsDims);
    } else {
      rhsTemp = getTemp();
      setResultTemp(rhsTemp);
      emitTempDefinition(nestingLevel*INDENT_PER_LEVEL, rhsTemp);
      rhsExpr->visit(this);
    }

    // emit for-loop nest for the 'rhs':
    emitLoopHeaderNest(rhsDims);
  }

  EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
  append(result + " = " + lhsTemp + " * " + rhsTemp + ";\n");

  exprIndices = savedExprIndices;
}

void CEmitter::visitContractionExpr(const ContractionExpr *en) {
  assert(en->getNumChildren() == 2);

  const std::string result = getResultTemp();
  std::string lhsTemp, rhsTemp;

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

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
  append(result + " = 0.0;\n");

  // visit the 'lhs':
  {
    exprIndices = lhsIndices;

    if (lhsExpr->isIdentifier()) {
      lhsTemp = lhsExpr->getName() + subscriptString(lhsIndices, lhsDims);
    } else {
      lhsTemp = getTemp();
      emitTempDefinition(nestingLevel*INDENT_PER_LEVEL, lhsTemp);
      setResultTemp(lhsTemp);
      lhsExpr->visit(this);
    }

    // emit for-loop nest for the 'lhs' early:
    emitLoopHeaderNest(lhsDims);
  }

  // visit the 'rhs', emit loop headers as necessary:
  {
    exprIndices = rhsIndices;

    if (rhsExpr->isIdentifier()) {
      rhsTemp = rhsExpr->getName() + subscriptString(rhsIndices, rhsDims);
    } else {
      rhsTemp = getTemp();
      setResultTemp(rhsTemp);
      emitTempDefinition(nestingLevel*INDENT_PER_LEVEL, rhsTemp);
      rhsExpr->visit(this);
    }

    // emit for-loop nest for the 'rhs':
    emitLoopHeaderNest(rhsDims);
  }

  EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
  append(result + " += " + lhsTemp + " * " + rhsTemp + ";\n");

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
  const std::string result = getResultTemp();

  const std::vector<std::string> savedExprIndices = exprIndices;
  const int savedNestingLevel = nestingLevel;

  const std::vector<int> &dims = getDims(en);

  const std::string &firstResultIndex = savedExprIndices[0];
  std::vector<std::string> childExprIndices;
  // skip the first index:
  for (int i = 1; i < savedExprIndices.size(); i++)
    childExprIndices.push_back(savedExprIndices[i]);

  exprIndices = childExprIndices;

  for (int e = 0; e < en->getNumChildren(); e++) {
    // replace the first index in 'result' with the constant 'e':
    const std::string &firstResultConstantIndex = std::to_string(e);
    size_t i = result.find(firstResultIndex);
    std::string resultWithConstantFirstIndex = result;
    if (i != std::string::npos)
      resultWithConstantFirstIndex.replace(i,
                                           firstResultIndex.length(),
                                           firstResultConstantIndex);

    const ExprNode *child = en->getChild(e);
    const std::vector<int> childDims = getDims(child);

    std::string temp;
    if (child->isIdentifier()) {
      temp = child->getName() + subscriptString(childExprIndices, childDims);
    } else {
      temp = getTemp();
      emitTempDefinition(nestingLevel*INDENT_PER_LEVEL, temp);
      setResultTemp(temp);
      child->visit(this);
    }

    emitLoopHeaderNest(childDims);

    EMIT_INDENT(nestingLevel*INDENT_PER_LEVEL);
    append(resultWithConstantFirstIndex + " = " + temp + ";\n");

    // close for-loops ...
    while(nestingLevel > savedNestingLevel) {
      --nestingLevel;
      emitForLoopFooter(nestingLevel*INDENT_PER_LEVEL);
    }
    // ... and remove the corresponding loop indices from 'loopedOverIndices':
    for (int i = 0; i < exprIndices.size(); i++)
      loopedOverIndices.erase(exprIndices[i]);
  }

  exprIndices = savedExprIndices;
}

void CEmitter::visitIdentifierExpr(const IdentifierExpr *en) {
  assert(0 &&
         "internal error: code emission for identifier has been optimized out");
}
