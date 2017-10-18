
#ifndef __INDEX_MAPPER_H__
#define __INDEX_MAPPER_H__

#include <map>
#include <set>
#include <string>


#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"


class IndexMapper : public ExprTreeVisitor {
public:
  typedef std::vector<std::string> IndexSetTy;
  typedef std::map<const IdentifierExpr *, IndexSetTy> IdExprToIdxSetMapTy;

private:
  IdExprToIdxSetMapTy IdExprToIdxSetMap;

  const ExprNode *Root;

  int Counter;

  const std::string Dummy = "D";

  IndexSetTy exprIndices;

private:
  std::string getIndex() { return std::to_string(Counter++); }

  void initIndices() {
    for (auto d: Root->getDims()) {
      exprIndices.push_back(getIndex());
    }
  }

public:
  IndexMapper(const ExprNode *root, const IndexSetTy &indices)
  : Root(root)
  , exprIndices(indices) {}

  void run() {
    //Counter = 0;
    //initIndices();

    Root->visit(this);
  }

  const IndexSetTy &getIndices() const { return exprIndices; }
  const IdExprToIdxSetMapTy &getIdToIdxSetMap() const {
    return IdExprToIdxSetMap;
  }

  void visitChildren(const ExprNode *en) {
    for (int i = 0; i < en->getNumChildren(); i++)
      en->getChild(i)->visit(this);
  }

  virtual void visitIdentifierExpr(const IdentifierExpr *en) override {
    IdExprToIdxSetMap[en] = exprIndices;
  }

  #define DEF_VISIT_EXPR_NODE(Kind)                                \
  virtual void visit##Kind##Expr(const Kind##Expr *en) override {  \
    visitChildren(en);                                             \
  }

  DEF_VISIT_EXPR_NODE(Add)
  DEF_VISIT_EXPR_NODE(Sub)
  DEF_VISIT_EXPR_NODE(Mul)
  DEF_VISIT_EXPR_NODE(Div)

  #undef DEF_VISIT_EXPR_NODE

  virtual void visitScalarMulExpr(const ScalarMulExpr *en) override {
    en->getChild(1)->visit(this);
  }

  virtual void visitScalarDivExpr(const ScalarDivExpr *en) override {
    en->getChild(0)->visit(this);
  }

  #define DECL_VISIT_EXPR_NODE(Kind)                                \
  virtual void visit##Kind##Expr(const Kind##Expr *en) override;

  DECL_VISIT_EXPR_NODE(Product)
  DECL_VISIT_EXPR_NODE(Stack)
  DECL_VISIT_EXPR_NODE(Transposition)
  DECL_VISIT_EXPR_NODE(Contraction)

  #undef DECL_VISIT_EXPR_NODE

  IndexSetTy getLhsIndices(const ExprNode *en) const;

  static bool areIndicesAdjacent(const std::string &i0,
                                 const std::string &i1,
                                 const IndexSetTy &indices);

  bool areIndicesAlwaysAdjacent(const std::string &i0,
                                const std::string &i1) const;
};

#endif /* ____ID_COLLECTOR_H__ */
