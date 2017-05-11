
#include <cassert>
#include <cstring>
#include <string>


#include "Runtime/TensorContext.h"
#include "Runtime/TensorExecution.h"
#include "Runtime/TensorKernel.h"


TensorExecution::TensorExecution(TensorContext *ctx,
                                 const TensorContext::KernelHandle k)
  : Context(ctx),
    TheKernel(k),
    TotalNumArguments(Context->getNumFormalArguments(&TheKernel)),
    NumSetArguments(0) {
  ActualArguments = new ArgPtr[TotalNumArguments];
  memset(ActualArguments, 0, sizeof(ArgPtr)*TotalNumArguments);
}

TensorExecution::~TensorExecution() {
  delete [] ActualArguments;
}

void TensorExecution::addArgument(const std::string &name, ArgPtr arg) {
  assert(getNumSetArguments() < TotalNumArguments
         && "runtime error: attempting to set to many actual arguments");

  const TensorContext::KernelHandle kh = getKernelHandle();
  const unsigned position =
    getContext()->getFormalArgumentPosition(&kh, name);
  assert(position != TensorKernel::UNKNOWN_FORMAL_ARG
         && "runtime error: invalid argument name");

  ActualArguments[position] = arg;
  ++NumSetArguments;
}

void TensorExecution::addArguments(const char **names, ArgPtr *args, const int num_args) {
  const TensorContext::KernelHandle kh = getKernelHandle();

  for (int i = 0; i < num_args; i++) {
    const unsigned position =
      getContext()->getFormalArgumentPosition(&kh, names[i]);
    assert(position != TensorKernel::UNKNOWN_FORMAL_ARG
           && "runtime error: invalid argument name");

    ActualArguments[position] = args[i];
    ++NumSetArguments;
  }

  assert(getNumSetArguments() <= TotalNumArguments
         && "runtime error: attempting to set to many actual arguments");
}

void TensorExecution::clearArguments() {
  memset(ActualArguments, 0, sizeof(ArgPtr)*TotalNumArguments);
  NumSetArguments = 0;
}

void TensorExecution::execute() const {
  const TensorContext::KernelHandle kh = getKernelHandle();
  const TensorKernel *k = getContext()->getKernel(&kh);
  assert(k->isCodeValid() && "runtime error: kernel code absent");

  if (getNumSetArguments() != k->getNumFormalArguments()) {
    assert(0 && "runtime error: insuffient actual arguments for execution");
    return;
  }

  (k->getCode())(ActualArguments);
}
