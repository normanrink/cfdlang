
#ifndef __TENSOR_GRAPH_HPP__
#define __TENSOR_GRAPH_HPP__

#include <cassert>
#include <sstream>


template<typename NodeID, typename EdgeID>
GraphNode<NodeID, EdgeID>::GraphNode(NodeID &&id, int rank)
  : ID(id), Rank(rank) {
  for (int i = 0; i < this->getRank(); i++)
    Legs.push_back(nullptr);
}

template<typename NodeID, typename EdgeID>
const GraphEdge<NodeID, EdgeID> *const &
GraphNode<NodeID, EdgeID>::at(int i) const {
  assert(i < this->getRank());
  return Legs.at(i);
}

template<typename NodeID, typename EdgeID>
const GraphEdge<NodeID, EdgeID> *&
GraphNode<NodeID, EdgeID>::operator[](int i) {
  assert(i < this->getRank());
  return Legs[i];
}

template<typename NodeID, typename EdgeID>
void
GraphNode<NodeID, EdgeID>::unset(int i) {
  assert(i < this->getRank());
  Legs[i] = nullptr;
}

template<typename NodeID, typename EdgeID>
bool
GraphNode<NodeID, EdgeID>::isSet(int i) const {
  assert(i < this->getRank());
  return Legs[i] != nullptr;
}

template<typename NodeID, typename EdgeID>
bool
GraphNode<NodeID, EdgeID>::anySet() const {
  for (int i = 0; i < this->getRank(); i++)
    if (isSet(i))
      return true;

  return false;
}

template<typename NodeID, typename EdgeID>
bool
GraphNode<NodeID, EdgeID>::anyUnset() const {
  for (int i = 0; i < this->getRank(); i++)
    if (isUnset(i))
      return true;

  return false;
}


template<typename NodeID, typename EdgeID>
GraphEdge<NodeID, EdgeID>::GraphEdge(EdgeID &&id,
                                     const NodeIndexPair &src,
                                     const NodeIndexPair &tgt)
  : ID(id), Edge(src, tgt) {}

template<typename NodeID, typename EdgeID>
GraphEdge<NodeID, EdgeID>::GraphEdge(EdgeID &&id,
                                     const GraphNode<NodeID, EdgeID> *srcNode,
                                     int srcIndex,
                                     const GraphNode<NodeID, EdgeID> *tgtNode,
                                     int tgtIndex)
  : GraphEdge(EdgeID(id),
              NodeIndexPair(srcNode, srcIndex),
              NodeIndexPair(tgtNode, tgtIndex)) {}


template<typename NodeID, typename EdgeID>
TensorGraph<NodeID, EdgeID>::~TensorGraph() {
  bool success = true;
  
  while (!Edges.empty())
    success &= eraseEdge(Edges.begin()->first);

  while (!Nodes.empty())
    success &= eraseNode(Nodes.begin()->first);

  assert(success &&
         "internal error: erasing of graph components should not fail"); 
}

template<typename NodeID, typename EdgeID>
bool
TensorGraph<NodeID, EdgeID>::isNode(const NodeID &id) {
  return Nodes.count(id);
}

template<typename NodeID, typename EdgeID>
bool
TensorGraph<NodeID, EdgeID>::isEdge(const EdgeID &id) {
  return Edges.count(id);
}

template<typename NodeID, typename EdgeID>
bool
TensorGraph<NodeID, EdgeID>::addNode(const NodeID &id, int rank) {
  if (isNode(id))
    return false;

  auto *n = new GraphNode<NodeID, EdgeID>(NodeID(id), rank);
  Nodes[id] = n;
  return true;
}

template<typename NodeID, typename EdgeID>
GraphNode<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getNode(const NodeID &id) {
  assert(isNode(id));
  return Nodes[id];
}

