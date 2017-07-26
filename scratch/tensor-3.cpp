/* ----------------------------- tensor-3.cpp --------------------------------*/
/* This file is not part of the 'CFDlang' compiler or library. It is merely
 * intended as a (partial and incomplete) study into how template meta-
 * programming can be used to express and set up tensor operations.
 *
 * This is work in progress.                                                  */
/* ---------------------------------------------------------------------------*/

#include <iostream>

#include <type_traits>


/* ---------------------------------------------------------------------------*/
/* 'TType' describes the type of a tensor, which is determined by
 *    (1) the tensor's rank (template parameter 'N'),
 *    (2) the type of elements in the tensor (template parameter 'ET'),
 *    (3) the size of the tensor's 1st dimension (template parameter 'd'),
 *    (4) the type of the tensor's "tail", i.e. dimensions 2--N (paramter 'T').
 *
 * Note that the indexing of the dimensions of a tensor starts at zero.
 * Hence, the template parameter 'd' describes the size of the tensor's
 * dimension at index zero. (In mathematical terms, this is the 1st dimension.)
 *
 * Note that tensor types are built up recursively: the type of the tensor's
 * "tail" must be a tensor type too, i.e. template parameter 'T' must also
 * be a 'TType'.
 *
 */
template<int N, typename ET, int d, typename T>
struct TType {
  using TTailType = T;
  static constexpr int rank = N;
  static constexpr int dim = d;
  static constexpr int num = d*TTailType::num;

  using ElementType = ET;

  static constexpr int size(int i) {
    return (i == 0) ? dim : TTailType::size(i-1);
  }
};

// Note that for a 1-dimensional tensor, the "tail type" must be
// be the same type as that of a scalar, i.e. the element type 'ET'.
template<typename ET, int d, typename T>
struct TType<1, ET, d, T> {
  using TTailType = T;
  static constexpr int rank = 1;
  static constexpr int dim = d;
  static constexpr int num = d;

  using ElementType = ET;

  TType() { static_assert(std::is_same<T, ET>::value, ""); }

  static constexpr int size(int i) {
    /* Note that Argument 'i' cannot be evaluated statically: */
    /* static_assert(i == 0, ""); */
    return dim;
  }
};
/* ---------------------------------------------------------------------------*
 *
 * ---------------------------------------------------------------------------*/
/* Builders for tensor types.
 * Each builder struct defines 'type' which is the built 'TType'.
 *
 * 'TProductTypeBuilder' builds the type of the outer product:
 *  (tensor of type 'T0') \otimes (tensor of type 'T1').
 *
 * 'TContractionTypeBuilder' builds resulting type when tensors of type 'T0'
 *  and type 'T1' are contracted along dimensions 'd0' and 'd1'. (Note that
 *  dimensions are indexed starting from zero.)
 */
template<typename T0, typename T1>
struct TProductTypeBuilder {
private:
  using tail = TProductTypeBuilder<typename T0::TTailType, T1>;

public:
  using type = TType<(T0::rank + T1::rank), typename T0::ET, T0::dim, tail>;

  TProductTypeBuilder() {
    using T0_ET = typename T0::ElementType;
    using T1_ET = typename T1::ElementType;
    static_assert(std::is_same<T0_ET, T1_ET>::value, "");
  }
};

template<typename ET, int d, typename T1>
struct TProductTypeBuilder<TType<1, ET, d, ET>, T1> {
  using type = TType<(T1::rank + 1), ET, d, T1>;

  TProductTypeBuilder() {
    using T1_ET = typename T1::ElementType;
    static_assert(std::is_same<ET, T1_ET>::value, "");
  }
};


/* forward declaration of the general 'TContractionTypeBuilder':              
 */
template<typename T0, int d0, typename T1, int d1>
struct TContractionTypeBuilder;

/* 'TContractionTypeBuilder':
 * specialization to the case where the 0-th dimension of 'T0'
 * is contracted with a 1-dimensional tensor (i.e. a vector).
 */
template<typename T0, int d>
struct TContractionTypeBuilder
<T0, 0, TType<1, typename T0::ElementType, d, typename T0::ElementType>, 0> {
public:
  using type = typename T0::TTailType;

  /* Note that this spezialization of 'TContractionTypeBuilder' also
   * covers the case 'T0::dim == 1', when 'T0::Tail == ET' must hold. */
  TContractionTypeBuilder() {
    static_assert(T0::dim != 1 ||
                  std::is_same<typename T0::TTailType,
                               typename T0::ElementType>::value, "");
  }
};

