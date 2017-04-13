
#include <dlfcn.h>
#include <unistd.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <fstream>


#include "AST/AST.h"
#include "Compile/Context.h"
#include "CodeGen/GraphCodeGen.h"
#include "CodeGen/CEmitter.h"
#include "Parse/Parser.h"
#include "Sema/Sema.h"


#define TMP_FOLDER     (std::string("/tmp"))
#define CFDLANG_PREFIX (std::string("cfdlang"))


Context::~Context() {
  for (auto &eit: TheExecutions) delete eit.second;
  for (auto &kit: TheKernels) delete kit.second;
}

void Context::compile(Handle *const h, const char *source, bool rowMajor) {
  const std::string CC = std::getenv("CFDLANG_CC");

  const std::string FunctionName = CFDLANG_PREFIX +
                                   "_" + std::to_string(getHandle());

  const std::string CFileName = TMP_FOLDER + "/" + CFDLANG_PREFIX +
                                "-C-" + std::to_string(getHandle()) + ".c";
  const std::string SOFileName = TMP_FOLDER + "/" + CFDLANG_PREFIX +
                                 "-SO-" + std::to_string(getHandle()) + ".so";

  Kernel *K = new Kernel;
  {
    Parser parser(source);
    const int parserResult = parser.parse();
    assert(!parserResult);

    Sema sema;
    sema.visitProgram(parser.getAST());

    GraphCodeGen GCG(&sema, FunctionName);
    CEmitter emitter(&GCG, rowMajor, /* emitWrapper= */ true);
    emitter.codeGen(parser.getAST());

    Program::destroy(parser.getAST());

    K->setSource(emitter.getCode().c_str());

    std::ofstream COutFile(CFileName);
    COutFile << K->getSource();
    COutFile.close();

    int status;
    const pid_t pid = fork();
    if (pid == 0) {
      // in the child process:
      execlp(CC.c_str(),
             CC.c_str(),
             "-fpic", "-fpic", "-shared",
#ifdef NDEBUG
             "-O3",
#else
             "-O0", "-g",
#endif /* NDEBUG */
             "-o", SOFileName.c_str(),
             CFileName.c_str(), nullptr);
      // 'execlp' does not return
    } else {
      assert(pid != -1);
      // in the parent process:
      waitpid(pid, &status, 0);
      // TODO this assertion fails when running in 'lldb':
      //assert(WIFEXITED(status));
    }
    // the child process does not get here

    // load the shared library now since we may not want to
    // wait for it when we finally run the compiled function:
    void *dlhandle = dlopen(SOFileName.c_str(), RTLD_NOW);
    Kernel::CodePtr code =
      (Kernel::CodePtr)dlsym(dlhandle, FunctionName.c_str());

    K->setCode(code);

    for (const auto &formal: GCG.getFunctionArguments())
      K->addFormalArgument(formal.name);
  }

  TheKernels[getHandle()] = K;

  *h = getHandle();
  TheExecutions[*h] = new Execution(K);
}
