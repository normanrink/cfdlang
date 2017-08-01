/* ----------------------------- tensor-3.cpp --------------------------------*/
/* This file is not part of the 'CFDlang' compiler or library. It is merely
 * intended as a (partial and incomplete) study into how template meta-
 * programming can be used to express and set up tensor operations.
 *
 * This is work in progress.                                                  */
/* ---------------------------------------------------------------------------*/

#include <cmath>
#include <iostream>

#include <type_traits>


/* ---------------------------------------------------------------------------*/
/* 'select_type_if' is a helper struct that accesses its argument's 'type'
 * field only if the flag 'b' is true.
 *
 * 'select_type_if' is needed in the 'TContractionTypeBuilder' below to avoid
 * a failing type instantiation if 'b == false'. (The failing instantiation
 * in the 'TContractionTypeBuilder' is, in fact, not needed when 'b == false'.)
 */
template<bool b, typename TTy>
struct select_type_if;

template<typename TTy>
struct select_type_if<true, TTy> {
  using type = typename TTy::type;
};

template<typename TTy>
struct select_type_if<false, TTy> {
  using type = void;
};
/* ---------------------------------------------------------------------------*
 *
 * ---------------------------------------------------------------------------*/
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
  using T0_ET = typename T0::ElementType;
  using T1_ET = typename T1::ElementType;

  using tail = typename TProductTypeBuilder<typename T0::TTailType, T1>::type;

