
#ifndef __ID_FINDER_H__
#define __ID_FINDER_H__

#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


namespace CFDlang {

class IdFinder : public ExprTreeVisitor {
private:
  const ExprNode *Root;
  bool Incompatible;

  std::string NameToFind;
  bool Found;
  bool IdIncompatible;

public:
  IdFinder(const ExprNode *root)
    : Root(root) {}
  
  void visitChildren(const ExprNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++)
      en->getChild(i)->visit(this);
  }

  virtual void visitIdentifierExpr(const IdentifierExpr *en) override {
    if (en->getName() == NameToFind) {
      Found = true;
      // if the identifiers seen so far are still compatible,
      // set to 'Incompatible':
      // (make sure that 'IdIncompatible' is never unset once
      // it has been set to 'true')
      if (IdIncompatible == false)
        IdIncompatible = Incompatible;
    }
  }

  #define DEF_VISIT_COMPATIBLE_EXPR_NODE(Kind)                     \
  virtual void visit##Kind##Expr(const Kind##Expr *en) override {  \
    visitChildren(en);                                             \
  }

  DEF_VISIT_COMPATIBLE_EXPR_NODE(Add)
  DEF_VISIT_COMPATIBLE_EXPR_NODE(Sub)
  DEF_VISIT_COMPATIBLE_EXPR_NODE(Mul)
  DEF_VISIT_COMPATIBLE_EXPR_NODE(ScalarMul)
  DEF_VISIT_COMPATIBLE_EXPR_NODE(Div)
  DEF_VISIT_COMPATIBLE_EXPR_NODE(ScalarDiv)

  #undef DEF_VISIT_COMPATIBLE_EXPR_NODE

  #define DEF_VISIT_INCOMPATIBLE_EXPR_NODE(Kind)                   \
  virtual void visit##Kind##Expr(const Kind##Expr *en) override {  \
    bool savedIncompatible = Incompatible;                         \
    Incompatible = true;                                           \
    visitChildren(en);                                             \
    Incompatible = savedIncompatible;                              \
  }

  DEF_VISIT_INCOMPATIBLE_EXPR_NODE(Product)
  DEF_VISIT_INCOMPATIBLE_EXPR_NODE(Stack)
  DEF_VISIT_INCOMPATIBLE_EXPR_NODE(Transposition)
  DEF_VISIT_INCOMPATIBLE_EXPR_NODE(Contraction)

  #undef DEF_VISIT_INCOMPATIBLE_EXPR_NODE

  virtual bool find(const std::string nameToFind) {
    NameToFind = nameToFind;
    Found = false;
    IdIncompatible = false;
    Incompatible = false;
    Root->visit(this);
    return Found;
  }

  bool getIdIncompatible() const { return IdIncompatible; }
};

};

#endif /* ____ID_FINDER_H__ */
