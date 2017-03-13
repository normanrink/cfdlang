
#ifndef __GRAPH_CODEGEN_H__
#define __GRAPH_CODEGEN_H__

#include <list>
#include <utility>
#include <vector>


#include "AST/AST.h"
#include "CodeGen/CodeGen.h"
#include "CodeGen/TensorGraph.h"


//template class GraphNode<StringID, StringID>;
//template class GraphEdge<StringID, StringID>;
//template class TensorGraph<StringID, StringID>;


class GraphCodeGen : public CodeGen {
public:
  typedef GraphNode<StringID, StringID> GCG_Node;
  typedef GraphEdge<StringID, StringID> GCG_Edge;
  typedef TensorGraph<StringID, StringID> GCG_Graph;

  typedef std::pair<const std::string, GCG_Graph *> GCG_LabeledGraph;
  typedef std::vector<GCG_Edge::NodeIndexPair> GCG_Legs;

private:
  // "map" names of temporary variable to expressions represented by graphs:
  // (Since the order is important, hence cannot use 'std::map'.)
  std::list<GCG_LabeledGraph> Graphs;

  // helpers in building graphs:
  GCG_Graph *curGraph;
  GCG_Legs curLegs;

public:
  GraphCodeGen(const Sema *sema);
  ~GraphCodeGen();

  virtual void visitProgram(const Program *p) override;

  virtual void visitDecl(const Decl *d) override {}
  virtual void visitStmt(const Stmt *s) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  void visitContraction(const Expr *e, const TupleList &indices);
};

#endif /* !__GRAPH_CODEGEN_H__ */

