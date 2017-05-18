
#ifndef __TENSOR_WRAPPERS_H__
#define __TENSOR_WRAPPERS_H__


typedef unsigned long handle;
typedef int Flag;


void TensorInitContext(handle *h_ctx);
void TensorFinalContext(handle *h_ctx);


void TensorInitCodeGen(handle *h_ctx, handle *h_cg,
                       const char *source,
                       const Flag *rowMajor,
                       const Flag *fuseElementLoop,
                       const Flag *restrictPointer,
                       const Flag *graphCodeGen);
void TensorGenerateCCode(handle *h_ctx, handle *h_cg);
void TensorFinalCodeGen(handle *h_ctx, handle *h_cg);


void TensorInitKernel(handle *h_ctx, handle *h_cg, handle *h_k,
                      const Flag *cleanOnDestruction);
void TensorBuildKernel(handle *h_ctx, handle *h_k);
void TensorFinalKernel(handle *h_ctx, handle *h_k);


void TensorInitExecution(handle *h_ctx, handle *h_k, handle *h_ex);
void TensorAddArgument(handle *h_ctx, handle *h_ex,
                       const char *name,
                       void *arg);
void TensorAddArguments(handle *h_ctx, handle *h_ex,
                        const char **name,
                        void **arg,
                        const int *num_args);
void TensorClearArguments(handle *h_ctx, handle *h_ex);
void TensorExecute(handle *h_ctx, handle *h_ex);
void TensorExecuteWithArguments(handle *h_ctx, handle *h_ex,
                                const char **name,
                                void **arg,
                                const int *num_args);
void TensorFinalExecution(handle *h_ctx, handle *h_ex);

#endif /* __TENSOR_WRAPPERS_H__ */
