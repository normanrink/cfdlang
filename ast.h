
#ifndef __AST_H__
#define __AST_H__


#include <string>
#include <vector>
#include <cassert>
#include <map>



enum NodeType {
  NT_Program,

  NT_DeclList,
  NT_StmtList,
  NT_ExprList,

  /* declarations: */
  NT_VarDecl,
  NT_TypeDecl,

  /* statement: */
  NT_Stmt,

  /* expressions: */
  NT_TensorExpr,
  NT_DotExpr,
  NT_Identifier, 
  NT_Integer,
  NT_BrackExpr,
  NT_ParenExpr,

  NT_DECLTYPE_COUNT
};

class Node {
private:
  NodeType NdType;

protected:
  Node(NodeType nt) : NdType(nt) {}
  virtual ~Node() {}

public:
  NodeType getNodeType() const { return NdType; };

  virtual void deepDelete() const = 0;

  virtual void print(unsigned indent = 0) const = 0;
};



/***********************/
/* template for lists: */
/***********************/

template <typename T, NodeType nt>
class NodeList : public Node {
private:
  std::vector<T *> elements;

public:
  NodeList() : Node(nt) {}
  NodeList(T *);
  virtual ~NodeList() {}

  void append(T *t) { elements.push_back(t); }
 
  int size() const { return elements.size(); } 
  
  T * operator[](int i) const { return elements.at(i); }

  virtual void deepDelete() const final;

  virtual void print(unsigned int indent = 0) const final;

  static const NodeList *create() {
    return new NodeList();
  }

  static const NodeList *create(T *t) {
    return new NodeList(t);
  }

  static NodeList *append(NodeList *l, T *t) {
    l->append(t);
    return l;
  }
};



/****************/
/* expressions: */
/****************/

class Expr : public Node {
protected:
  Expr(NodeType nt) : Node(nt) {}

public:
  virtual ~Expr() {}
};


class Factor : public Expr {
protected:
  Factor(NodeType nt) : Expr(nt) {}

public:
  virtual ~Factor() {}
};


class BinaryExpr : public Expr {
private:
  const Expr *LeftExpr;
  const Factor *RightFactor;

public:
  BinaryExpr(NodeType nt, const Expr *left, const Factor *right)
    : Expr(NT_TensorExpr), LeftExpr(left), RightFactor(right) {
    assert(nt == NT_TensorExpr || nt == NT_DotExpr);
  }

  virtual void deepDelete() const final;

  virtual void print(unsigned indent = 0) const final;

  static BinaryExpr *create(NodeType nt, const Expr *left, const Factor *right) {
    return new BinaryExpr(nt, left, right);
  }
};


class Identifier : public Factor {
private:
  const std::string Name;

public:
  Identifier(const std::string name)
    : Factor(NT_Identifier), Name(name) {}

  const std::string &getName() const { return Name; }

  virtual void deepDelete() const final {}

  virtual void print(unsigned indent = 0) const final;

  static const Identifier *create(const std::string &name) {
    return new Identifier(name);
  }
};


class Integer : public Factor {
private:
  int Value;

public:
  Integer(int value)
    : Factor(NT_Integer), Value(value) {}

  int getValue() const { return Value; }

  virtual void deepDelete() const final {}

  virtual void print(unsigned indent = 0) const final;

  static const Integer *create(int value) {
    return new Integer(value);
  }
};


typedef NodeList<const Expr, NT_ExprList> ExprList;


class BrackExpr : public Factor {
private:
  const ExprList *Exprs;

public:
  BrackExpr(const ExprList *exprs)
    : Factor(NT_BrackExpr), Exprs(exprs) {}

  virtual void deepDelete() const final;

  virtual void print(unsigned indent = 0) const final;

  static const BrackExpr *create(const ExprList *exprs) {
    return new BrackExpr(exprs);
  }
};


class ParenExpr : public Factor {
private:
  const Expr *NestedExpr;

public:
  ParenExpr(const Expr *expr)
    : Factor(NT_ParenExpr), NestedExpr(expr) {}

  virtual void deepDelete() const final;

  virtual void print(unsigned indent = 0) const final;

  static const ParenExpr *create(const Expr *expr) {
    return new ParenExpr(expr);
  }
};



/**************/
/* statement: */
/**************/

class Stmt : public Node {
private:
  const Identifier *Id;
  const Expr *RightExpr;

public:
  Stmt(const Identifier *id, const Expr *expr)
    : Node(NT_Stmt), Id(id), RightExpr(expr) {}

  virtual void deepDelete() const final;

  virtual void print(unsigned int indent = 0) const final;

  static Stmt *create(const Identifier *id, const Expr *expr) {
    return new Stmt(id, expr);
  }
};


typedef NodeList<const Stmt, NT_StmtList> StmtList;



/****************/
/* declaration: */
/****************/

class Decl : public Node {
private:
  const Identifier *Id;
  const Expr *TypeExpr;

public:
  Decl(NodeType nt, const Identifier *id, const Expr *expr) 
    : Node(nt), Id(id), TypeExpr(expr) {
    assert(nt == NT_VarDecl || nt == NT_TypeDecl);
  }

  virtual void deepDelete() const final;

  virtual void print(unsigned int indent = 0) const final;

  static const Decl *create(NodeType nt, const Identifier *id, const Expr *expr) {
    return new Decl(nt, id, expr);
  }
};


typedef NodeList<const Decl, NT_DeclList> DeclList;



/*************/
/* program:: */
/*************/

class Program : public Node {
private:
  const DeclList *Decls;
  const StmtList *Stmts;

public:
  Program(const DeclList *decls, const StmtList *stmts)
    : Node(NT_Program), Decls(decls), Stmts(stmts) {}
  
  virtual void print(unsigned indent = 0) const final;

  virtual void deepDelete() const final;

  static const Program *create(const DeclList *decls, const StmtList *stmts) {
    return new Program(decls, stmts);
  }

  static void destroy(const Program *p) {
    p->deepDelete();
    delete p;
  }
};

#endif /* __AST_H__ */

