
#ifndef __GRAPH_H__
#define __GRAPH_H__

#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <utility>


class Edge {
public:
  typedef std::string Node;
  typedef std::pair<const Node, int> NdIdxPair;
    
private:
  std::pair<NdIdxPair, NdIdxPair> E;

public:
  Edge(const NdIdxPair &src, const NdIdxPair &tgt)
    : E(src, tgt) {}

  const std::string &getSrcLabel() const { return E.first.first; }
  int getSrcIndex() const { return E.first.second; }

  const std::string &getTgtLabel() const { return E.second.first; }
  int getTgtIndex() const { return E.second.second; }
};


class Graph {
public:
  typedef std::set<const Edge *> EdgeSet;

private:
  EdgeSet ES;

public:
  Graph() {}

  const EdgeSet &getEdges() const { return ES; }

  void getNodeLabels(std::set<std::string> &labels) {
    labels.clear();
    for (auto &e : getEdges()) {
      const std::string l0 = e->getSrcLabel();
      const std::string l1 = e->getTgtLabel();

      if (!labels.count(l0)) labels.insert(l0);
      if (!labels.count(l1)) labels.insert(l1);
    }
  }

  void addEdge(const Edge::NdIdxPair &src, const Edge::NdIdxPair &tgt) {
    const Edge *e = new Edge(src, tgt);
    ES.insert(e);
  }

  void deleteEdge(const Edge *e) {
    if (!ES.count(e))
      return;

    ES.erase(ES.find(e));
    delete e;
  }

  ~Graph() {
    while (!ES.empty())
      deleteEdge(*ES.begin());
  }

  Graph operator+(const Graph &rhs) const {
    Graph result;

    for (const auto &e : this->getEdges())
      result.ES.insert(e);
    for (const auto &e : rhs.getEdges())
      result.ES.insert(e);

    return result;
  }

  void plot() const {
    const std::string name = std::tmpnam(nullptr);
    std::ofstream of(name);
    std::cout << "Writing graph to file \'" << name << "\' ... \n";

    of << "graph " << std::to_string(this) << "{\n";
    
    std::set<std::string> nodes;
    getNodeLabels(nodes);
    for (const auto &n : nodes)
      of << n << " [label=\"" << n << "\"];\n";

    for (const auto &e : getEdges()) {
      const std::string l0 = e->getSrcLabel();
      int i0 = e->getSrcIndex();

      const std::string l1 = e->getTgtLabel();
      int i1 = e->getTgtIndex();

      of << l0 << " -- " << l1 << " [label=\""
         << "(" << l0 << ", " << l1 << "):"
         << "(" << std::to_string(i0) << ", "
         << std::to_string(i1) << ")\"];\n";
    }

    of << "}\n";
    of.close();
  }
};

#endif /* __GRAPH_H__ */