public:
  using type = TType<(T0::rank + T1::rank), T0_ET, T0::dim, tail>;

  TProductTypeBuilder() {
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


/* 'TContractionTypeBuilder': forward declaration of the general template. */
template<typename T0, int d0, typename T1, int d1>
struct TContractionTypeBuilder;

/* 'TContractionTypeBuilder': contraction of two 1-dimensional tensors. */
template<typename ET, int d>
struct TContractionTypeBuilder
<TType<1, ET, d, ET>, 0, TType<1, ET, d, ET>, 0> {
public:
  using type = ET;
};

/* 'TContractionTypeBuilder':
 * contraction with a 1-dim. tensor on the RHS, base case ('d0 == 0').
 */
template<typename T0, typename ET, int d>
struct TContractionTypeBuilder
<T0, 0, TType<1, ET, d, ET>, 0> {
public:
  using type = typename T0::TTailType;

  TContractionTypeBuilder() {
    static_assert(std::is_same<typename T0::ElementType, ET>::value, "");
    static_assert(T0::dim == d, "");
  }
};

/* 'TContractionTypeBuilder':
 * contraction with a 1-dim. tensor on the RHS, general case ('d0 > 0').
 */
template<typename T0, int d0, typename ET, int d>
struct TContractionTypeBuilder
<T0, d0, TType<1, ET, d, ET>, 0> {
private:
  using T0_Head = TType<1, ET, T0::dim, ET>;
  using T0_Tail = typename T0::TTailType;

  using tail = typename
    TContractionTypeBuilder<T0_Tail, (d0-1), TType<1, ET, d, ET>, 0>::type;

  static constexpr bool tailIsET = std::is_same<tail, ET>::value;

public:
  using type = typename std::conditional<
    tailIsET,
    T0_Head,
    typename select_type_if<!tailIsET,
                            TProductTypeBuilder<T0_Head, tail>>::type
  >::type;

  TContractionTypeBuilder() {
    /* The special case 'd0 == 0' has been handled above: */
    static_assert(d0 > 0, "");
    static_assert(std::is_same<typename T0::ElementType, ET>::value, "");
  }
};

/* 'TContractionTypeBuilder':
 * contraction with a 1-dim. tensor on the LHS, base case ('d1 == 0').
 */
template<typename ET, int d, typename T1>
struct TContractionTypeBuilder
<TType<1, ET, d, ET>, 0, T1, 0> {
public:
  using type = typename T1::TTailType;

  TContractionTypeBuilder() {
    static_assert(std::is_same<ET, typename T1::ElementType>::value, "");
    static_assert(T1::dim == d, "");
  }
};

/* 'TContractionTypeBuilder':
 * contraction with a 1-dim. tensor on the LHS, general case ('d1 > 0').
 */
template<typename ET, int d, typename T1, int d1>
struct TContractionTypeBuilder
<TType<1, ET, d, ET>, 0, T1, d1> {
private:
  using T1_Head = TType<1, ET, T1::dim, ET>;
  using T1_Tail = typename T1::TTailType;

  using tail = typename
    TContractionTypeBuilder<TType<1, ET, d, ET>, 0, T1_Tail, (d1-1)>::type;

  static constexpr bool tailIsET = std::is_same<tail, ET>::value;

public:
  using type = typename std::conditional<
    tailIsET,
    T1_Head,
    typename select_type_if<!tailIsET,
                            TProductTypeBuilder<T1_Head, tail>>::type
  >::type;

  TContractionTypeBuilder() {
    /* The special case 'd1 == 0' has been handled above: */
    static_assert(d1 > 0, "");
    static_assert(std::is_same<ET, typename T1::ElementType>::value, "");
  }
};

/* 'TContractionTypeBuilder':
 * general tensor types as arguments, contraction of the 0-th indices.
 */
template<typename T0, typename T1>
struct TContractionTypeBuilder
<T0, 0, T1, 0> {
private:
  using T0_Tail = typename T0::TTailType;
  using T1_Tail = typename T1::TTailType;

public:
  using type = typename TProductTypeBuilder<T0_Tail, T1_Tail>::type;

  TContractionTypeBuilder() {
    /* The special cases of 1-dim. tensors (i.e. vectors)
     * have already been handled:
     */
    static_assert(T0::rank > 1 && T1::rank > 1, "");
    /* This asertion should be redundant: */
    static_assert(T0_Tail::rank > 0 && T1_Tail::rank > 0, "");

    static_assert(std::is_same<typename T0::ElementType,
                               typename T1::ElementType>::value, "");

    static_assert(T0::dim == T1::dim, "");
  }
};

/* 'TContractionTypeBuilder':
 * general tensor types as arguments, general 'd0' (i.e. 'd0 > 0').
 */
template<typename T0, int d0, typename T1>
struct TContractionTypeBuilder
<T0, d0, T1, 0> {
private:
  using ET = typename T0::ElementType;
  using T0_Head = TType<1, ET, T0::dim, ET>;
  using T0_Tail = typename T0::TTailType;

  using tail = typename TContractionTypeBuilder<T0_Tail, (d0-1), T1, 0>::type;

public:
  using type = typename TProductTypeBuilder<T0_Head, tail>::type;

  TContractionTypeBuilder() {
    /* The special case 'd0 == 0' has been handled above: */
    static_assert(d0 > 0, "");

    /* The special cases of 1-dim. tensors (i.e. vectors)
     * have already been handled:
     */
    static_assert(T0::rank > 1 && T1::rank > 1, "");
    /* This asertion should be redundant: */
    static_assert(T0_Tail::rank > 0 && T1::TTailType::rank > 0, "");

    static_assert(std::is_same<ET, typename T1::ElementType>::value, "");
  }
};

/* 'TContractionTypeBuilder':
 * general tensor types as arguments, general 'd1' (i.e. 'd1 > 0').
 */
template<typename T0, typename T1, int d1>
struct TContractionTypeBuilder
<T0, 0, T1, d1> {
private:
  using ET = typename T0::ElementType;
  using T0_Head = TType<1, ET, T0::dim, ET>;
  using T0_Tail = typename T0::TTailType;
  using T1_Head = TType<1, ET, T1::dim, ET>;
  using T1_Tail = typename T1::TTailType;

  using tail1 = typename
    TContractionTypeBuilder<T0_Head, 0, T1_Tail, (d1-1)>::type;

  using tail2 = typename
  TProductTypeBuilder<T1_Head, tail1>::type;

public:
  using type = typename TProductTypeBuilder<T0_Tail, tail2>::type;

  TContractionTypeBuilder() {
    /* The special case 'd0 == 0' has been handled above: */
    static_assert(d1 > 0, "");

    /* The special cases of 1-dim. tensors (i.e. vectors)
     * have already been handled:
     */
    static_assert(T0::rank > 1 && T1::rank > 1, "");
    /* This asertion should be redundant: */
    static_assert(T0_Tail::rank > 0 && T1_Tail::rank > 0, "");

    static_assert(std::is_same<ET, typename T1::ElementType>::value, "");
  }
};

/* 'TContractionTypeBuilder':
 * general tensor types as arguments, general 'd0' and 'd1' (i.e. both > 0).
 */
template<typename T0, int d0, typename T1, int d1>
struct TContractionTypeBuilder {
private:
  using ET = typename T0::ElementType;
  using T0_Head = TType<1, ET, T1::dim, ET>;
  using T0_Tail = typename T0::TTailType;

  using tail = typename
    TContractionTypeBuilder<T0_Tail, (d0-1), T1, d1>::type;

  static constexpr bool tailIsET = std::is_same<tail, ET>::value;

public:
  using type = typename std::conditional<tailIsET,
    T0_Head,
    typename select_type_if<!tailIsET,
                            TProductTypeBuilder<T0_Head, tail>>::type
  >::type;

  TContractionTypeBuilder() {
    /* The special cases 'd0 == 0' and 'd1 == 0' have already been handled: */
    static_assert(d0 > 0 && d1 > 0, "");

    /* The special cases of 1-dim. tensors (i.e. vectors)
     * have already been handled:
     */
    static_assert(T0::rank > 1 && T1::rank > 1, "");
    /* This asertion should be redundant: */
    static_assert(T0_Tail::rank > 0 && T1::TTailType::rank > 0, "");

    static_assert(std::is_same<ET, typename T1::ElementType>::value, "");
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
 *  (1) 'TExprTensor' which is intended to wrap a 'TTensor' object,
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
  TExprContraction<TExprContraction, d0_, T1_, d1_>
  contract(const T1_ &rhs) const {
    return TExprContraction<TExprContraction, d0_, T1_, d1_>(*this, rhs);
  }
};


/* 'TExprTensor':
 * represents an expression that conists only of a tensor object,
 * i.e. a 'TExprTensor' object is intended to simply wrap a 'TTensor'.
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
  const int M = 20;
  const int N = 20;

  /* The maximum allowed relative error has been adjusted so that at least
   * a few lines are pronted out for each contraction expression below: */
  const double max_rel_error = 7.0e-15;
  auto rel_error = [](double a, double b) -> double {
    return std::fabs(a-b)/std::fabs(a);
  };

#if 0
  {
    TTensor<2, double, M, N> M;
    TTensor<1, double, N> v;

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
  }
#endif

  {
    TTensor<2, double, N, M> A;
    TTensor<3, double, M, M, M> u;

    /* Initialization: */
    {
      int i, j, k;
      MultiIndex<2, int> m_ij({&i, &j});
      MultiIndex<3, int> m_ijk({&i, &j, &k});

      for (i = 0; i < u.dim(0); i++) {
        for (j = 0; j < u.dim(1); j++) {
          for (k = 0; k < u.dim(2); k++) {
            u[m_ijk] = (double)rand() / (double)RAND_MAX;
          }
        }
      }
      for (i = 0; i < A.dim(0); i++) {
        for (j = 0; j < A.dim(1); j++) {
            A[m_ij] = (double)rand() / (double)RAND_MAX;
        }
      }
    }

    auto TA = TExprTensor<decltype(A)>(A);
    auto Tu = TExprTensor<decltype(u)>(u);

    int i0, i1, j0, j1, k0, k1;
    MultiIndex<3, int> m_i1j1k1({&i1, &j1, &k1}),
                       m_i0j0k0({&i0, &j0, &k0}),
                       m_k0j0i0({&k0, &j0, &i0});
    MultiIndex<2, int> m_i1i0({&i1, &i0}),
                       m_j1j0({&j1, &j0}),
                       m_k1k0({&k1, &k0});


    /* contraction expression no. 1: */
    {
      auto Tv = TA.contract<1, 2>(TA.contract<1, 2>(TA.contract<1, 2>(Tu)));

      std::cout << "\ncontraction expression no. 1:\n";

      for (i1 = 0; i1 < Tv.dim(0); i1++) {
        for (j1 = 0; j1 < Tv.dim(1); j1++) {
          for (k1 = 0; k1 < Tv.dim(2); k1++) {
            double t = 0.0;
            for (i0 = 0; i0 < Tu.dim(0); i0++) {
              for (j0 = 0; j0 < Tu.dim(1); j0++) {
                for (k0 = 0; k0 < Tu.dim(2); k0++) {
                  t += TA[m_i1i0] * TA[m_j1j0] * TA[m_k1k0] * Tu[m_i0j0k0];
                }
              }
            }
            const auto v = Tv[m_i1j1k1];

            if (rel_error(t, v) > max_rel_error) {
              std::cout << "no. 1: ";
              std::cout << "(" << i1 << "," << j1 << "," << k1 << "): ";
              std::cout << t << " -- " << v;
              std::cout << " rel. error: " << rel_error(t, v) << "\n";
            }
          }
        }
      }

      std::cout << "-----------------------------\n\n";
    }
    /* contraction expression no. 2: */
    {
      auto Tv = TA.contract<1, 2>(TA.contract<1, 1>(TA.contract<1, 0>(Tu)));

      std::cout << "contraction expression no. 2:\n";

      for (i1 = 0; i1 < Tv.dim(0); i1++) {
        for (j1 = 0; j1 < Tv.dim(1); j1++) {
          for (k1 = 0; k1 < Tv.dim(2); k1++) {
            double t = 0.0;
            for (i0 = 0; i0 < Tu.dim(0); i0++) {
              for (j0 = 0; j0 < Tu.dim(1); j0++) {
                for (k0 = 0; k0 < Tu.dim(2); k0++) {
                  t += TA[m_i1i0] * TA[m_j1j0] * TA[m_k1k0] * Tu[m_k0j0i0];
                }
              }
            }
            const auto v = Tv[m_i1j1k1];

            if (rel_error(t, v) > max_rel_error) {
              std::cout << "no. 2: ";
              std::cout << "(" << i1 << "," << j1 << "," << k1 << "): ";
              std::cout << t << " -- " << v;
              std::cout << " rel. error: " << rel_error(t, v) << "\n";
            }
          }
        }
      }
    }

    std::cout << "-----------------------------\n\n";
  }

  return 0;
}


