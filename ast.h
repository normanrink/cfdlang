
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

  NT_NODETYPE_COUNT
};


class ASTVisitor;


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

  virtual void visit(ASTVisitor *v) const = 0;
};



/***********************/
/* template for lists: */
/***********************/

template <typename T, NodeType nt, typename Derived>
class NodeList : public Node {
public:
  typedef std::vector<T *> Container;

private:
  Container elements;

public:
  NodeList() : Node(nt) {}
  NodeList(T *);
  virtual ~NodeList() {}

  void append(T *t) { elements.push_back(t); }
 
  int size() const { return elements.size(); } 
  
  T * operator[](int i) const { return elements.at(i); }

  virtual void deepDelete() const final;

  virtual void print(unsigned int indent = 0) const final;

  static Derived *create() {
    return new Derived();
  }

  static Derived *create(T *t) {
    return new Derived(t);
  }

  static Derived *append(Derived *l, T *t) {
    l->append(t);
    return l;
  }

  typename Container::const_iterator begin() const { return elements.begin(); }
  typename Container::const_iterator end() const { return elements.end(); }

  virtual void visit(ASTVisitor *v) const override {}
};



/****************/
/* expressions: */
/****************/

class Expr : public Node {
protected:
  Expr(NodeType nt) : Node(nt) {}

public:
  virtual ~Expr() {}

  virtual void visit(ASTVisitor *v) const override;
};


class Factor : public Expr {
protected:
  Factor(NodeType nt) : Expr(nt) {}

public:
  virtual ~Factor() {}

  virtual void visit(ASTVisitor *v) const override = 0;
};


class BinaryExpr : public Expr {
private:
  const Expr *LeftExpr;
  const Factor *RightFactor;

public:
  BinaryExpr(NodeType nt, const Expr *left, const Factor *right)
    : Expr(nt), LeftExpr(left), RightFactor(right) {
    assert(nt == NT_TensorExpr || nt == NT_DotExpr);
  }

  const Expr *getLeft() const { return LeftExpr; }
  const Factor *getRight() const { return RightFactor; }

  virtual void deepDelete() const final;

  virtual void print(unsigned indent = 0) const final;

  static BinaryExpr *create(NodeType nt, const Expr *left, const Factor *right) {
    return new BinaryExpr(nt, left, right);
  }

  virtual void visit(ASTVisitor *v) const override;
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
  
  virtual void visit(ASTVisitor *v) const override;
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

  virtual void visit(ASTVisitor *v) const override;
};


class ExprList : public NodeList<const Expr, NT_ExprList, ExprList> {
public:
  ExprList() : NodeList() {}
  ExprList(const Expr *e) : NodeList(e) {}

  virtual void visit(ASTVisitor *v) const override;
};


class BrackExpr : public Factor {
private:
  const ExprList *Exprs;

public:
  BrackExpr(const ExprList *exprs)
    : Factor(NT_BrackExpr), Exprs(exprs) {}

  const ExprList *getExprs() const { return Exprs; }

  virtual void deepDelete() const final;

  virtual void print(unsigned indent = 0) const final;

  static const BrackExpr *create(const ExprList *exprs) {
    return new BrackExpr(exprs);
  }

  virtual void visit(ASTVisitor *v) const override;
};


class ParenExpr : public Factor {
private:
  const Expr *NestedExpr;

public:
  ParenExpr(const Expr *expr)
    : Factor(NT_ParenExpr), NestedExpr(expr) {}

  const Expr *getExpr() const { return NestedExpr; }

  virtual void deepDelete() const final;

  virtual void print(unsigned indent = 0) const final;

  static const ParenExpr *create(const Expr *expr) {
    return new ParenExpr(expr);
  }

  virtual void visit(ASTVisitor *v) const override;
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

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getExpr() const { return RightExpr; }

  virtual void deepDelete() const final;

  virtual void print(unsigned int indent = 0) const final;

  static Stmt *create(const Identifier *id, const Expr *expr) {
    return new Stmt(id, expr);
  }

  virtual void visit(ASTVisitor *v) const override;
};


class StmtList : public NodeList<const Stmt, NT_StmtList, StmtList> {
public:
  StmtList() : NodeList() {}
  StmtList(const Stmt *s) : NodeList(s) {}

  virtual void visit(ASTVisitor *v) const override;
};



/****************/
/* declaration: */
/****************/

class Decl : public Node {
public:
  enum IOSpecifier {
    IO_Empty  = 0,
    IO_Input  = 1 << 0,
    IO_Output = 1 << 1,
  };

private:
  const Identifier *Id;
  const Expr *TypeExpr;

  const IOSpecifier IOSpec;

public:

  Decl(NodeType nt, const Identifier *id, const Expr *expr,
       IOSpecifier iospec = IO_Empty) 
    : Node(nt), Id(id), TypeExpr(expr), IOSpec(iospec) {
    assert(nt == NT_VarDecl || nt == NT_TypeDecl);
  }

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getTypeExpr() const { return TypeExpr; }
  const IOSpecifier getIOSpecifier() const { return IOSpec; }

  virtual void deepDelete() const final;

  virtual void print(unsigned int indent = 0) const final;

  static const Decl *create(NodeType nt, const Identifier *id,
                            const Expr *expr, IOSpecifier iospec = IO_Empty) {
    return new Decl(nt, id, expr, iospec);
  }
  
  virtual void visit(ASTVisitor *v) const override;
};


class DeclList : public NodeList<const Decl, NT_DeclList, DeclList> {
public:
  DeclList() : NodeList() {}
  DeclList(const Decl *d) : NodeList(d) {}

  virtual void visit(ASTVisitor *v) const override;
};



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
  
  const DeclList *getDecls()  const { return Decls; }
  const StmtList *getStmts()  const { return Stmts; }

  virtual void print(unsigned indent = 0) const final;

  virtual void deepDelete() const final;

  static const Program *create(const DeclList *decls, const StmtList *stmts) {
    return new Program(decls, stmts);
  }

  static void destroy(const Program *p) {
    p->deepDelete();
    delete p;
  }

  virtual void visit(ASTVisitor *v) const override;
};



/****************************/
/* visitor class/interface: */
/****************************/

class ASTVisitor {
public:
  virtual void visitProgram(const Program *);

  template<typename T, NodeType nt, typename Derived>
  void visitNodeList(const NodeList<T, nt, Derived> *list);

  #define DECL_VISIT_LIST(Derived)                    \
    virtual void visit##Derived(const Derived *list);

  DECL_VISIT_LIST(DeclList)
  DECL_VISIT_LIST(StmtList)
  DECL_VISIT_LIST(ExprList)

  virtual void visitDecl(const Decl *) = 0;

  virtual void visitStmt(const Stmt *) = 0;

  virtual void visitExpr(const Expr *);
  virtual void visitFactor(const Factor *);
  virtual void visitBinaryExpr(const BinaryExpr *) = 0;
  virtual void visitIdentifier(const Identifier *) = 0;
  virtual void visitInteger(const Integer *) = 0;
  virtual void visitBrackExpr(const BrackExpr *) = 0;
  virtual void visitParenExpr(const ParenExpr *);
};

#endif /* __AST_H__ */

