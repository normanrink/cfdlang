
#ifndef __EXPR_TREE_H__
#define __EXPR_TREE_H__

#include <set>
#include <string>
#include <vector>
#include <cassert>
#include <map>


#include "CodeGen/CodeGen.h"
#include "Sema/Symbol.h"


class ExprTreeVisitor;


class ExprNode {
public:
  enum ExprKind {
    EK_Add,
    EK_Sub,
    EK_Mul,
    EK_Div,
    EK_Contraction,
    EK_Product,
    EK_Stack,
    EK_Identifier,

    EK_EXPRKIND_COUNT
  };

private:
  const ExprKind ExKind;

  const int NumChildren;

  std::vector<const ExprNode *> Children;

public:
  int getNumChildren() const { return NumChildren; }

  const ExprNode *getChild(int i) const {
    assert(i < getNumChildren());
    return Children[i];
  }

public:
  ExprNode(ExprKind ek, int numChildren);
  virtual ~ExprNode() {}

  enum ExprKind getExprKind() const { return ExKind; }

protected:
  void setChild(int i, const ExprNode *en) {
    assert(i < getNumChildren());
    Children[i] = en;
  }

  static std::map<ExprKind, std::string> ExprLabel;

public:
  virtual void deepDelete() const;

  virtual void print(unsigned indent = 0) const;

  virtual void visit(ExprTreeVisitor *v) const = 0;

  virtual bool isIdentifier() const { return false; }
  virtual const Symbol *getSymbol() const { return nullptr; }
};


#define DECL_EXPR_NODE_CLASS(Kind)                     \
class Kind##Expr : public ExprNode {                   \
public:                                                \
  Kind##Expr(const ExprNode *lhs, const ExprNode *rhs) \
    : ExprNode(EK_##Kind, 2) {                         \
    setChild(0, lhs);                                  \
    setChild(1, rhs);                                  \
  }                                                    \
                                                       \
  virtual void visit(ExprTreeVisitor *v) const;        \
                                                       \
  static Kind##Expr *create(const ExprNode *lhs,       \
                            const ExprNode *rhs) {     \
    return new Kind##Expr(lhs, rhs);                   \
  }                                                    \
};

DECL_EXPR_NODE_CLASS(Add)
DECL_EXPR_NODE_CLASS(Sub)
DECL_EXPR_NODE_CLASS(Mul)
DECL_EXPR_NODE_CLASS(Div)
DECL_EXPR_NODE_CLASS(Product)

#undef DECL_EXPR_NODE_CLASS


class ContractionExpr : public ExprNode {
private:
  const CodeGen::List LeftIndices;
  const CodeGen::List RightIndices;

public:
  ContractionExpr(const ExprNode *lhs, const CodeGen::List &leftIndices,
                  const ExprNode *rhs, const CodeGen::List &rightIndices);

  const CodeGen::List &getLeftIndices() const { return LeftIndices; }
  const CodeGen::List &getRightIndices() const { return RightIndices; }

  virtual void print(unsigned indent = 0) const;

  virtual void visit(ExprTreeVisitor *v) const;

  static ContractionExpr *create(const ExprNode *lhs,
                                 const CodeGen::List &leftIndices,
                                 const ExprNode *rhs,
                                 const CodeGen::List &rightIndices) {
    return new ContractionExpr(lhs, leftIndices, rhs, rightIndices);
  }
};


class StackExpr : public ExprNode {
public:
  StackExpr(const std::vector<const ExprNode *> &members);
 
  virtual void visit(ExprTreeVisitor *v) const;

  static StackExpr *create(const std::vector<const ExprNode *> &members) {
    return new StackExpr(members);
  }
};


class IdentifierExpr : public ExprNode {
private:
  const Symbol *symbol;

public:
  IdentifierExpr(const Symbol *sym)
    : ExprNode(EK_Identifier, 0), symbol(sym) {}

  virtual bool isIdentifier() const { return true; }
  virtual const Symbol *getSymbol() const { return symbol; }

  virtual void print(unsigned indent = 0) const;
 
  virtual void visit(ExprTreeVisitor *v) const;

  static IdentifierExpr *create(const Symbol *sym) {
    return new IdentifierExpr(sym);
  }
};



    /****************************/
    /* visitor class/interface: */
    /****************************/

class ExprTreeVisitor {
public:
  #define DECL_VISIT_EXPR_NODE(Kind)                       \
  virtual void visit##Kind##Expr(const Kind##Expr *e) = 0;

  DECL_VISIT_EXPR_NODE(Add)
  DECL_VISIT_EXPR_NODE(Sub)
  DECL_VISIT_EXPR_NODE(Mul)
  DECL_VISIT_EXPR_NODE(Div)
  DECL_VISIT_EXPR_NODE(Contraction)
  DECL_VISIT_EXPR_NODE(Product)
  DECL_VISIT_EXPR_NODE(Stack)
  DECL_VISIT_EXPR_NODE(Identifier)

  #undef DECL_VISIT_EXPR_NODE
};



    /****************************************/
    /* factory class for memory management: */
    /****************************************/

class ExprNodeBuilder {
private:
  std::set<const ExprNode *> AllocatedNodes;

public:
  ~ExprNodeBuilder();

  #define DECL_BUILDER_CREATE_EXPR_NODE(Kind)                               \
  Kind##Expr *create##Kind##Expr(const ExprNode *lhs, const ExprNode *rhs);

  DECL_BUILDER_CREATE_EXPR_NODE(Add)
  DECL_BUILDER_CREATE_EXPR_NODE(Sub)
  DECL_BUILDER_CREATE_EXPR_NODE(Mul)
  DECL_BUILDER_CREATE_EXPR_NODE(Div)
  DECL_BUILDER_CREATE_EXPR_NODE(Product)

  #undef DECL_BUILDER_CREATE_EXPR_NODE

  ContractionExpr *createContractionExpr(const ExprNode *lhs,
                                         const CodeGen::List &leftIndices,
                                         const ExprNode *rhs,
                                         const CodeGen::List &rightIndices);
  StackExpr *createStackExpr(const std::vector<const ExprNode *> &);
  IdentifierExpr *createIdentifierExpr(const Symbol *);
};

#endif /* __EXPR_TREE_H__ */
