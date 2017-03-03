
#ifndef __TENSOR_GRAPH_H__
#define __TENSOR_GRAPH_H__

#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <utility>


struct GraphEdge;
struct TensorGraph;

struct GraphNode {
  const std::string Label;
  const int Rank;
  std::vector<const GraphEdge *> Legs;

  GraphNode(const std::string &label, int rank)
    : Label(label), Rank(rank) {
    for (int i = 0; i < Rank; i++)
      Legs.push_back(nullptr);
  }

  bool valid() const;
  
  const std::string &getLabel() const { return Label; }
};

struct GraphEdge {
public:
  typedef std::pair<const void *, int> NdIdxPair;
    
  std::pair<NdIdxPair, NdIdxPair> E;

public:
  GraphEdge(const NdIdxPair &src, const NdIdxPair &tgt)
    : E(src, tgt) {}

  const void *getSrcId() const { return E.first.first; }
  const std::string &getSrcLabel(const TensorGraph *TG) const;
  int getSrcIndex() const { return E.first.second; }

  const void *getTgtId() const { return E.second.first; }
  const std::string &getTgtLabel(const TensorGraph *TG) const;
  int getTgtIndex() const { return E.second.second; }
};


struct TensorGraph {
public:
  typedef std::map<const void *, GraphNode *> NodeMap;
  typedef std::set<const GraphEdge *> EdgeSet;

  NodeMap Nodes;
  EdgeSet Edges;

  bool existsNode(const void *id) {
    return Nodes.count(id);
  }

  void addNode(const void *id, const std::string &label, int rank) {
    assert(Nodes.count(id) == 0);

    GraphNode *n = new GraphNode(label, rank);
    Nodes[id] = n;
  }

  GraphNode *getNode(const void *id, const std::string &label, int rank) {
    if (!existsNode(id))
      addNode(id, label, rank);

    return Nodes[id];
  }

  void addEdge(GraphEdge::NdIdxPair src,
               GraphEdge::NdIdxPair tgt) {
    assert(Nodes.count(src.first) == 1);
    assert(Nodes.count(tgt.first) == 1);

    GraphEdge *e = new GraphEdge(src, tgt);
    Edges.insert(e);

    Nodes[src.first]->Legs[src.second] = e;
    Nodes[tgt.first]->Legs[tgt.second] = e;
  }

  ~TensorGraph() {
    for (const auto &e : Edges) delete e;
    for (const auto &n : Nodes) delete n.second;
  }

  void plot() const {
    const std::string name = std::string(std::tmpnam(nullptr)) + ".dot";
    std::ofstream of(name);
    std::cout << "Writing graph to file \'" << name << "\' ... \n";

    of << "graph <" << this << "> {\n";
    
    for (const auto &n : Nodes) {
      const void *id = n.first;
      const GraphNode *gn = n.second;
      of << "<" << id << ">"
         << " [label=\"<" << gn->getLabel() << "," << id << ">\"];\n";
    }

    for (const auto &e : Edges) {
      const void *id0 = e->getSrcId();
      const std::string l0 = Nodes.at(id0)->getLabel();
      int i0 = e->getSrcIndex();

      const void *id1 = e->getTgtId();
      const std::string l1 = Nodes.at(id1)->getLabel();
      int i1 = e->getTgtIndex();

      of << "<"<< id0 << ">" << " -- " << "<" << id1 << ">"
         << " [label=\"" << "(" << l0 << ":" << i0 << "),"
                         << "(" << l1 << ":" << i1 << ")\"];\n";
    }

    of << "}\n";
    of.close();
  }
};

#endif /* __TENSOR_GRAPH_H__ */

