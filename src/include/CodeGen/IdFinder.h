
#ifndef __ID_FINDER_H__
#define __ID_FINDER_H__

#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


class IdFinder : public ExprTreeVisitor {
private:
  const ExprNode *Root;

  std::string NameToFind;
  bool Found;

public:
  IdFinder(const ExprNode *root)
    : Root(root) {}
  
  void visitChildren(const ExprNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++)
      en->getChild(i)->visit(this);
  }

  virtual void visitIdentifierExpr(const IdentifierExpr *en) override {
    if (en->getName() == NameToFind)
      Found = true;
  }

  #define DEF_VISIT_EXPR_NODE(Kind)                                \
  virtual void visit##Kind##Expr(const Kind##Expr *en) override {  \
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

  #undef DECL_VISIT_EXPR_NODE

  virtual bool find(const std::string nameToFind) {
    NameToFind = nameToFind;
    Found = false;
    Root->visit(this);
    return Found;
  }
};

#endif /* ____ID_FINDER_H__ */