template<typename NodeID, typename EdgeID>
GraphNode<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getNode(const NodeID &id, int rank) {
  if (!isNode(id)) {
    addNode(id, rank);
    return getNode(id);
  }

  GraphNode<NodeID, EdgeID> *n = getNode(id);
  assert(n->getRank() == rank);
  return n;
}

template<typename NodeID, typename EdgeID>
bool
TensorGraph<NodeID, EdgeID>::eraseNode(const NodeID &id) {
  if (!isNode(id))
    return false;

  GraphNode<NodeID, EdgeID> *n = Nodes[id];
  if (n->anySet())
      // Cannot erase this node if it has outgoing edges:
      return false;

  Nodes.erase(id);
  delete n;
  return true;
}

template<typename NodeID, typename EdgeID>
bool
TensorGraph<NodeID, EdgeID>::addEdge(const EdgeID &id,
                                     const GraphNode<NodeID, EdgeID> *srcNode,
                                     int srcIndex,
                                     const GraphNode<NodeID, EdgeID> *tgtNode,
                                     int tgtIndex) {
  if (!isNode(srcNode->getID()) || !isNode(tgtNode->getID()))
    return false;
  if (srcNode->isSet(srcIndex) || tgtNode->isSet(tgtIndex))
    return false;

  auto *e = new GraphEdge<NodeID, EdgeID>(EdgeID(id),
                                          srcNode, srcIndex,
                                          tgtNode, tgtIndex);
  Edges[id] = e;
  auto *writableSrcNode = getNode(srcNode->getID());
  auto *writableTgtNode = getNode(tgtNode->getID());
  (*writableSrcNode)[srcIndex] = e;
  (*writableTgtNode)[tgtIndex] = e;
  return true;
}

template<typename NodeID, typename EdgeID>
GraphEdge<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getEdge(const EdgeID &id) {
  assert(isEdge(id));
  return Edges[id];
}

template<typename NodeID, typename EdgeID>
GraphEdge<NodeID, EdgeID> *
TensorGraph<NodeID, EdgeID>::getEdge(const EdgeID &id,
                                     const GraphNode<NodeID, EdgeID> *srcNode,
                                     int srcIndex,
                                     const GraphNode<NodeID, EdgeID> *tgtNode,
                                     int tgtIndex) {
  if (!isEdge(id))
    return addEdge(id, srcNode, srcIndex, tgtNode, tgtIndex);

  GraphEdge<NodeID, EdgeID> *e = getEdge(id);
  assert(e->getSrcNode() == srcNode && e->getSrcIndex == srcIndex &&
         e->getTgtNode() == tgtNode && e->getTgtIndex == tgtIndex);
  return e;
}

template<typename NodeID, typename EdgeID>
bool
TensorGraph<NodeID, EdgeID>::eraseEdge(const EdgeID &id) {
  if (!isEdge(id))
    return false;

  GraphEdge<NodeID, EdgeID> *e = getEdge(id);
  auto *srcNode = getNode(e->getSrcNode()->getID());
  auto *tgtNode = getNode(e->getTgtNode()->getID());
  srcNode->unset(e->getSrcIndex());
  tgtNode->unset(e->getTgtIndex());
  Edges.erase(id);
  delete e;
  return true;
}

template<typename NodeID, typename EdgeID>
void
TensorGraph<NodeID, EdgeID>::plot(std::ofstream &of) const {
  of << "graph <" << this << "> {\n";
    
  for (const auto &n : Nodes) {
    const NodeID &id = n.first;
    of << "\"" << id.str()  << "\""
       << " [label=\"" << id.getLabel() << "\"];\n";
  }

  for (const auto &e : Edges) {
    const EdgeID &id = e.first;
    const NodeID &src = e.second->getSrcNode()->getID();
    const NodeID &tgt = e.second->getTgtNode()->getID();

    of << "\"" << src.str() << "\" -- \"" << tgt.str() << "\""
       << " [label=\"" << id.getLabel() << "\"];\n";
  }

  of << "}\n";
}

#endif /* __TENSOR_GRAPH_HPP__ */

