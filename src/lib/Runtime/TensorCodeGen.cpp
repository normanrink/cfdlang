
#include <string>


#include "AST/AST.h"
#include "CodeGen/DirectCodeGen.h"
#include "CodeGen/GraphCodeGen.h"
#include "CodeGen/CEmitter.h"
#include "Parse/Parser.h"
#include "Runtime/TensorCodeGen.h"
#include "Sema/Sema.h"


TensorCodeGen::TensorCodeGen(const char *source,
                             bool rowMajor,
                             bool restrictPointer,
                             bool graphCodeGen)
  : Source(source),
    RowMajor(rowMajor),
    RestrictPointer(restrictPointer),
    GraphCodeGen(graphCodeGen),
    CG(nullptr),
    CCode("") {}

TensorCodeGen::~TensorCodeGen() {
  resetCodeGen();
}

bool TensorCodeGen::isCodeGenValid() const {
  return (CG != nullptr);
}

void TensorCodeGen::resetCodeGen() {
  if (!isCodeGenValid())
    return;

  delete CG;
  CCode = "";
}

void TensorCodeGen::generate(const std::string &resultName) {
  Parser P(Source.c_str());
  int parseResult = P.parse();
  assert(!parseResult
         && "runtime error: invalid source code");

  Sema S;
  // TODO expose the internal error handling in 'Sema':
  S.visitProgram(P.getAST());

  resetCodeGen();

  if (GraphCodeGen)
    CG = new class GraphCodeGen(&S, resultName);
  else
    CG = new DirectCodeGen(&S, resultName);

  CEmitter emitter(CG,
                   /* rowMajor */ RowMajor,
                   /* emitWrapper */ true,
                   /* restrictPointer */ RestrictPointer);
  emitter.codeGen(P.getAST());
  CCode = emitter.getCode();
  FunctionName = CG->getFunctionName();

  // the AST is no longer needed after code has been generated:
  Program::destroy(P.getAST());
}

std::string TensorCodeGen::getCCode() const {
  if (!isCodeGenValid())
    assert(0 && "runtime error: invalid code generator");

  return CCode;
}

std::string TensorCodeGen::getFunctionName() const {
  if (!isCodeGenValid())
    assert(0 && "runtime error: invalid code generator");

  return FunctionName;
}

unsigned TensorCodeGen::getNumFunctionArguments() const {
  if (!isCodeGenValid())
    assert(0 && "runtime error: invalid code generator");

  return CG->getNumFunctionArguments();
}

std::string TensorCodeGen::getFunctionArgumentName(unsigned i) const {
  if (!isCodeGenValid())
    assert(0 && "runtime error: invalid code generator");

  assert(i < CG->getNumFunctionArguments()
         && "runtime error: index out of bounds for function arguments");

  return CG->getFunctionArgument(i).name;
}
