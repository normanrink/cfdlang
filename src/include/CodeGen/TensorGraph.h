
#ifndef __TENSOR_GRAPH_H__
#define __TENSOR_GRAPH_H__

#include <fstream>
#include <map>
#include <utility>
#include <vector>


#include "CodeGen/Comparable.h"


template<typename NodeID, typename EdgeID> class GraphEdge;
template<typename NodeID, typename EdgeID> class TensorGraph;


template<typename NodeID, typename EdgeID>
class GraphNode : public Comparable<GraphNode<NodeID, EdgeID>> {
private:
  NodeID ID;

  const int Rank;

  std::vector<const GraphEdge<NodeID, EdgeID> *> Legs;

  struct {
    GraphNode<NodeID, EdgeID> *Pred;
    GraphNode<NodeID, EdgeID> *Succ;
  } Seq;

public:
  GraphNode(NodeID &&id, int rank);

  const NodeID &getID() const { return ID; }
  NodeID &getID() { return ID; }

  int getRank() const { return Rank; }

  const GraphEdge<NodeID, EdgeID> *const &at(int i) const;
  const GraphEdge<NodeID, EdgeID> *& operator[](int i);

  void unset(int i);

  bool isSet(int i) const;
  bool isUnset(int i) const { return !isSet(i); }
  bool anySet() const;
  bool anyUnset() const;
  bool allSet() const { return !anyUnset(); }
  bool allUnset() const { return !anySet(); }

  int countSet() const;

  const GraphNode<NodeID, EdgeID> *getPred() const { return Seq.Pred; }
  GraphNode<NodeID, EdgeID> *getPred() { return Seq.Pred; }
  const GraphNode<NodeID, EdgeID> *getSucc() const { return Seq.Succ; }
  GraphNode<NodeID, EdgeID> *getSucc() { return Seq.Succ; }

  void setPred(GraphNode<NodeID, EdgeID> *n) { Seq.Pred = n; }
  void setSucc(GraphNode<NodeID, EdgeID> *n) { Seq.Succ = n; }

  void updateSequence(GraphNode<NodeID, EdgeID> *pred,
                      GraphNode<NodeID, EdgeID> *succ);
  
  bool hasPred() const { return (getPred() != nullptr); }
  bool hasSucc() const { return (getSucc() != nullptr); }

  virtual bool operator==(const GraphNode<NodeID, EdgeID> &rhs) const final {
    return getID() == rhs.getID();
  }
  virtual bool operator<(const GraphNode<NodeID, EdgeID> &rhs) const final {
    return getID() < rhs.getID();
  }
};


template<typename NodeID, typename EdgeID>
class GraphEdge : public Comparable<GraphEdge<NodeID, EdgeID>> {
public:
  typedef std::pair<const GraphNode<NodeID, EdgeID> *, int> NodeIndexPair;
    
private:
  EdgeID ID;

  std::pair<NodeIndexPair, NodeIndexPair> Edge;

public:
  GraphEdge(EdgeID &&id,
            const NodeIndexPair &src, const NodeIndexPair &tgt);
  GraphEdge(EdgeID &&id,
            const GraphNode<NodeID, EdgeID> *srcNode, int srcIndex,
            const GraphNode<NodeID, EdgeID> *tgtNode, int tgtIndex);

  const EdgeID &getID() const { return ID; }
  EdgeID &getID() { return ID; }

  const GraphNode<NodeID, EdgeID> *getSrcNode() const {
    return Edge.first.first;
  }
  int getSrcIndex() const { return Edge.first.second; }
  const NodeID &getSrcID() const { return getSrcNode()->getID(); }

  const GraphNode<NodeID, EdgeID> *getTgtNode() const {
    return Edge.second.first;
  }
  int getTgtIndex() const { return Edge.second.second; }
  const NodeID &getTgtID() const { return getTgtNode()->getID(); }

  virtual bool operator==(const GraphEdge<NodeID, EdgeID> &rhs) const final {
    return getID() == rhs.getID();
  }
  virtual bool operator<(const GraphEdge<NodeID, EdgeID> &rhs) const final {
    return getID() < rhs.getID();
  }
};


template<typename NodeID, typename EdgeID>
class TensorGraph {
public:
  typedef std::map<NodeID, GraphNode<NodeID, EdgeID> *> NodeMap;
  typedef std::map<EdgeID, GraphEdge<NodeID, EdgeID> *> EdgeMap;

private:
  NodeMap Nodes;
  EdgeMap Edges;

public:
  TensorGraph() {}
  ~TensorGraph();

  const NodeMap &getNodes() const { return Nodes; }
  const EdgeMap &getEdges() const { return Edges; }

  bool empty() const;
  int getNumEdges() const;
  int getNumEdges(const NodeID &id) const;

  void getEdgesFromNode(EdgeMap &result,
                        const GraphNode<NodeID, EdgeID> *n) const;
  void getEdgesToNode(EdgeMap &result,
                      const GraphNode<NodeID, EdgeID> *n) const;
  void getEdgesBetweenNodes(EdgeMap &result,
                            const GraphNode<NodeID, EdgeID> *src,
                            const GraphNode<NodeID, EdgeID> *tgt) const;

  bool isNode(const NodeID &id) const;
  bool isEdge(const EdgeID &id) const;

  bool addNode(const NodeID &id, int rank);
  const GraphNode<NodeID, EdgeID> *getNode(const NodeID &id) const;
  GraphNode<NodeID, EdgeID> *getNode(const NodeID &id);
  GraphNode<NodeID, EdgeID> *getNode(const NodeID &id, int rank);
  bool eraseNode(const NodeID &id);

  bool addEdge(const EdgeID &id,
               const GraphNode<NodeID, EdgeID> *srcNode, int srcIndex,
               const GraphNode<NodeID, EdgeID> *tgtNode, int tgtIndex);
  bool addEdge(const GraphEdge<NodeID, EdgeID> &e);
  const GraphEdge<NodeID, EdgeID> *getEdge(const EdgeID &id) const;
  GraphEdge<NodeID, EdgeID> *getEdge(const EdgeID &id);
  GraphEdge<NodeID, EdgeID> *getEdge(const EdgeID &id,
                                     const GraphNode<NodeID, EdgeID> *srcNode,
                                     int srcIndex,
                                     const GraphNode<NodeID, EdgeID> *tgtNode,
                                     int tgtIndex);
  bool eraseEdge(const EdgeID &id);

  void plot(std::ofstream &of) const;

  typename NodeMap::const_iterator nodes_begin() const { return Nodes.begin(); }
  typename NodeMap::const_iterator nodes_end() const { return Nodes.end(); }
  typename EdgeMap::const_iterator edges_begin() const { return Edges.begin(); }
  typename EdgeMap::const_iterator edges_end() const { return Edges.end(); }

  const GraphNode<NodeID, EdgeID> *getStartNode() const;
};


#include "CodeGen/TensorGraph.hpp"

#endif /* __TENSOR_GRAPH_H__ */

