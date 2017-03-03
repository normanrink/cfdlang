
#include "graph2.h"


bool GraphNode::valid() const {
  if (Out.size() != Dim)
    return false;

  for (int i = 0; i < Dim; i++) {
    const auto &e = Legs[i];
    if (e == nullptr)
      continue;
    if (e->getSrcLabel() != Label && e->getTgtLabel() != Label)
      return false;
    if (e->getSrcLabel() == Label && e->getSrcIndex() != i)
      return false;
    if (e->getTgtLabel() == Label && e->getTgtIndex() != i)
      return false;
  }

  return true;
}

const std::string &GraphEdge::getSrcLabel(const TensorGraph *TG) const {
  return TG->Nodes[getSrcId()]->getLabel();
}

const std::string &GraphEdge::getTgtLabel(const TensorGraph *TG) const {
  return TG->Nodes[getTgtId()]->getLabel();
}
