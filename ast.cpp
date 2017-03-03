
#include <assert.h>
#include <sstream>
#include <string>
#include <iostream>


#include "ast.h"


#define EMIT_INDENT(indent) { std::cout.width((indent)); \
                              std::cout << std::left << ""; \
                              std::cout.unsetf(std::ios::adjustfield); }


static std::map<NodeType, std::string> NodeLabel = {
  { NT_Program, "Program" },

  { NT_DeclList, "DeclList" },
  { NT_StmtList, "StmtList" },
  { NT_ExprList, "ExprList" },

  { NT_VarDecl, "VarDecl" },
  { NT_TypeDecl, "TypeDecl" },

  { NT_Stmt, "Stmt" },

  { NT_TensorExpr, "TensorExpr" },
  { NT_DotExpr, "DotExpr" },
  { NT_Identifier, "Identifier" },
  { NT_Integer, "Integer" },
  { NT_BrackExpr, "BrackExpr" },
  { NT_BrackExpr, "ParenExpr" },
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
  RightFactor->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void BinaryExpr::deepDelete() const {
  LeftExpr->deepDelete();
  delete LeftExpr;

  RightFactor->deepDelete();
  delete RightFactor;
}

void BinaryExpr::visit(ASTVisitor *v) const {
  v->visitBinaryExpr(this);
}


template<typename T, NodeType nt, typename Derived>
NodeList<T, nt, Derived>::NodeList(T *t)
  : NodeList() {
  append(t);
}

template<typename T, NodeType nt, typename Derived>
void NodeList<T, nt, Derived>::print(unsigned indent) const {
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

template<typename T, NodeType nt, typename Derived>
void NodeList<T, nt, Derived>::deepDelete() const {
  for (int i = 0; i < size(); i++) {
    elements[i]->deepDelete();
    delete elements[i];
  }
}


template class NodeList<Decl, NT_DeclList, DeclList>;

void DeclList::visit(ASTVisitor *v) const {
  v->visitDeclList(this);
}


template class NodeList<Stmt, NT_StmtList, StmtList>;

void StmtList::visit(ASTVisitor *v) const {
  v->visitStmtList(this);
}


template class NodeList<Expr, NT_ExprList, ExprList>;

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


void Program::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  std::stringstream ss;
  ss << " <" << std::hex << this << ">";

  EMIT_INDENT(indent)
  std::cout << "(" << str << ss.str() << "\n";
  Decls->print(indent + str.length() + 1);
  Stmts->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void Program::deepDelete() const {
  Decls->deepDelete();
  delete Decls;

  Stmts->deepDelete();
  delete Stmts;
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


template<typename T, NodeType nt, typename Derived>
void ASTVisitor::visitNodeList(const NodeList<T, nt, Derived> *list) {
  for (const auto &i : *list)
    i->visit(this);
}

#define DEF_VISIT_LIST(Derived)                          \
  void ASTVisitor::visit##Derived(const Derived *list) { \
      visitNodeList(list);                               \
  }

DEF_VISIT_LIST(DeclList)
DEF_VISIT_LIST(StmtList)
DEF_VISIT_LIST(ExprList)

void ASTVisitor::visitProgram(const Program *p) {
  p->getDecls()->visit(this);
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

