
#ifndef __GRAPH_CODEGEN_H__
#define __GRAPH_CODEGEN_H__

#include <list>
#include <set>
#include <utility>
#include <vector>


#include "AST/AST.h"
#include "CodeGen/Comparable.h"
#include "CodeGen/CodeGen.h"
#include "CodeGen/ExprTree.h"
#include "CodeGen/TensorGraph.h"


class GraphCodeGen : public CodeGen {
public:
  typedef AddressID<ExprNode> NodeID;
  typedef StringID EdgeID;

  typedef GraphNode<NodeID, EdgeID> GCG_Node;
  typedef GraphEdge<NodeID, EdgeID> GCG_Edge;
  typedef TensorGraph<NodeID, EdgeID> GCG_Graph;

  typedef std::vector<GCG_Edge::NodeIndexPair> GCG_Legs;

  typedef std::set<const GCG_Edge *> EdgeSet;

private:
  // helpers in building graphs:
  GCG_Graph *curGraph;
  GCG_Legs curLegs;
  GCG_Node *curEnd;

  void updateCurEnd(GCG_Node *n);

  // map 'Exprs' in the AST to graphs representing the 'Expr':
  // (Note that not all 'Expr' fomr the AST will end up in this
  // map. Mot notably the 'Expr' from the RHS of a 'Stmt' will
  // be put in this map.)
  std::map<const Expr *, const GCG_Graph *> ExprGraphs;

  // keep track of allocated graphs:
  std::set<const GCG_Graph *> Graphs;

public:
  GraphCodeGen(const Sema *sema);

public:
  virtual void visitStmt(const Stmt *) override;

  virtual void visitBinaryExpr(const BinaryExpr *) override;
  virtual void visitIdentifier(const Identifier *) override;
  virtual void visitInteger(const Integer *) override;
  virtual void visitBrackExpr(const BrackExpr *) override;
  virtual void visitParenExpr(const ParenExpr *) override;

  static void dump(const GCG_Graph &g);

private:
  void visitContraction(const Expr *e, const TupleList &indices);

  void buildExprTreeForExpr(const Expr *);
  const ExprNode *buildExprTreeForGraph(GCG_Graph *);

  void selectEdgesToContract(EdgeSet &result, const GCG_Graph &g) const;
  void getRemainingEdgesAtNode(EdgeSet &result, const GCG_Node &n,
                               const EdgeSet &toContract) const;
  void replaceEdgesAtNode(GCG_Graph &graph, const GCG_Node &oldNode,
                          const EdgeSet &edgesAtOldNode,
                          const GCG_Node &newNode,
                          int shift, const EdgeSet &toContract);
};

#endif /* !__GRAPH_CODEGEN_H__ */