/* 'TContractionTypeBuilder':
 * specialization to the case where any dimension of 'T0' is
 * contracted with a 1-dimensional tensor (i.e. a vector).
 */
template<typename T0, int d0, int d>
struct TContractionTypeBuilder
<T0, d0, TType<1, typename T0::ElementType, d, typename T0::ElementType>, 0> {
private:
  using ET = typename T0::ElementType;
  using T0_Tail = typename T0::TTailType;

  /* The case 'd0 == 0' is handled by the previous template specialization,
   * hence we can assume that 'd0 > 0'. Therefore, first contract the
   * "tail" of 'T0' with 1-dimensional tensor: */
  using tail = typename
    TContractionTypeBuilder<T0_Tail, (d0-1), TType<1, ET, d, ET>, 0>::type;

  static constexpr bool tailIsET = std::is_same<tail, ET>::value;

public:
  /* Form the outer product of the 0-th dimension of 'T0' with 'tail': */
  using type = typename
    std::conditional<tailIsET,
                     TType<1, ET, d, ET>,
                     TProductTypeBuilder<TType<1, ET, d, ET>, tail>>::type;

  TContractionTypeBuilder() {
    /* Since 'd0 > 0' must hold, must also have 'T0::dim > 0'. Note again that
     * the case 'd0 == 0' is handled by the previous template specialization, */
    static_assert(T0::dim > 0, "");
  }
};

/* 'TContractionTypeBuilder': general case
 */
template<typename T0, int d0, typename T1, int d1>
struct TContractionTypeBuilder {
private:
  using ET = typename T0::ElementType;
  using T1_Tail = typename T1::TTailType;

  using front = typename
    TProductTypeBuilder<T0, TType<1, ET, T1::dim, ET>>::type;

public:
  // CHECK: The case 'd1 == 0' does not seem to be handled by the
  // previous specializations for general 'T0'.
  using type = typename
    TContractionTypeBuilder<front, d0, T1_Tail, (d1-1)>::type;

  TContractionTypeBuilder() {
    using T0_ET = typename T0::ElementType;
    using T1_ET = typename T1::ElementType;
    static_assert(std::is_same<T0_ET, T1_ET>::value, "");

    /* The case 'T1::dim == 0' is handled by the two specializations above. */
    static_assert(T1::dim > 0, "");
  }
};


/* 'TTypeBuilder': convert a template parameter pack into a nested 'TType'.
 */
template<int N, typename ET, int d, int... ds>
struct TTypeBuilder {
private:
  using tail = typename TTypeBuilder<N-1, ET, ds...>::type;

public:
  using type = TType<N, ET, d, tail>;
};

template<typename ET, int d>
struct TTypeBuilder<1, ET, d> {
  using type = TType<1, ET, d, ET>;
};
/* ---------------------------------------------------------------------------*
 *
 * ---------------------------------------------------------------------------*/
/* 'MultiIndex' holds pointers to a tuple of 'N' indices of type 'I'.
 */
template<int N, typename I>
class MultiIndex {
  const I *data[N];

public:
  MultiIndex() {}

  MultiIndex(std::initializer_list<const I *> init) {
    int c = 0;
    for (auto i : init) data[c++] = i;
  }
  MultiIndex(const I *const *begin, const I *const *end) {
    std::copy(begin, end, &data[0]);
  }

  inline const I &operator[](int i) const { return *data[i]; }
  template<int i> inline const I &get() const { return *data[i]; }

  inline const I *getPtr(int i) const { return data[i]; }
  inline const I *&getPtr(int i) { return data[i]; }

  /* copy constructor: */
  MultiIndex(const MultiIndex<N, I> &mi) {
    for (int i = 0; i < N; i++) data[i] = mi.getPtr(i);
  }

  /* Construct a NEW 'MultiIndex' from the FIRST 'M' elements in 'data': */
  template<int M>
  inline MultiIndex<M, I> front() const {
    return (M == 0) ? MultiIndex<M, I>()
    : MultiIndex<M, I>(&data[0], &data[M]);
  }

  /* Construct a NEW 'MultiIndex' from the LAST 'M' elements in 'data': */
  template<int M>
  inline MultiIndex<M, I> back() const {
    return (M == 0) ? MultiIndex<M, I>()
    : MultiIndex<M, I>(&data[N-M], &data[N]);
  }

