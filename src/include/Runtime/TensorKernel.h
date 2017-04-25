
#ifndef __TENSOR_KERNEL_H__
#define __TENSOR_KERNEL_H__

#include <map>
#include <string>


#include "Runtime/TensorContext.h"


class TensorKernel {
public:
  typedef void (*CodePtr)(void **);

  static unsigned UNKNOWN_FORMAL_ARG;

private:
  TensorContext *Context;

  const TensorContext::CodeGenHandle TheCodeGen;
  const bool CleanOnDestruction;

  //std::string Source;

  CodePtr Code;

  unsigned NumFormalArguments;
  std::map<std::string, unsigned> FormalArguments;

  std::string CFunctionName;
  std::string CSourceFilePath;
  std::string LibSOFilePath;

protected:
  TensorContext *getContext() { return Context; }

public:
  const TensorContext::CodeGenHandle getCodeGenHandle() const {
    return TheCodeGen;
  }

protected:
  void addFormalArgument(const std::string &name) {
    FormalArguments[name] = NumFormalArguments++;
  }

  //void setSource(const char *source) { Source = std::string(source); }
  void setCode(const CodePtr codePtr) { Code = codePtr; }

  void invalidateCode();

private:
  void reset();

public:
  TensorKernel(TensorContext *ctx,
               const TensorContext::CodeGenHandle cg,
               bool CleanOnDestruction = false);
  ~TensorKernel();

  const TensorContext *getContext() const { return Context; }
  //const std::string &getSource() const { return Source; }
  const CodePtr getCode() const { return Code; }
  bool isCodeValid() const;

  unsigned getNumFormalArguments() const { return NumFormalArguments; }
  unsigned getFormalArgumentPosition(const std::string &name) const {
    if (!FormalArguments.count(name))
      return UNKNOWN_FORMAL_ARG;

    return FormalArguments.at(name);
  }

  const std::string &getCFunctionName() const { return CFunctionName; }
  const std::string &getCSourceFilePath() const { return CSourceFilePath; }
  const std::string &getLibSOFilePath() const { return LibSOFilePath; }

  void build();
  void load();
  void unload();
  void clean();

private:
  // helpers to obtain information from the 'Context':
  std::string getUniqueIdentifier();
  const std::string &getTmpFolder() const;
};

#endif /* __CFD_KERNEL_H__ */