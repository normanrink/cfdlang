
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


void Identifier::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << " \"" << getName() << "\")\n";
}


void Integer::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << " \"" << getValue() << "\")\n";
}


void BinaryExpr::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
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

template<typename T, NodeType nt>
NodeList<T, nt>::NodeList(T *t)
  : NodeList() {
  append(t);
}

template<typename T, NodeType nt>
void NodeList<T, nt>::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
 
  for (int i = 0; i < size(); i++)
    elements[i]->print(indent + str.length() + 1); 

  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

template<typename T, NodeType nt>
void NodeList<T, nt>::deepDelete() const {
  for (int i = 0; i < size(); i++) {
    elements[i]->deepDelete();
    delete elements[i];
  }
}


template class NodeList<const Decl, NT_DeclList>;
template class NodeList<const Stmt, NT_StmtList>;
template class NodeList<const Expr, NT_ExprList>;


void Decl::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
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


void Program::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
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


void BrackExpr::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
  Exprs->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void BrackExpr::deepDelete() const {
  Exprs->deepDelete();
  delete Exprs;
}


void ParenExpr::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
  NestedExpr->print(indent + str.length() + 1);
  EMIT_INDENT(indent + 1)
  std::cout << ")\n";
}

void ParenExpr::deepDelete() const {
  NestedExpr->deepDelete();
  delete NestedExpr;
}


void Stmt::print(unsigned indent) const {
  std::string str = NodeLabel[getNodeType()];

  EMIT_INDENT(indent)
  std::cout << "(" << str << "\n";
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


