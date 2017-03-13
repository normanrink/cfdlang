
#ifndef __TENSOR_GRAPH_H__
#define __TENSOR_GRAPH_H__

#include <fstream>
#include <map>
#include <utility>
#include <vector>
#include <sstream>
#include <string>


template<typename NodeID, typename EdgeID> class GraphEdge;
template<typename NodeID, typename EdgeID> class TensorGraph;


template<typename NodeID, typename EdgeID>
class GraphNode {
private:
  NodeID ID;

  const int Rank;

  std::vector<const GraphEdge<NodeID, EdgeID> *> Legs;

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
};


template<typename NodeID, typename EdgeID>
class GraphEdge {
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

  const GraphNode<NodeID, EdgeID> *getTgtNode() const {
    return Edge.second.first;
  }
  int getTgtIndex() const { return Edge.second.second; }
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

  bool isNode(const NodeID &id);
  bool isEdge(const EdgeID &id);

  bool addNode(const NodeID &id, int rank);
  GraphNode<NodeID, EdgeID> *getNode(const NodeID &id);
  GraphNode<NodeID, EdgeID> *getNode(const NodeID &id, int rank);
  bool eraseNode(const NodeID &id);

  bool addEdge(const EdgeID &id,
               const GraphNode<NodeID, EdgeID> *srcNode, int srcIndex,
               const GraphNode<NodeID, EdgeID> *tgtNode, int tgtIndex);
  GraphEdge<NodeID, EdgeID> *getEdge(const EdgeID &id);
  GraphEdge<NodeID, EdgeID> *getEdge(const EdgeID &id,
                                     const GraphNode<NodeID, EdgeID> *srcNode,
                                     int srcIndex,
                                     const GraphNode<NodeID, EdgeID> *tgtNode,
                                     int tgtIndex);


  bool eraseEdge(const EdgeID &id);


  void plot(std::ofstream &of) const;
};


template<typename Derived>
class GraphComponentID {
private:
  const std::string Label;

public:
  GraphComponentID(const std::string label = "")
    : Label(label) {}

  const std::string &getLabel() const { return Label; }

  virtual const std::string str() const = 0;

  virtual bool operator==(const Derived &id) const = 0;
  virtual bool operator!=(const Derived &id) const {
    return !(*this == id);
  }
  virtual bool operator<(const Derived &id) const = 0;
  virtual bool operator<=(const Derived &id) const {
    return (*this < id) || (*this == id);
  }
  virtual bool operator>=(const Derived &id) const {
    return !(*this < id);
  }
  virtual bool operator>(const Derived &id) const {
    return (*this >= id) && (*this != id);
  }
};


class StringID : public GraphComponentID<StringID> {
private:
  const std::string ID;

public:
  StringID(std::string &&id, const std::string label = "")
    : GraphComponentID<StringID>(label), ID(id) {}

  StringID(const StringID &rhs)
    : GraphComponentID<StringID>(rhs.getLabel()), ID(rhs.str()) {}

  const std::string str() const final { return ID; }
  bool operator==(const StringID &id) const final {
    return ID == id.str();
  }
  bool operator<(const StringID &id) const final {
    return ID < id.str();
  }
};


class AddressID : public GraphComponentID<AddressID> {
private:
  const void *ID;

public:
  AddressID(const void *id, const std::string label = "")
    : GraphComponentID<AddressID>(label), ID(id) {}

  const void *get() const { return ID; }

  AddressID(const AddressID &rhs, const std::string label = "")
    : GraphComponentID<AddressID>(rhs.getLabel()), ID(rhs.get()) {}

  const std::string str() const {
    std::stringstream ss;
    ss << std::hex << ID;
    return ss.str();
  }

  bool operator==(const AddressID &id) const final { return ID == id.get(); }
  bool operator<(const AddressID &id) const final { return ID < id.get(); }
};

#include "CodeGen/TensorGraph.hpp"

#endif /* __TENSOR_GRAPH_H__ */

