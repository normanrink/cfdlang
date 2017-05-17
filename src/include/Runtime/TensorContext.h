
#ifndef __TENSOR_CONTEXT_H__
#define __TENSOR_CONTEXT_H__

#include <unistd.h>
#include <sys/types.h>


#include <map>
#include <string>


class TensorCodeGen;
class TensorKernel;
class TensorExecution;


class TensorContext {
public:
  // for external use (i.e. external to the runtime library):
  typedef unsigned Handle;

  // for internal use (i.e. internal to the whole runtime library):
  typedef unsigned CodeGenHandle;
  typedef unsigned KernelHandle;
  typedef unsigned ExecutionHandle;

private:
  const std::string TmpFolder;
  const std::string CCompileCommand;
  const std::string Prefix;

  const pid_t ProcessID;

  std::map<CodeGenHandle, TensorCodeGen *> TheCodeGens;
  std::map<KernelHandle, TensorKernel *> TheKernels;
  std::map<ExecutionHandle, TensorExecution *> TheExecutions;

  unsigned NumHandles;

private:
  unsigned getHandle() { return NumHandles++; }

  TensorCodeGen *getCodeGen(const CodeGenHandle *h);
  TensorKernel *getKernel(const KernelHandle *h);
  TensorExecution *getExecution(const ExecutionHandle *h);

public:
  const TensorCodeGen *getCodeGen(const CodeGenHandle *h) const;
  const TensorKernel *getKernel(const KernelHandle *h) const;
  const TensorExecution *getExecution(const ExecutionHandle *h) const;

public:
  bool isCodeGenHandleValid(const CodeGenHandle *h) const;
  bool isKernelHandleValid(const KernelHandle *h) const;
  bool isExecutionHandleValid(const ExecutionHandle *h) const;

public:
  TensorContext();
  ~TensorContext();

  const std::string &getTmpFolder() const { return TmpFolder; }
  const std::string &getCCompileCommand() const { return CCompileCommand; }
  const std::string &getPrefix() const { return Prefix; }

  pid_t getProcessID() const { return ProcessID; }


  // interface for code generation:
  void initCodeGen(CodeGenHandle *h, const char *source,
                   bool rowMajor, bool fuseElementLoop, bool graphCodeGen);
  void finalCodeGen(const CodeGenHandle *h);

  void generateCCode(const CodeGenHandle *h);

  unsigned getNumFunctionArguments(const CodeGenHandle *h) const;
  std::string getFunctionArgumentName(const CodeGenHandle *h, unsigned i) const;
  std::string getFunctionName(const CodeGenHandle *h) const;

  void generateCSourceFile(const CodeGenHandle *h,
                           const std::string &CSourcePath) const;
  void generateLibSOFile(const CodeGenHandle *h,
                         const std::string &LibSOPath,
                         const std::string &CSourcePath) const;


  // interface for kernels:
  void initKernel(KernelHandle *h,
                  const CodeGenHandle *cg,
                  bool cleanOnDestruction = false);
  void buildKernel(const KernelHandle *h);
  void finalKernel(const KernelHandle *h);

  unsigned getNumFormalArguments(const KernelHandle *h) const;
  unsigned getFormalArgumentPosition(const KernelHandle *h,
                                     const std::string &name) const;


  // interface for executions:
  void initExecution(ExecutionHandle *h, const KernelHandle *k);
  void finalExecution(const ExecutionHandle *h);

  void addArgument(const ExecutionHandle *h,
                   const std::string &name,
                   void *arg);
  void addArguments(const ExecutionHandle *h,
                    const char **names,
                    void **arg,
                    const int *num_args);
  void clearArguments(const ExecutionHandle *h);

  void execute(const ExecutionHandle *h) const;
  void executeWithArguments(const ExecutionHandle *h,
                            const char **names,
                            void **arg,
                            const int *num_args);


  std::string getUniqueIdentifier(const std::string &separator);

  void *loadLibSOWithSymbol(const std::string &path,
                            const std::string &name);
  int unloadLibSO(const std::string &path);

  int removeFile(const std::string &path) const;

private:
  // book-keeping of dynamically loaded (shared object) libraries:
  std::map<std::string, void *> LibSOs;
};

#endif /* __TENSOR_CONTEXT_H__ */
