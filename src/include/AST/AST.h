
#ifndef __AST_H__
#define __AST_H__


#include <string>
#include <vector>
#include <cassert>
#include <map>


class ASTVisitor;


class ASTNode {
public:
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
    NT_TranspositionExpr,
    NT_ContractionExpr,
    NT_AddExpr,
    NT_SubExpr,
    NT_MulExpr,
    NT_DivExpr,
    NT_ProductExpr,
    NT_Identifier, 
    NT_Integer,
    NT_BrackExpr,
    NT_ParenExpr,

    NT_NODETYPE_COUNT
  };

private:
  NodeType NdType;

protected:
  ASTNode(NodeType nt) : NdType(nt) {}
  virtual ~ASTNode() {}

protected:
  static std::map<NodeType, std::string> NodeLabel;

public:
  NodeType getNodeType() const { return NdType; };

  virtual void deepDelete() const = 0;

  virtual void print(unsigned indent = 0) const = 0;

  virtual void visit(ASTVisitor *v) const = 0;
};



/***********************/
/* template for lists: */
/***********************/

template <typename T, ASTNode::NodeType nt, typename Derived>
class ASTNodeList : public ASTNode {
public:
  typedef std::vector<T *> Container;

private:
  Container elements;

public:
  ASTNodeList() : ASTNode(nt) {}
  ASTNodeList(T *);
  virtual ~ASTNodeList() {}

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

class Expr : public ASTNode {
protected:
  Expr(NodeType nt) : ASTNode(nt) {}

public:
  virtual ~Expr() {}

  virtual void visit(ASTVisitor *v) const override;

  virtual bool isIdentifier() const { return false; }
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
  const Expr *RightExpr;

public:
  BinaryExpr(NodeType nt, const Expr *left, const Expr *right)
    : Expr(nt), LeftExpr(left), RightExpr(right) {
    assert(nt == NT_TranspositionExpr ||
           nt == NT_ContractionExpr ||
           nt == NT_AddExpr ||
           nt == NT_SubExpr ||
           nt == NT_MulExpr ||
           nt == NT_DivExpr ||
           nt == NT_ProductExpr);
  }

  const Expr *getLeft() const { return LeftExpr; }
  const Expr *getRight() const { return RightExpr; }

  virtual void deepDelete() const final;

  virtual void print(unsigned indent = 0) const final;

  static BinaryExpr *create(NodeType nt,
                            const Expr *left, const Expr *right) {
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

  virtual bool isIdentifier() const override { return true; }
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


class ExprList : public ASTNodeList<const Expr,
                                    ASTNode::NT_ExprList,
                                    ExprList> {
public:
  ExprList() : ASTNodeList() {}
  ExprList(const Expr *e) : ASTNodeList(e) {}

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

class Stmt : public ASTNode {
private:
  const Identifier *Id;
  const Expr *RightExpr;

public:
  Stmt(const Identifier *id, const Expr *expr)
    : ASTNode(NT_Stmt), Id(id), RightExpr(expr) {}

  const Identifier *getIdentifier() const { return Id; }
  const Expr *getExpr() const { return RightExpr; }

  virtual void deepDelete() const final;

  virtual void print(unsigned int indent = 0) const final;

  static Stmt *create(const Identifier *id, const Expr *expr) {
    return new Stmt(id, expr);
  }

  virtual void visit(ASTVisitor *v) const override;
};


class StmtList : public ASTNodeList<const Stmt,
                                    ASTNode::NT_StmtList,
                                    StmtList> {
public:
  StmtList() : ASTNodeList() {}
  StmtList(const Stmt *s) : ASTNodeList(s) {}

  virtual void visit(ASTVisitor *v) const override;
};



/****************/
/* declaration: */
/****************/

class Decl : public ASTNode {
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
    : ASTNode(nt), Id(id), TypeExpr(expr), IOSpec(iospec) {
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


class DeclList : public ASTNodeList<const Decl,
                                    ASTNode::NT_DeclList,
                                    DeclList> {
public:
  DeclList() : ASTNodeList() {}
  DeclList(const Decl *d) : ASTNodeList(d) {}

  virtual void visit(ASTVisitor *v) const override;
};



/*************/
/* program:: */
/*************/

class Program : public ASTNode {
private:
  const DeclList *Decls;
  const StmtList *Stmts;

public:
  Program(const DeclList *decls, const StmtList *stmts)
    : ASTNode(NT_Program), Decls(decls), Stmts(stmts) {}
  
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

  template<typename T, ASTNode::NodeType nt, typename Derived>
  void visitASTNodeList(const ASTNodeList<T, nt, Derived> *list);

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

