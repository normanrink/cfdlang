
#include <assert.h>
#include <sstream>
#include <string>
#include <iostream>


#include "AST/AST.h"


namespace CFDlang {

#define EMIT_INDENT(indent) { std::cout.width((indent)); \
                              std::cout << std::left << ""; \
                              std::cout.unsetf(std::ios::adjustfield); }


std::map<ASTNode::NodeType, std::string> ASTNode::NodeLabel = {
  { NT_Program, "Program" },

  { NT_DeclList, "DeclList" },
  { NT_StmtList, "StmtList" },
  { NT_ExprList, "ExprList" },

  { NT_VarDecl, "VarDecl" },
  { NT_TypeDecl, "TypeDecl" },

  { NT_ElemDirect, "ElemDirect" },

  { NT_Stmt, "Stmt" },

  { NT_TranspositionExpr, "TranspositionExpr" },
  { NT_ContractionExpr, "ContractionExpr" },
  { NT_AddExpr, "AddExpr" },
  { NT_SubExpr, "SubExpr" },
  { NT_MulExpr, "MulExpr" },
  { NT_DivExpr, "DivExpr" },
  { NT_ProductExpr, "ProductExpr" },
  { NT_Identifier, "Identifier" },
  { NT_Integer, "Integer" },
  { NT_BrackExpr, "BrackExpr" },
  { NT_ParenExpr, "ParenExpr" },
};


void Expr::visit(ASTVisitor *v) const {
  assert(0 && "internal error: base class 'Expr' should not be visited");
}


void Factor::visit(ASTVisitor *v) const {
  assert(0 && "internal error: base class 'Factor' should not be visited");
}


void Identifier::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << " \"" << getName() << "\")\n";
}

void Identifier::visit(ASTVisitor *v) const {
  v->visitIdentifier(this);
}


void Integer::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << " \"" << getValue() << "\")\n";
}

void Integer::visit(ASTVisitor *v) const {
  v->visitInteger(this);
}


void BinaryExpr::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  LeftExpr->print(indent + str.length() + 1);
  RightExpr->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void BinaryExpr::deepDelete() const {
  LeftExpr->deepDelete();
  delete LeftExpr;

  RightExpr->deepDelete();
  delete RightExpr;
}

void BinaryExpr::visit(ASTVisitor *v) const {
  v->visitBinaryExpr(this);
}


template<typename T, ASTNode::NodeType nt, typename Derived>
ASTNodeList<T, nt, Derived>::ASTNodeList(T *t)
  : ASTNodeList() {
  append(t);
}

template<typename T, ASTNode::NodeType nt, typename Derived>
void ASTNodeList<T, nt, Derived>::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
 
  for (int i = 0; i < size(); i++)
    elements[i]->print(indent + str.length() + 1); 

  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

template<typename T, ASTNode::NodeType nt, typename Derived>
void ASTNodeList<T, nt, Derived>::deepDelete() const {
  for (int i = 0; i < size(); i++) {
    elements[i]->deepDelete();
    delete elements[i];
  }
}


template class ASTNodeList<const Decl, ASTNode::NT_DeclList, DeclList>;

void DeclList::visit(ASTVisitor *v) const {
  v->visitDeclList(this);
}


template class ASTNodeList<const Stmt, ASTNode::NT_StmtList, StmtList>;

void StmtList::visit(ASTVisitor *v) const {
  v->visitStmtList(this);
}


template class ASTNodeList<const Expr, ASTNode::NT_ExprList, ExprList>;

void ExprList::visit(ASTVisitor *v) const {
  v->visitExprList(this);
}


void Decl::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str();
  
  if (getNodeType() == NT_VarDecl) {
    if (getIOSpecifier() & IO_Input)
      std::cout << " input";
    if (getIOSpecifier() & IO_Output)
      std::cout << " output";
  }
  std::cout << "\n";

  Id->print(indent + str.length() + 1);
  TypeExpr->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Decl::deepDelete() const {
  Id->deepDelete();
  delete Id;

  TypeExpr->deepDelete();
  delete TypeExpr;
}

void Decl::visit(ASTVisitor *v) const {
  v->visitDecl(this);
}


void ElemDirect::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str();

  switch (getPOSSpecifier()) {
  case POS_First:
    std::cout << " first";
    break;
  case POS_Last:
    std::cout << " last";
    break;
  default:
    assert(0 && "internal error: invalid position specifier");
  }
  std::cout << "\n";

  I->print(indent + str.length() + 1);
  Identifiers->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1);
  std::cout << ")\n";
}

void ElemDirect::deepDelete() const {
  I->deepDelete();
  delete I;

  Identifiers->deepDelete();
  delete Identifiers;
}

void ElemDirect::visit(ASTVisitor *v) const {
  v->visitElemDirect(this);
}


void Program::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  Decls->print(indent + str.length() + 1);
  if (Elem) { Elem->print(indent + str.length() + 1); }
  Stmts->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Program::deepDelete() const {
  Decls->deepDelete();
  delete Decls;

  Stmts->deepDelete();
  delete Stmts;

  if (Elem) {
    Elem->deepDelete();
    delete Elem;
  }
}

void Program::visit(ASTVisitor *v) const {
  v->visitProgram(this);
}


void BrackExpr::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  Exprs->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void BrackExpr::deepDelete() const {
  Exprs->deepDelete();
  delete Exprs;
}

void BrackExpr::visit(ASTVisitor *v) const {
  v->visitBrackExpr(this);
}


void ParenExpr::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  NestedExpr->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void ParenExpr::deepDelete() const {
  NestedExpr->deepDelete();
  delete NestedExpr;
}

void ParenExpr::visit(ASTVisitor *v) const {
  v->visitParenExpr(this);
}


void Stmt::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  Id->print(indent + str.length() + 1);
  RightExpr->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Stmt::deepDelete() const {
  Id->deepDelete();
  delete Id;

  RightExpr->deepDelete();
  delete RightExpr;
}

void Stmt::visit(ASTVisitor *v) const {
  v->visitStmt(this);
}


template<typename T, ASTNode::NodeType nt, typename Derived>
void ASTVisitor::visitASTNodeList(const ASTNodeList<T, nt, Derived> *list) {
  for (const auto &i : *list)
    i->visit(this);
}

#define DEF_VISIT_LIST(Derived)                          \
  void ASTVisitor::visit##Derived(const Derived *list) { \
      visitASTNodeList(list);                               \
  }

DEF_VISIT_LIST(DeclList)
DEF_VISIT_LIST(StmtList)
DEF_VISIT_LIST(ExprList)

void ASTVisitor::visitProgram(const Program *p) {
  p->getDecls()->visit(this);

  // Note that the element directive may not be present in a program,
  // i.e. 'ed == nullptr' is possible:
  const ElemDirect *ed = p->getElem();
  if (ed)
    ed->visit(this);

  p->getStmts()->visit(this);
}

void ASTVisitor::visitExpr(const Expr *e) {
  e->visit(this);
}

void ASTVisitor::visitFactor(const Factor *f) {
  f->visit(this);
}

void ASTVisitor::visitParenExpr(const ParenExpr *pe) {
  pe->getExpr()->visit(this);
}

};
