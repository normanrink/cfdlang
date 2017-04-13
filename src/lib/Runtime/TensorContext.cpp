
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>


#include <ctime>
#include <fstream>
#include <sstream>
#include <string>


#include "Runtime/TensorCodeGen.h"
#include "Runtime/TensorContext.h"
#include "Runtime/TensorExecution.h"
#include "Runtime/TensorKernel.h"


#define CFDLANG_ENV_TMP_FOLDER     "CFDLANG_TMP_FOLDER"
#define CFDLANG_DEFAULT_TMP_FOLDER "/tmp"
#define CFDLANG_ENV_CC             "CFDLANG_CC"
#define CFDLANG_DEFAULT_CC         "gcc"
#define CFDLANG_ENV_PREFIX         "CFDLANG_PREFIX"
#define CFDLANG_DEFAULT_PREFIX     "cfdlang"



TensorContext::TensorContext()
: TmpFolder(getenv(CFDLANG_ENV_TMP_FOLDER) ? getenv(CFDLANG_ENV_TMP_FOLDER)
                                           : (CFDLANG_DEFAULT_TMP_FOLDER)),
  CCompileCommand(getenv(CFDLANG_ENV_CC) ? getenv(CFDLANG_ENV_CC)
                                         : (CFDLANG_DEFAULT_CC)),
  Prefix(getenv(CFDLANG_ENV_PREFIX) ? getenv(CFDLANG_ENV_PREFIX)
                                    : (CFDLANG_DEFAULT_PREFIX)),
  ProcessID(getpid()),
  NumHandles(0) {}

TensorContext::~TensorContext() {
  for (auto &cit: TheCodeGens) delete cit.second;
  for (auto &kit: TheKernels) delete kit.second;
  for (auto &eit: TheExecutions) delete eit.second;
}

bool TensorContext::isCodeGenHandleValid(const CodeGenHandle *h) const {
  return (TheCodeGens.find(*h) != TheCodeGens.end());
}

bool TensorContext::isKernelHandleValid(const KernelHandle *h) const {
  return (TheKernels.find(*h) != TheKernels.end());
}

bool TensorContext::isExecutionHandleValid(const ExecutionHandle *h) const {
  return (TheExecutions.find(*h) != TheExecutions.end());
}

TensorCodeGen *TensorContext::getCodeGen(const CodeGenHandle *h) {
  assert(isCodeGenHandleValid(h)
         && "runtime error: invalid handle for code generation");

  TensorCodeGen *cg = TheCodeGens[*h];
  assert(cg && "internal error: invalid code generator");
  return cg;
}

const TensorCodeGen *TensorContext::getCodeGen(const CodeGenHandle *h) const {
  assert(isCodeGenHandleValid(h)
         && "runtime error: invalid handle for code generation");

  const TensorCodeGen *cg = TheCodeGens.at(*h);
  assert(cg && "internal error: invalid code generator");
  return cg;
}

TensorKernel *TensorContext::getKernel(const KernelHandle *h) {
  assert(isKernelHandleValid(h)
         && "runtime error: invalid handle for kernel");

  TensorKernel *k = TheKernels[*h];
  assert(k && "internal error: invalid kernel");
  return k;
}

const TensorKernel *TensorContext::getKernel(const KernelHandle *h) const {
  assert(isKernelHandleValid(h)
         && "runtime error: invalid handle for kernel");

  const TensorKernel *k = TheKernels.at(*h);
  assert(k && "internal error: invalid kernel");
  return k;
}

TensorExecution *TensorContext::getExecution(const ExecutionHandle *h) {
  assert(isExecutionHandleValid(h)
         && "runtime error: invalid handle for execution");

  TensorExecution *e = TheExecutions[*h];
  assert(e && "internal error: invalid execution");
  return e;
}

const TensorExecution *
TensorContext::getExecution(const ExecutionHandle *h) const {
  assert(isExecutionHandleValid(h)
         && "runtime error: invalid handle for execution");

  const TensorExecution *e = TheExecutions.at(*h);
  assert(e && "internal error: invalid execution");
  return e;
}

void TensorContext::initCodeGen(CodeGenHandle *h, const char *source,
                                bool rowMajor, bool graphCodeGen) {
  *h = getHandle();
  TheCodeGens[*h] = new TensorCodeGen(source, rowMajor, graphCodeGen);
}

void TensorContext::finalCodeGen(const CodeGenHandle *h) {
  TensorCodeGen *cg = getCodeGen(h);
  delete cg;
  TheCodeGens.erase(*h);
}

void TensorContext::generateCCode(const CodeGenHandle *h) {
  TensorCodeGen *cg = getCodeGen(h);
  const std::string functionName = getPrefix() + "_" + getUniqueIdentifier("_");
  cg->generate(functionName);
}