  /* Construct a NEW 'MultiIndex' by concatenating 'data' from
   * this 'MultiIndex' with another 'MultiIndex' (i.e. 'rhs'):
   */
  template<int M>
  inline MultiIndex<N+M, I> operator+(const MultiIndex<M, I> &rhs) const {
    MultiIndex<M+N, I> result(&data[0], &data[N]);
    for (int i = 0; i < M; i++) result.getPtr(N+i) = rhs.getPtr(i);
    return result;
  }

  inline void print() const {
    std::cout << "(";
    for (int i = 0; i < N; i++) {
      std::cout << (*this)[i];
      if (i != (N-1)) std::cout << ",";
    }
  }
};


/* 'MultiIndexFlattener':
 * turns a 'MultiIndex' into a flat index to access elements of a tensor
 * of type 'TType'.
 */
template<int N, typename I, typename TType>
struct MultiIndexFlattener
: MultiIndexFlattener<N-1, I, typename TType::TTailType> {
private:
  const MultiIndex<N, I> mi__;

  using Base = MultiIndexFlattener<N-1, I, typename TType::TTailType>;

  const Base *base() const { return static_cast<const Base*>(this); }

public:
  MultiIndexFlattener(const MultiIndex<N, I> &mi)
  : Base(mi.template back<N-1>())
  , mi__(mi) {
    static_assert(N == TType::rank, "");
  }

  inline I operator()(void) const {
    return mi__.template get<0>() * TType::TTailType::num + (*base())();
  };
};

template<typename I, typename TType>
struct MultiIndexFlattener<1, I, TType> {
private:
  const MultiIndex<1, I> mi__;

public:
  MultiIndexFlattener(const MultiIndex<1, I> &mi) : mi__(mi) {}

  inline I operator()(void) const {
    return mi__.template get<0>();
  };
};
/* ---------------------------------------------------------------------------*
 *
 * ---------------------------------------------------------------------------*/
/* 'TTensor' is the fundamental object that represents a tensor. The elements
 * of the tensor are stored inside a 'TTensor' object in the member 'data',
 * which is a flat array (with elements of type 'ET').
 */
template<int N, typename ET, int d, int... ds>
class TTensor {
public:
  using type = typename TTypeBuilder<N, ET, d, ds...>::type;

  static constexpr int rank() { return type::rank; }
  static constexpr int dim(int i) { return type::size(i); }
  static constexpr int elements() { return type::num; }

private:
  ET data[elements()];

public:
  TTensor() {}

  /* Elements of this tensor are accessed using a 'MultiIndex': */
  ET &operator[](const MultiIndex<N, int> &mi);
  const ET &operator[](const MultiIndex<N, int> &mi) const;
};

template<int N, typename ET, int d, int... ds>
ET &
TTensor<N, ET, d, ds...>::operator[](const MultiIndex<N, int> &mi) {
  MultiIndexFlattener<N, int, type> MIF(mi);
  return data[MIF()];
}

template<int N, typename ET, int d, int... ds>
const ET &
TTensor<N, ET, d, ds...>::operator[](const MultiIndex<N, int> &mi) const {
  MultiIndexFlattener<N, int, type> MIF(mi);
  return data[MIF()];
}
/* ---------------------------------------------------------------------------*
 *
 * ---------------------------------------------------------------------------*/
/* Types prefixed with 'TExpr...' represent expressions involving tensors.
 * Currently, the following expressions are supported:
 *  (1) 'TExptTensor' which is intended to wrap a 'TTensor' object,
 *  (2) 'TExprContraction' which represents the contraction of two 'TExpr...'
 *      objects along a (single) pair of dimensions ('d0' and 'd1').
 *
 * Each of the 'TExpr...' classes defines a method 'contract' that returns
 * an object of type 'TExprContraction'. The intended meaning of this is that
 * 'contract' produces a contraction expression whose arguments are the
 * expressions'this' and 'rhs' (supplied as a function argument to 'contract').
 */
template<typename T0, int d0, typename T1, int d1>
class TExprContraction {
private:
  using T0_Type = typename T0::type;
  using T1_Type = typename T1::type;

  using T0_Type_ET = typename T0_Type::ElementType;
  using T1_Type_ET = typename T1_Type::ElementType;
  using ET = T0_Type_ET;

public:
  using type = typename
    TContractionTypeBuilder<T0_Type, d0, T1_Type, d1>::type;

