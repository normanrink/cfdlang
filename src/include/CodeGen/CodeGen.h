
#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <list>
#include <map>
#include <string>
#include <vector>


#include "Sema/Sema.h"
#include "Sema/TensorType.h"


namespace CFDlang {

class ExprNode;
class ExprNodeBuilder;


class CodeGen : public ASTVisitor {
public:
  // a program consists of delcared identifiers ...
  typedef std::list<ExprNode *> DeclaredIdListTy;

  // .. and assignments:
  struct Assignment {
    ExprNode *lhs;
    ExprNode *rhs;
  };
  typedef std::list<Assignment> AssignmentsListTy;

  // additionally, a program has input and output, both of which
  // are implemented as function arguments:
  struct FunctionArgument {
    int position;
    std::string name;
  };
  typedef std::vector<FunctionArgument> FunctionArgumentsListTy;

private:
  // member variables that capture the state of this object:
  const Sema *TheSema;

  int TempCounter;
  std::string Code;

  const std::string FunctionName;

public:
  // methods for accessing and manipulating this object's state:
  const Sema *getSema() const { return TheSema; }
  std::string getTemp();
  const std::string &getCode() const { return Code; }

  void append(const std::string &code) { Code += code; }

  const std::string &getFunctionName() const { return FunctionName; }

public:
  // constructor and deconstructor:
  CodeGen(const Sema *sema, const std::string &functionName);
  virtual ~CodeGen();

protected:
  // state that is built up during code generation:

  // for creating new 'ExprNode' objects:
  ExprNodeBuilder *ENBuilder;

  // the three components of the resulting program:
  DeclaredIdListTy DeclaredIds;
  AssignmentsListTy Assignments;
  FunctionArgumentsListTy FunctionArguments;

  // each 'Expr' in the AST ismapped to an 'ExprNode':
  std::map<const Expr *, ExprNode *> ExprTrees;
  void EXPR_TREE_MAP_ASSERT(const Expr *expr) const ;

protected:
  void addExprNode(const Expr *expr, ExprNode *en) {
    ExprTrees[expr] = en;
  }
  ExprNode *getExprNode(const Expr *expr) const {
    return ExprTrees.at(expr);
  }

public:
  // methods for accessing and manipulating state
  // that is built up during code generation:

  ExprNodeBuilder *getENBuilder() { return ENBuilder; }

  virtual void addDeclaredId(const Decl *d);
  virtual void addAssignment(const Stmt *s);
  virtual void addFunctionArgument(const std::string &name);

  DeclaredIdListTy &getDeclaredIds() { return DeclaredIds; }
  AssignmentsListTy &getAssignments() { return Assignments; }
  FunctionArgumentsListTy &getFunctionArguments() { return FunctionArguments; }

  const DeclaredIdListTy &getDeclaredIds() const { return DeclaredIds; }
  const AssignmentsListTy &getAssignments() const { return Assignments; }
  const FunctionArgumentsListTy &getFunctionArguments() const {
    return FunctionArguments;
  };

  unsigned getNumFunctionArguments() const {
    return FunctionArguments.size();
  };
  const FunctionArgument &getFunctionArgument(unsigned i) const;

public:
  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

public:
  // useful helpers:
  typedef std::vector<int> List;
  typedef std::vector<int> Tuple;
  typedef std::vector<Tuple> TupleList;

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
  static void shiftList(int shiftAmount, List &list);
  static void shiftTupleList(int shiftAmount, TupleList &tuple);
  static void flattenTupleList(const TupleList &list, std::list<int> &result);
  static void unpackPairList(const TupleList &list, List &left, List &right);
  static void adjustForContractions(List &indices,
                                    const TupleList &contractions);

  static const std::string getListString(const List &list);
  static const std::string getTupleListString(const TupleList &list);

  static const BinaryExpr *extractTensorExprOrNull(const Expr *e);

public:
  typedef std::vector<int> DimsTy;
  typedef std::vector<std::string> TempVecTy;

private:
  std::map<DimsTy, TempVecTy> FreeTemps;

public:
  std::string getTempWithDims(const DimsTy &dims) {
    TempVecTy &temps = FreeTemps[dims];

    //if (temps.size() == 0) {
      return getTemp();
    //} else {
    //  auto result = temps.back();
    //  temps.pop_back();
    //  return result;
    //}
  }

  void freeTempWithDims(const std::string &name, const DimsTy &dims) {
    TempVecTy &temps = FreeTemps[dims];
    temps.push_back(name);
  }
};

};

#endif /* __CODEGEN_H__ */
