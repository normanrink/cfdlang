
#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <list>
#include <map>
#include <vector>

#include "ast.h"
#include "sema.h"


class CodeGen : public ASTVisitor {
public:
  typedef std::vector<int> List;
  typedef std::vector<int> Tuple;
  typedef std::vector<Tuple> TupleList;

  // useful helpers:
  enum Comparison {
    CMP_Less,
    CMP_LessEqual,
    CMP_Equal,
    CMP_GreaterEqual,
    CMP_Greater,

    CMP_COMPARISON_COUNT
  };

  static bool allCompare(const List &list, Comparison cmp, int pivot);

  static bool isPairList(const TupleList &list);
  static bool partitionPairList(int pivot, const TupleList &list,
                                TupleList &left, TupleList &right,
                                TupleList &mixed);
  static void shiftList(int shiftAmount,List &list);
  static void shiftTupleList(int shiftAmount, TupleList &tuple);
  static void flattenTupleList(const TupleList &list, std::list<int> &result);
  static void unpackPairList(const TupleList &list, List &left, List &right);
  static void adjustForContractions(List &indices, const TupleList &contractions);
};


class NumpyCodeGen : public CodeGen {
  const Sema *TheSema;

  std::string Code;
  
  int TempCounter;

  // map 'Expr' nodes in the AST to temporary variables:
  std::map<const Expr *, std::string> ExprTemps;

  const std::string ModulePrefix;

protected:
  const std::string getTemp() {
    return "t" + std::to_string(TempCounter++);
  }

  const std::string addTempForExpr(const Expr *e) {
    const std::string t = getTemp();
    ExprTemps[e] = t;
    return t;
  }

  const std::string addNameForExpr(const Expr *e, const std::string &name) {
    const std::string t = getTemp();
    ExprTemps[e] = name;
    return name;
  }

  const std::string getTempForExpr(const Expr *e) const {
    return ExprTemps.at(e);
  }

  const std::string &getPrefix() const { return ModulePrefix; }

public:
  NumpyCodeGen(const Sema *sema, const std::string &prefix = "np")
    : TheSema(sema), Code(""), TempCounter(0), ModulePrefix(prefix) {}

  const Sema *getSema() const { return TheSema; }
  const std::string &getCode() const { return Code; }

  void append(const std::string &code) { Code += code; }

  virtual void visitProgram(const Program *p) override;

  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  const std::string visitContraction(const Expr *e, const TupleList &indices);

  const std::string getTensorDotString(const std::string &r,
                                       const std::string &t0,
                                       const std::string &t1,
                                       const std::string axes = "");
};


class TheanoCodeGen : public NumpyCodeGen {
private:
  std::map<const TensorType *, std::string> TypeTemps;

  void constructTypes();

public:
  TheanoCodeGen(const Sema *sema, const std::string &prefix = "T")
    : NumpyCodeGen(sema, prefix) {}

  virtual void visitProgram(const Program *p) override;

  virtual void visitDecl(const Decl *d) override;
};

#endif /* !__CODEGEN_H__ */

