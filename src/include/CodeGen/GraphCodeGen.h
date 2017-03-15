
#ifndef __GRAPH_CODEGEN_H__
#define __GRAPH_CODEGEN_H__

#include <list>
#include <set>
#include <utility>
#include <vector>


#include "AST/AST.h"
#include "CodeGen/Comparable.h"
#include "CodeGen/CodeGen.h"
#include "CodeGen/TensorGraph.h"


class GraphCodeGen : public CodeGen {
public:
  typedef GraphNode<StringID, StringID> GCG_Node;
  typedef GraphEdge<StringID, StringID> GCG_Edge;
  typedef TensorGraph<StringID, StringID> GCG_Graph;

  typedef std::pair<const std::string, GCG_Graph *> GCG_LabeledGraph;
  typedef std::vector<GCG_Edge::NodeIndexPair> GCG_Legs;

  typedef std::set<const GCG_Edge *> EdgeSet;

private:
  // "map" names of temporary variable to expressions represented by graphs:
  // (Since the order is important, cannot use 'std::map' here.)
  std::list<GCG_LabeledGraph> Graphs;

  // helpers in building graphs:
  GCG_Graph *curGraph;
  GCG_Legs curLegs;
  GCG_Node *curEnd;

  void updateCurEnd(GCG_Node *n);

public:
  GraphCodeGen(const Sema *sema);
  ~GraphCodeGen();

  virtual void visitProgram(const Program *p) override;

  virtual void visitDecl(const Decl *d) override;
  virtual void visitStmt(const Stmt *s) override;

  virtual void visitBinaryExpr(const BinaryExpr *be) override;
  virtual void visitIdentifier(const Identifier *id) override;
  virtual void visitInteger(const Integer *i) override;
  virtual void visitBrackExpr(const BrackExpr *be) override;
  virtual void visitParenExpr(const ParenExpr *pe) override;

  void visitContraction(const Expr *e, const TupleList &indices);

  virtual void visitProgramPrologue(const Program *p) {}
  virtual void visitProgramEpilogue(const Program *p) {}

  virtual void visitDeclPrologue(const Decl *d) {}
  virtual void visitDeclEpilogue(const Decl *d) {}

  static void dump(const GCG_Graph &g);
  void dump() const;

  virtual void emitGraph(const std::string &name, GCG_Graph *graph);
  virtual void selectEdgesToContract(EdgeSet &result,
                                     const GCG_Graph &g) const;
  void getRemainingEdgesAtNode(EdgeSet &result, const GCG_Node &n,
                               const EdgeSet &toContract) const;
  void replaceEdgesAtNode(GCG_Graph &graph, const GCG_Node &oldNode,
                          const EdgeSet &edgesAtOldNode,
                          const GCG_Node &newNode,
                          int shift, const EdgeSet &toContract);
  virtual void
  emitContraction(const std::string &result,
                  const std::string &lhs, const List &srcIndices,
                  const std::string &rhs, const List &tgtIndices) = 0;
  virtual void
  emitTensorProduct(const std::string &result,
                    const std::string &lhs, const std::string &rhs) = 0;
  virtual void
  emitTensorStack(const std::string &result,
                  const std::list<std::string> &temps) = 0;
  virtual void
  emitAssignment(const std::string &result, const std::string &expr) = 0;
};

#endif /* !__GRAPH_CODEGEN_H__ */

