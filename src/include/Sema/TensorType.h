
#ifndef __TENSOR_TYPE_H__
#define __TENSOR_TYPE_H__

#include <vector>
#include <string>


namespace CFDlang {

class TensorType {
private:
  std::vector<int> Dims;

public:
  TensorType() {}
  TensorType(const std::vector<int> &dims)
    : Dims(dims) {}

  void addDim(int dim) { Dims.push_back(dim); }

  int getRank() const { return Dims.size(); }

  int getDim(unsigned i) const { return Dims.at(i); }

  bool operator==(const TensorType &rhs) const;
  bool operator!=(const TensorType &rhs) const {
    return !(*this == rhs);
  }

  bool equals(const std::vector<int> &dims) const {
    return (*this == TensorType(dims));
  }

  const std::string getDimString() const;

  const std::vector<int> &getDims() const { return Dims; }
};

};

#endif /* !__TENSOR_TYPE_H__ */
