
#ifndef __TENSOR_EXECUTION_H__
#define __TENSOR_EXECUTION_H__

#include <string>
#include <vector>


#include "Runtime/TensorContext.h"


class TensorExecution {
public:
  typedef void * ArgPtr;

private:
  TensorContext *Context;

  const TensorContext::KernelHandle TheKernel;
  const unsigned TotalNumArguments;

  unsigned NumSetArguments;
  ArgPtr *ActualArguments;

public:
  int getNumSetArguments() const { return NumSetArguments;; }

public:
  TensorExecution(TensorContext *ctx,
                  const TensorContext::KernelHandle k);
  ~TensorExecution();

  const TensorContext *getContext() const { return Context; }
  const TensorContext::KernelHandle getKernelHandle() const {
    return TheKernel;
  }

  void addArgument(const std::string &name, ArgPtr arg);
  void addArguments(const char **names, ArgPtr *args, const int num_args);
  void clearArguments();

  void execute() const;
};

#endif /* __TENSOR_EXECUTION_H__ */

