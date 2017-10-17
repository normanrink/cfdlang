
#ifndef __ID_COLLECTOR_H__
#define __ID_COLLECTOR_H__

#include <map>
#include <set>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


class IdCollector : public ExprTreeVisitor {
public:
  typedef std::set<const IdentifierExpr *> IdExprSetTy;
  typedef std::map<std::string, IdExprSetTy> IdMapTy;

private:
  std::set<std::string> Ids;

  IdMapTy IdToExprsMap;

public:
  IdCollector() {
    Ids.clear();
    IdToExprMap.clear();
  }

  const std::set<std::string> &getIds() const { return Ids; }
  const IdentifierExpr *getIdExpr(const std::string &name) const {
    return IdToExprsMap[name];
  }
  
  void visitChildren(const ExprNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++)
      en->getChild(i)->visit(this);
  }

  virtual void visitIdentifierExpr(const IdentifierExpr *en) override {
    const std::string &name = en->getName();
    if (Ids.find(name) == Ids.end()) {
      Ids.insert(name);
    }

    IdExprSetTy &exprs = IdToExprsMap[name];
    if (exprs.find(en) == exprs.end()) {
      expr.insert(en);
    }
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

  #undef DEF_VISIT_INCOMPATIBLE_EXPR_NODE

};

#endif /* ____ID_COLLECTOR_H__ */
