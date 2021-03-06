
#ifndef __TENSOR_CODEGEN_H__
#define __TENSOR_CODEGEN_H__

#include <string>


#include "AST/AST.h"
#include "CodeGen/CodeGen.h"
#include "CodeGen/CEmitter.h"
#include "Parse/Parser.h"
#include "Runtime/TensorContext.h"
#include "Sema/Sema.h"


class TensorCodeGen {
private:
  const std::string Source;
  const CFDlang::Program *AST;

  const bool RowMajor;
  const bool RestrictPointer;
  const bool IccPragmas;
  const bool OMPPragmas;
  const bool GraphCodeGen;

  CodeGen *CG;

  std::string CCode;
  std::string FunctionName;

private:
  bool isCodeGenValid() const;
  void resetCodeGen();

  TensorCodeGen(const char *source,
                const CFDlang::Program *ast,
                bool rowMajor,
                bool restrictPointer = true,
                bool iccPragmas = false,
                bool ompPragmas = false,
                bool graphCodeGen = true);

public:
  TensorCodeGen(const char *source,
                bool rowMajor,
                bool restrictPointer = true,
                bool iccPragmas = false,
                bool ompPragmas = false,
                bool graphCodeGen = true);
                
  TensorCodeGen(const CFDlang::Program *ast,
                bool rowMajor,
                bool restrictPointer = true,
                bool iccPragmas = false,
                bool ompPragmas = false,
                bool graphCodeGen = true);

  ~TensorCodeGen();

  void generate(const std::string &resultName);

  std::string getCCode() const;
  std::string getFunctionName() const;

  unsigned getNumFunctionArguments() const;
  std::string getFunctionArgumentName(unsigned i) const;

  bool getOMPPragmas() const { return OMPPragmas; }
};

#endif /* __TENSOR_CODEGEN_H__ */
