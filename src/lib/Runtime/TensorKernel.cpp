
#include <string>


#include "Runtime/TensorCodeGen.h"
#include "Runtime/TensorContext.h"
#include "Runtime/TensorKernel.h"


#define CFDLANG_FILENAME_PREFIX (std::string("cfdlang"))


unsigned TensorKernel::UNKNOWN_FORMAL_ARG = -1;

void TensorKernel::reset() {
  Code = nullptr;

  FormalArguments.clear();
  NumFormalArguments = 0;

  CFunctionName = "";
  CSourceFilePath = "";
  LibSOFilePath = "";
}

TensorKernel::TensorKernel(TensorContext *ctx,
                           const TensorContext::CodeGenHandle cg,
                           bool cleanOnDestruction)
  : Context(ctx),
    TheCodeGen(cg),
    CleanOnDestruction(cleanOnDestruction) {
  // Note that 'Context' and 'CleanOnDestruction' are not reset:
  reset();
}

TensorKernel::~TensorKernel() {
  unload();
  if (CleanOnDestruction)
    clean();
}

bool TensorKernel::isCodeValid() const {
  return (getCode() != nullptr);
}

void TensorKernel::invalidateCode() {
  setCode(nullptr);
}

void TensorKernel::load() {
  setCode((CodePtr)getContext()->loadLibSOWithSymbol(getLibSOFilePath(),
                                                     getCFunctionName()));
}

void TensorKernel::unload() {
  if (!isCodeValid())
    return;

  getContext()->unloadLibSO(getLibSOFilePath());
  invalidateCode();
}

void TensorKernel::clean() {
  if (isCodeValid())
    unload();

  getContext()->removeFile(getLibSOFilePath());
  getContext()->removeFile(getCSourceFilePath());

  reset();
}

void TensorKernel::build() {
  const TensorContext::CodeGenHandle h = getCodeGenHandle();
  CFunctionName = getContext()->getFunctionName(&h);
  
  CSourceFilePath = getTmpFolder() + "/" + \
                    CFDLANG_FILENAME_PREFIX + "-" + \
                    getUniqueIdentifier() + ".c";
  getContext()->generateCSourceFile(&h, CSourceFilePath);

  for (int i = 0; i < getContext()->getNumFunctionArguments(&h); i++)
    addFormalArgument(getContext()->getFunctionArgumentName(&h, i));

  LibSOFilePath = getTmpFolder() + "/" + \
                  CFDLANG_FILENAME_PREFIX + "-" + \
                  getUniqueIdentifier() + ".so";
  getContext()->generateLibSOFile(&h, LibSOFilePath, CSourceFilePath);
}

std::string TensorKernel::getUniqueIdentifier() {
  return getContext()->getUniqueIdentifier("-");
}

const std::string &TensorKernel::getTmpFolder() const {
  return getContext()->getTmpFolder();
}
