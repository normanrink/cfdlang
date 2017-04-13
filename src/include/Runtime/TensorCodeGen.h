
#ifndef __TENSOR_CODEGEN_H__
#define __TENSOR_CODEGEN_H__

#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/CEmitter.h"
#include "Parse/Parser.h"
#include "Runtime/TensorContext.h"
#include "Sema/Sema.h"


class TensorCodeGen {
private:
  const std::string Source;

  const bool RowMajor;
  const bool GraphCodeGen;

  CodeGen *CG;

  std::string CCode;
  std::string FunctionName;

private:
  bool isCodeGenValid() const;
  void resetCodeGen();

public:
  TensorCodeGen(const char *source,
                bool rowMajor,
                bool graphCodeGen = true);
  ~TensorCodeGen();

  void generate(const std::string &resultName);

  std::string getCCode() const;
  std::string getFunctionName() const;

  unsigned getNumFunctionArguments() const;
  std::string getFunctionArgumentName(unsigned i) const;
};

#endif /* __TENSOR_CODEGEN_H__ */
