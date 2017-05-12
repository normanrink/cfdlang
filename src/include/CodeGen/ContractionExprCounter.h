
#ifndef __CONTRACTION_EXPR_COUNTER_H__
#define __CONTRACTION_EXPR_COUNTER_H__

#include <map>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


class ContractionExprCounter : public ExprTreeVisitor {
private:
  const ExprNode *Root;

  unsigned Counter;
  const ExprNode *Deepest;

public:
  ContractionExprCounter(const ExprNode *root)
    : Root(root) {}
  
  void visitChildren(const ExprNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++)
      en->getChild(i)->visit(this);
  }

  virtual void visitContractionExpr(const ContractionExpr *en) override {
    ++Counter;
    Deepest = en;
    visitChildren(en);
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
  DEF_VISIT_EXPR_NODE(Identifier)

  #undef DECL_VISIT_EXPR_NODE

  virtual void run() {
    Counter = 0;
    Deepest = nullptr;
    Root->visit(this);
  }

  unsigned getCount() const {
    return Counter;
  }
  
  const ExprNode* getDeepest() const {
    return Deepest;
  }
};

#endif /* __CONTRACTION_EXPR_COLLECTOR_H__ */
