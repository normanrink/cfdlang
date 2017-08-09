
#include <string>


#include "Runtime/TensorContext.h"


typedef unsigned long handle;
typedef int Flag;


static inline
TensorContext *castToTensorContext(handle *h) {
  return (TensorContext *) *h;
}

static inline
TensorContext::CodeGenHandle castToCodeGenHandle(handle *h) {
  return (TensorContext::CodeGenHandle) *h;
}

static inline
TensorContext::KernelHandle castToKernelHandle(handle *h) {
  return (TensorContext::KernelHandle) *h;
}

static inline
TensorContext::ExecutionHandle castToExecutionHandle(handle *h) {
  return (TensorContext::ExecutionHandle) *h;
}


extern "C" void TensorInitContext(handle *h_ctx) {
  TensorContext *ctx = new TensorContext();
  *h_ctx = (handle) ctx;
}

extern "C" void TensorFinalContext(handle *h_ctx) {
  delete castToTensorContext(h_ctx);
}


extern "C" void TensorInitCodeGen(handle *h_ctx, handle *h_cg,
                                  const char *source,
                                  const Flag *rowMajor,
                                  const Flag *restrictPointer,
                                  const Flag *graphCodeGen) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::CodeGenHandle cgh;

  ctx->initCodeGen(&cgh, source,
                   *rowMajor,
                   *restrictPointer,
                   *graphCodeGen);
  *h_cg = (handle) cgh;
}

extern "C" void TensorGenerateCCode(handle *h_ctx, handle *h_cg) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::CodeGenHandle cgh = castToCodeGenHandle(h_cg);

  ctx->generateCCode(&cgh);
}

extern "C" void TensorFinalCodeGen(handle *h_ctx, handle *h_cg) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::CodeGenHandle cgh = castToCodeGenHandle(h_cg);

  ctx->finalCodeGen(&cgh);
}


extern "C" void TensorInitKernel(handle *h_ctx, handle *h_cg, handle *h_k,
                                 const Flag *cleanOnDestruction) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::CodeGenHandle cgh = castToCodeGenHandle(h_cg);
  TensorContext::KernelHandle kh;

  ctx->initKernel(&kh, &cgh, *cleanOnDestruction);
  *h_k = (handle) kh;
}

extern "C" void TensorBuildKernel(handle *h_ctx, handle *h_k) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::KernelHandle kh = castToKernelHandle(h_k);

  ctx->buildKernel(&kh);
}

extern "C" void TensorFinalKernel(handle *h_ctx, handle *h_k) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::KernelHandle kh = castToKernelHandle(h_k);

  ctx->finalKernel(&kh);
}


extern "C" void TensorInitExecution(handle *h_ctx, handle *h_k, handle *h_ex) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::KernelHandle kh = castToKernelHandle(h_k);
  TensorContext::ExecutionHandle eh;

  ctx->initExecution(&eh, &kh);
  *h_ex = (handle) eh;
}

extern "C" void TensorAddArgument(handle *h_ctx, handle *h_ex,
                                  const char *name,
                                  void *arg) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::ExecutionHandle eh = castToExecutionHandle(h_ex);

  ctx->addArgument(&eh, std::string(name), arg);
}

extern "C" void TensorAddArguments(handle *h_ctx, handle *h_ex,
                                   const char **names,
                                   void **args,
                                   const int *num_args) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::ExecutionHandle eh = castToExecutionHandle(h_ex);

  ctx->addArguments(&eh, names, args, *num_args);
}

extern "C" void TensorClearArguments(handle *h_ctx, handle *h_ex) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::ExecutionHandle eh = castToExecutionHandle(h_ex);

  ctx->clearArguments(&eh);
}

extern "C" void TensorExecute(handle *h_ctx, handle *h_ex) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::ExecutionHandle eh = castToExecutionHandle(h_ex);

  ctx->execute(&eh);
}

extern "C" void TensorExecuteWithArguments(handle *h_ctx, handle *h_ex,
                                           const char **names,
                                           void **args,
                                           const int *num_args) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::ExecutionHandle eh = castToExecutionHandle(h_ex);

  ctx->executeWithArguments(&eh, names, args, *num_args);
}

extern "C" void TensorExecuteWithPositionalArgs(handle *h_ctx, handle *h_ex,
                                                void **args,
                                                const int *num_args) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::ExecutionHandle eh = castToExecutionHandle(h_ex);

  ctx->executeWithPositionalArgs(&eh, args, *num_args);
}

extern "C" void TensorFinalExecution(handle *h_ctx, handle *h_ex) {
  TensorContext *ctx = castToTensorContext(h_ctx);
  TensorContext::ExecutionHandle eh = castToExecutionHandle(h_ex);

  ctx->finalExecution(&eh);
}

