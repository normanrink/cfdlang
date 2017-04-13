
#include <cassert>
#include <cstring>
#include <string>


#include "Runtime/TensorContext.h"
#include "Runtime/TensorExecution.h"
#include "Runtime/TensorKernel.h"


TensorExecution::TensorExecution(TensorContext *ctx,
                                 const TensorContext::KernelHandle *k)
  : Context(ctx),
    TheKernel(k),
    TotalNumArguments(Context->getNumFormalArguments(TheKernel)),
    NumSetArguments(0) {
  ActualArguments = new ArgPtr[TotalNumArguments];
  memset(ActualArguments, 0, sizeof(ArgPtr)*TotalNumArguments);
}

TensorExecution::~TensorExecution() {
  delete ActualArguments;
}

void TensorExecution::addArgument(const std::string &name, ArgPtr arg) {
  assert(getNumSetArguments() < TotalNumArguments
         && "runtime error: attempting to set to many actual arguments");

  const unsigned position =
    getContext()->getFormalArgumentPosition(getKernelHandle(), name);
  assert(position != TensorKernel::UNKNOWN_FORMAL_ARG
         && "runtime error: invalid argument name");

  ActualArguments[position] = arg;
  ++NumSetArguments;
}

void TensorExecution::clearArguments() {
  memset(ActualArguments, 0, sizeof(ArgPtr)*TotalNumArguments);
  NumSetArguments = 0;
}

void TensorExecution::execute() const {
  const TensorKernel *k = getContext()->getKernel(getKernelHandle());
  assert(k->isCodeValid() && "runtime error: kernel code absent");

  (k->getCode())(ActualArguments);
}
