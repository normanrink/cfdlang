
#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <list>
#include <map>
#include <string>
#include <vector>


#include "Sema/Sema.h"
#include "Sema/TensorType.h"


class CodeGen : public ASTVisitor {
private:
  const Sema *TheSema;

  int TempCounter;

  std::string Code;
  
  std::map<const TensorType *, std::string> EmittedTypes;

public:
  CodeGen(const Sema *sema);

  const Sema *getSema() const { return TheSema; }
  std::string getTemp();
  const std::string &getCode() const { return Code; }

  void append(const std::string &code) { Code += code; }

  bool typeEmitted(const TensorType *type) const;
  const std::string &getEmittedTypeName(const TensorType *type) const;
  void addEmittedType(const TensorType *type, const std::string &name);

// useful helpers:
public:
  typedef std::vector<int> List;
  typedef std::vector<int> Tuple;
  typedef std::vector<Tuple> TupleList;

  enum Comparison {
    CMP_Less,
    CMP_LessEqual,
    CMP_Equal,
    CMP_GreaterEqual,
    CMP_Greater,

    CMP_COMPARISON_COUNT
  };

  static bool allCompare(const List &list, Comparison cmp, int pivot);

  static bool isPairList(const TupleList &list);
  static bool partitionPairList(int pivot, const TupleList &list,
                                TupleList &left, TupleList &right,
                                TupleList &mixed);
  static void shiftList(int shiftAmount, List &list);
  static void shiftTupleList(int shiftAmount, TupleList &tuple);
  static void flattenTupleList(const TupleList &list, std::list<int> &result);
  static void unpackPairList(const TupleList &list, List &left, List &right);
  static void adjustForContractions(List &indices,
                                    const TupleList &contractions,
                                    bool up = false);

  static const std::string getTupleListString(const TupleList &list);

  static const BinaryExpr *extractTensorExprOrNull(const Expr *e);
};

#endif /* __CODEGEN_H__ */