void TensorContext::generateCSourceFile(const CodeGenHandle *h,
                                        const std::string &CSourcePath) const {
  const TensorCodeGen *cg = getCodeGen(h);

  std::ofstream CSourceFile(CSourcePath);
  CSourceFile << cg->getCCode();
  CSourceFile.close();
}

unsigned TensorContext::getNumFunctionArguments(const CodeGenHandle *h) const {
  const TensorCodeGen *cg = getCodeGen(h);
  return cg->getNumFunctionArguments();
}

std::string TensorContext::getFunctionArgumentName(const CodeGenHandle *h,
                                                   unsigned i) const {
  const TensorCodeGen *cg = getCodeGen(h);
  return cg->getFunctionArgumentName(i);
}

std::string TensorContext::getFunctionName(const CodeGenHandle *h) const {
  const TensorCodeGen *cg = getCodeGen(h);
  return cg->getFunctionName();
}

void TensorContext::generateLibSOFile(const CodeGenHandle *h,
                                      const std::string &LibSOPath,
                                      const std::string &CSourcePath) const {
  int status;

  const pid_t pid = fork();
  if (pid == 0) {
    // in the child process:
    execlp(CCompileCommand.c_str(),
           CCompileCommand.c_str(),
           "-fpic", "-fpic", "-shared",
#ifdef NDEBUG
           "-O3",
#else
           "-O0", "-g",
#endif /* NDEBUG */
           "-o", LibSOPath.c_str(),
           CSourcePath.c_str(), nullptr);
    // 'execlp' does not return
  } else {
    assert(pid != -1);
    // in the parent process:
    waitpid(pid, &status, 0);
    // TODO this assertion fails when running in 'lldb':
    //assert(WIFEXITED(status));
  }
}

void TensorContext::initKernel(KernelHandle *h,
                               const CodeGenHandle *cg,
                               bool cleanOnDestruction) {
  *h = getHandle();
  TheKernels[*h] = new TensorKernel(this, cg, cleanOnDestruction);
}

void TensorContext::buildKernel(const KernelHandle *h) {
  TensorKernel *k = getKernel(h);
  k->build();
  k->load();
}

void TensorContext::finalKernel(const KernelHandle *h) {
  TensorKernel *k = getKernel(h);
  delete k;
  TheKernels.erase(*h);
}

unsigned TensorContext::getNumFormalArguments(const KernelHandle *h) const {
  const TensorKernel *k = getKernel(h);
  return k->getNumFormalArguments();
}

unsigned
TensorContext::getFormalArgumentPosition(const KernelHandle *h,
                                         const std::string &name) const {
  const TensorKernel *k = getKernel(h);
  return k->getFormalArgumentPosition(name);
}

void TensorContext::initExecution(ExecutionHandle *h,
                                  const KernelHandle *k) {
  *h = getHandle();
  TheExecutions[*h] = new TensorExecution(this, k);
}

void TensorContext::finalExecution(const ExecutionHandle *h) {
  TensorExecution *e = getExecution(h);
  delete e;
  TheExecutions.erase(*h);
}

void TensorContext::addArgument(const ExecutionHandle *h,
                                const std::string &name,
                                void *arg) {
  TensorExecution *e = getExecution(h);
  e->addArgument(name, arg);
}

void TensorContext::clearArguments(const ExecutionHandle *h) {
  TensorExecution *e = getExecution(h);
  e->clearArguments();
}

void TensorContext::execute(const ExecutionHandle *h) const {
  const TensorExecution *e = getExecution(h);
  e->execute();
}

std::string TensorContext::getUniqueIdentifier(const std::string &separator) {
  const std::time_t time = std::time(nullptr);

  std::stringstream ss;
  ss << std::dec << getProcessID() << separator
     << std::hex << this << separator
     << std::dec << getHandle() << separator
     << std::dec << time;

  return ss.str();
}

void *TensorContext::loadLibSOWithSymbol(const std::string &path,
                                         const std::string &name) {
  // load the shared library now since we may not want to
  // wait for it when we finally run the compiled function:
  void *dl = dlopen(path.c_str(), RTLD_NOW);
  LibSOs[path] = dl;
  return (void *)dlsym(dl, name.c_str());
}

int TensorContext::unloadLibSO(const std::string &path) {
  if (LibSOs.find(path) == LibSOs.end())
    return 0;

  int result = dlclose(LibSOs[path]);
  if (result == 0)
    // successfully unloaded the "shared object" library
    LibSOs.erase(path);

  return result;
}

int TensorContext::removeFile(const std::string &path) const {
  return remove(path.c_str());
}

