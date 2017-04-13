
#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <cstring>
#include <list>
#include <string>


#include "CodeGen/CodeGen.h"


class Kernel {
public:
  typedef void (*CodePtr)(void **);

private:
  std::string Source;

  CodePtr Code;

  unsigned NumFormalArguments;
  std::map<std::string, unsigned> FormalArguments;

public:
  Kernel()
    : Code(nullptr), NumFormalArguments(0) {}
  ~Kernel() {}

  const std::string &getSource() const { return Source; }
  void setSource(const char *source) { Source = std::string(source); }

  const CodePtr getCode() const { return Code; }
  void setCode(const CodePtr codePtr) { Code = codePtr; }

  void addFormalArgument(const std::string &name) {
    FormalArguments[name] = NumFormalArguments++;
  }
  unsigned getNumFormalArguments() const { return NumFormalArguments; }

  unsigned getFormalArgumentPosition(const std::string &name) const {
    if (!FormalArguments.count(name))
      return -1;

    return FormalArguments.at(name);
  }
};


class Execution {
public:
  typedef void * ArgPtr;

private:
  const Kernel *TheKernel;

  const int NumArguments;

  ArgPtr *ActualArguments;

  int setArguments;

public:
  int getNumArguments() const { return NumArguments; }

  int getArgumentPosition(const std::string &name) {
    return TheKernel->getFormalArgumentPosition(name);
  }

public:
  Execution()
  : TheKernel(nullptr),
    NumArguments(0),
    ActualArguments(nullptr),
    setArguments(0) {}

  Execution(const Kernel *kernel)
  : TheKernel(kernel),
    NumArguments(TheKernel->getNumFormalArguments()),
    setArguments(0) {
    ActualArguments = new ArgPtr[getNumArguments()];
    clearArguments();
  }
  
  ~Execution() {
    if (ActualArguments != nullptr)
      delete ActualArguments;
  }

  void addArgument(const std::string &name, const ArgPtr arg) {
    assert(setArguments < getNumArguments());

    const unsigned position = getArgumentPosition(name);
    assert(position < getNumArguments());

    // increment the number of set arguments only if the argument
    // at 'position' has not previously been set:
    if (ActualArguments[position] == nullptr)
      ++setArguments;;

    ActualArguments[position] = arg;
  }

  void clearArguments() {
    std::memset(ActualArguments, 0, sizeof(ArgPtr)*getNumArguments());
    setArguments = 0;
  }

  void run() {
    assert(setArguments == getNumArguments());
    (TheKernel->getCode())(ActualArguments);
  }
};


class Context {
public:
  typedef unsigned Handle;

private:
  std::map<Handle, const Kernel *> TheKernels;

  std::map<Handle, Execution *> TheExecutions;

  unsigned NumHandles;

protected:
  unsigned getHandle() { return NumHandles++; }

public:
  Context() : NumHandles(0) {}
  ~Context();

  void compile(Handle *const h, const char *source, bool rowMajor = true);

  void addArgument(const Handle *const h, const std::string &name,
                   const Execution::ArgPtr arg) {
    assert(TheExecutions.count(*h));
    TheExecutions[*h]->addArgument(name, arg);
  }

  void execute(const Handle *const h) {
    assert(TheExecutions.count(*h));
    TheExecutions[*h]->run();
  }
};

#endif /* __CONTEXT_H__ */
