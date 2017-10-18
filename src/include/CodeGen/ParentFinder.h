
#ifndef __PARENT_FINDER_H__
#define __PARENT_FINDER_H__


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


class ParentFinder : public ExprTreeVisitor {
private:
  const ExprNode *Root;

  const ExprNode *NodeToFind;

  const ExprNode *Parent;
  bool Found;

public:
  ParentFinder(const ExprNode *root)
    : Root(root) {}
  
  void visitChildren(const ExprNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++) {
      Parent = en;
      en->getChild(i)->visit(this);
      if (Found)
        return;
    }
  }

  virtual void visitIdentifierExpr(const IdentifierExpr *en) override {
    if (en == NodeToFind) {
      Found = true;
    }
  }

  #define DEF_VISIT_EXPR_NODE(Kind)                                \
  virtual void visit##Kind##Expr(const Kind##Expr *en) override {  \
    if (en == NodeToFind) {                                        \
      Found = true;                                                \
      return;                                                      \
    }                                                              \
    visitChildren(en);                                             \
  }

  DEF_VISIT_EXPR_NODE(Add)
  DEF_VISIT_EXPR_NODE(Sub)
  DEF_VISIT_EXPR_NODE(Mul)
  DEF_VISIT_EXPR_NODE(ScalarMul)
  DEF_VISIT_EXPR_NODE(Div)
  DEF_VISIT_EXPR_NODE(ScalarDiv)
  DEF_VISIT_EXPR_NODE(Product)
  DEF_VISIT_EXPR_NODE(Stack)
  DEF_VISIT_EXPR_NODE(Transposition)
  DEF_VISIT_EXPR_NODE(Contraction)

  #undef DEF_VISIT_EXPR_NODE

  virtual const ExprNode *find(const ExprNode* nodeToFind) {
    NodeToFind = nodeToFind;
    Parent = nullptr;
    Found = false;
    Root->visit(this);
    return Parent;
  }
};

#endif /* ____PARENT_FINDER_H__ */