  static constexpr int rank() { return type::rank; }
  static constexpr int dim(int i) { return type::size(i); }
  static constexpr int elements() { return type::num; }

private:
  const T0 t0__;
  const T1 t1__;

public:
  TExprContraction(const T0 &t0, const T1 &t1) : t0__(t0), t1__(t1) {
    static_assert(std::is_same<T0_Type_ET, T1_Type_ET>::value, "");
    static_assert(T0::dim(d0) == T1::dim(d1), "");
  }

  ET operator[](const MultiIndex<type::rank, int> &mi) const;

  template<int d0_, int d1_, typename T1_>
  TExprContraction<type, d0_, T1_, d1_> contract(const T1_ &rhs) const {
    return TExprContraction<type, d0_, T1_, d1_>(*this, rhs);
  }
};


/* 'TExprTensor':
 * represents an expression that conists only of a tensor object, i.e.
 * an object is intended to simply wrap a 'TTensor'.
 */
template<typename T>
class TExprTensor {
public:
  using type = typename T::type;

  static constexpr int rank() { return type::rank; }
  static constexpr int dim(int i) { return type::size(i); }
  static constexpr int elements() { return type::num; }

private:
  using ET = typename type::ElementType;

  T &t__;

public:
  TExprTensor(T &t) : t__(t) {}

  ET &operator[](const MultiIndex<rank(), int> &mi) { return t__[mi]; }
  const ET &operator[](const MultiIndex<rank(), int> &mi) const {
    return t__[mi];
  }

  template<int d0, int d1, typename T1>
  TExprContraction<TExprTensor, d0, T1, d1> contract(const T1 &rhs) {
    return TExprContraction<TExprTensor, d0, T1, d1>(*this, rhs);
  }
};


/* Accessing elements of a contraction expression is not trivial: */
template<typename T0, int d0, typename T1, int d1>
typename T0::type::ElementType
TExprContraction<T0, d0, T1, d1>::
operator[](const MultiIndex<type::rank, int> &mi) const {
  static_assert(type::rank == T0::rank() + T1::rank() - 2, "");
  const auto fst = mi.template front<(T0::rank()-1)>();
  const auto snd = mi.template back<(T1::rank()-1)>();

  constexpr int const past_d0 = T0::rank() - 1 - d0;
  constexpr int const past_d1 = T1::rank() - 1 - d1;

  /* The index 'i' will be used for the dimensions that are contracted ... */
  int i;
  /* ... and it is packed up into a 'MultiIndex': */
  MultiIndex<1, int> index({&i});

  /* The appropriate 'MultiIndices' for the first and second argument of the
   * contraction (i.e. for 't0__' and 't1__') can now be built up easily
   * using operation defined by the 'MultiIndex' class:
   */
  const auto blowUpFst =
    fst.template front<d0>() + index + fst.template back<past_d0>();
  const auto blowUpSnd =
    snd.template front<d1>() + index + snd.template back<past_d1>();

  /* Note that this is (and should) be checked already by
   * the constructor 'TExprContraction':
   */
  static_assert(T0::dim(d0) == T1::dim(d1), "");

  ET result = 0;
  for (i = 0; i < T0::dim(d0); i++) {
    result += t0__[blowUpFst] * t1__[blowUpSnd];
  }
  return result;
}
/* ---------------------------------------------------------------------------*/

int main() {
  TTensor<2, double, 200, 200> M;
  TTensor<1, double, 200> v;

  auto Mv = TExprTensor<decltype(M)>(M)
              .contract<1, 0>(TExprTensor<decltype(v)>(v));

  int i, j;
  MultiIndex<1, int> mi({&i});
  MultiIndex<2, int> mij({&i, &j});

  for(i = 0; i < M.dim(0); i++) {
    for(j = 0; j < M.dim(1); j++) {
      M[mij] = (i == j) ? 2.0 : 0.0;
    }
  }

  for(i = 0; i < v.dim(0); i++) {
    v[mi] = i;
  }
  for(i = 0; i < v.dim(0); i++) {
    std::cout << v[mi] << ", ";
  }

  for (int t = 0; t < 1; t++) {
    for(i = 0; i < Mv.dim(0); i++) {
      v[mi] = Mv[mi];
    }
  }

  std::cout << "\n";
  for(i = 0; i < v.dim(0); i++) {
    std::cout << v[mi] << ", ";
  }
  std::cout << "\n";

  return 0;
}


