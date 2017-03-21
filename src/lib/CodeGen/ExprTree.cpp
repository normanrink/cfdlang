
#include <assert.h>
#include <sstream>
#include <string>
#include <iostream>


#include "CodeGen/ExprTree.h"


#define EMIT_INDENT(indent) { std::cout.width((indent)); \
                              std::cout << std::left << ""; \
                              std::cout.unsetf(std::ios::adjustfield); }


std::map<ExprNode::ExprKind, std::string> ExprNode::ExprLabel = {
  { EK_Add, "Add" },
  { EK_Sub, "Sub" },
  { EK_Mul, "Mul" },
  { EK_Div, "Div" },
  { EK_Contraction, "Contraction" },
  { EK_Product, "Product" },
  { EK_Stack, "Stack" },
  { EK_Identifier, "Identifier"}
};


ExprNode::ExprNode(ExprKind ek, int numChildren)
  : ExKind(ek), NumChildren(numChildren) {
    for (int i = 0; i < getNumChildren(); i++)
      Children.push_back(nullptr);
}

void ExprNode::print(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  for (int i = 0; i < getNumChildren(); i++)
    getChild(i)->print(indent + str.length() + 1);

  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void ExprNode::deepDelete() const {
  for (int i = 0; i < getNumChildren(); i++) {
    getChild(i)->deepDelete();
    delete getChild(i);
  }
}


#define DEF_EXPR_NODE_CLASS_VISIT(Kind)            \
void Kind##Expr::visit(ExprTreeVisitor *v) const { \
  v->visit##Kind##Expr(this);                      \
}

DEF_EXPR_NODE_CLASS_VISIT(Add)
DEF_EXPR_NODE_CLASS_VISIT(Sub)
DEF_EXPR_NODE_CLASS_VISIT(Mul)
DEF_EXPR_NODE_CLASS_VISIT(Div)
DEF_EXPR_NODE_CLASS_VISIT(Product)

#undef DEF_EXPR_NODE_CLASS_VISIT


ContractionExpr::ContractionExpr(const ExprNode *lhs,
                                 const CodeGen::List &leftIndices,
                                 const ExprNode *rhs,
                                 const CodeGen::List &rightIndices)
  : ExprNode(EK_Contraction, 2),
    LeftIndices(leftIndices),
    RightIndices(rightIndices) {
  setChild(0, lhs);
  setChild(1, rhs);
}

void ContractionExpr::print(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";

  getChild(1)->print(indent + str.length() + 1);
  EMIT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getListString(getLeftIndices()) << "\n";

  getChild(0)->print(indent + str.length() + 1);
  EMIT_INDENT(indent + str.length() + 1)
  std::cout << CodeGen::getListString(getRightIndices()) << "\n";

  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void ContractionExpr::visit(ExprTreeVisitor *v) const {
  v->visitContractionExpr(this);
}


StackExpr::StackExpr(const std::vector<const ExprNode *> &members)
  : ExprNode(EK_Stack, members.size()) {
    for (int i = 0; i < members.size(); i++)
      setChild(i, members[i]);
}

void StackExpr::visit(ExprTreeVisitor *v) const {
  v->visitStackExpr(this);
}


void IdentifierExpr::print(unsigned indent) const {
  std::string str = ExprLabel[getExprKind()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str()
            << " \"" << getSymbol()->getName() << "\")\n";
}

void IdentifierExpr::visit(ExprTreeVisitor *v) const {
  v->visitIdentifierExpr(this);
}


ExprNodeBuilder::~ExprNodeBuilder() {
  for (auto *en: AllocatedNodes)
    delete en;
}

#define DEF_BUILDER_CREATE_EXPR_NODE(Kind)                             \
Kind##Expr *ExprNodeBuilder::create##Kind##Expr(const ExprNode *lhs,   \
                                                const ExprNode *rhs) { \
  Kind##Expr *result = Kind ##Expr::create(lhs, rhs);                  \
  AllocatedNodes.insert(result);                                       \
  return result;                                                       \
}

DEF_BUILDER_CREATE_EXPR_NODE(Add)
DEF_BUILDER_CREATE_EXPR_NODE(Sub)
DEF_BUILDER_CREATE_EXPR_NODE(Mul)
DEF_BUILDER_CREATE_EXPR_NODE(Div)
DEF_BUILDER_CREATE_EXPR_NODE(Product)

#undef DEF_BUILDER_CREATE_EXPR_NODE

ContractionExpr *
ExprNodeBuilder::createContractionExpr(const ExprNode *lhs,
                                       const CodeGen::List &leftIndices,
                                       const ExprNode *rhs,
                                       const CodeGen::List &rightIndices) {
  ContractionExpr *result = ContractionExpr::create(lhs, leftIndices,
                                                    rhs, rightIndices);
  AllocatedNodes.insert(result);
  return result;
}

StackExpr *
ExprNodeBuilder::createStackExpr(const std::vector<const ExprNode *> &members) {

  StackExpr *result = StackExpr::create(members);
  AllocatedNodes.insert(result);
  return result;
}

IdentifierExpr *ExprNodeBuilder::createIdentifierExpr(const Symbol *sym) {
  IdentifierExpr *result = IdentifierExpr::create(sym);
  AllocatedNodes.insert(result);
  return result;
}
