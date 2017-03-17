
#ifndef __PYTHON_FRAGMENT_BUILDER_H__
#define __PYTHON_FRAGMENT_BUILDER_H__

#include <string>


#include "AST/AST.h"
#include "CodeGen/CodeGen.h"


class PythonFragBuilder {
private:
  const std::string ModulePrefix;

  std::string Fragment;

protected:
  const std::string &getModulePrefix() const { return ModulePrefix; }

public:
  PythonFragBuilder(const std::string &prefix);

  const std::string &getFragment() const { return Fragment; }

  void buildNumpyProgramPrologue(const CodeGen *cg);
  void buildTheanoProgramPrologue(const CodeGen *cg);

  void buildNumpyDeclEpilogue(const Decl *d, const CodeGen *cg);
  void buildTheanoDeclEpilogue(const Decl *d, CodeGen *cg);

  void buildStmtEpilogue(const Stmt *s, const DirectCodeGen *cg);

  void buildTensorDotString(const std::string &result,
                            const std::string &lhs,
                            const std::string &rhs,
                            const std::string &axes = "");

  void buildContractionEpilogue(const std::string &result,
                                const std::string &lhs,
                                const std::string &rhs, 
                                const CodeGen::TupleList &LeftAndRightIndices);

  void buildBinOpExprEpilogue(const BinaryExpr *be, const std::string &op,
                              const DirectCodeGen *cg);
  void buildAddExprEpilogue(const BinaryExpr *be, const DirectCodeGen *cg);
  void buildSubExprEpilogue(const BinaryExpr *be, const DirectCodeGen *cg);
  void buildMulExprEpilogue(const BinaryExpr *be, const DirectCodeGen *cg);
  void buildDivExprEpilogue(const BinaryExpr *be, const DirectCodeGen *cg);
  void buildProductExprEpilogue(const BinaryExpr *be, const DirectCodeGen *cg);

  void buildBrackExprEpilogue(const BrackExpr *be, const DirectCodeGen *cg);

  void buildTheanoProgramEpilogue(CodeGen *cg);

  void buildContraction(const std::string &result,
                        const std::string &lhs,
                        const CodeGen::List &lhsIndices,
                        const std::string &rhs,
                        const CodeGen::List &rhsIndices);

  void buildTensorProduct(const std::string &result,
                          const std::string &lhs, const std::string &rhs);

  void buildTensorStack(const std::string &result,
                        const std::list<std::string> &temps);

  void buildAssignment(const std::string &result, const std::string &expr);

  void buildBinOpExpr(const std::string &result, const std::string &op,
                      const std::string &lhs, const std::string &rhs);
  void buildAddExpr(const std::string &result,
                    const std::string &lhs, const std::string &rhs);
  void buildSubExpr(const std::string &result,
                    const std::string &lhs, const std::string &rhs);
  void buildMulExpr(const std::string &result,
                    const std::string &lhs, const std::string &rhs);
  void buildDivExpr(const std::string &result,
                    const std::string &lhs, const std::string &rhs);
};

#endif /* __PYTHON_FRAGMENT_BUILDER_H__ */

