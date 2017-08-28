
#include <cassert>
#include <iostream>
#include <type_traits>
#include <string>


// 'TType' represents the type of a tensor, which is
// essentially a list of dimensions (i.e. integers).
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
    assert(i == 0);
    return dim;
  }
};

// 'TTypeBuilder' is a helper struct for building tensor types
// (i.e. 'TType' structs) from variadic lists.
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

// 'TDropper' constructs a 'TType' by dropping the first 'N' dimensions of 'T':
template<typename T, int N>
struct TDropper {
  using type = typename TDropper<typename T::TTailType, N-1>::type;
};

template<typename T>
struct TDropper<T, 0> {
  using type = T;
};

// 'TTaker' constructs a 'TType' from first 'N' dimensions of 'T':
template<typename T, int N>
struct TTaker {
private:
  using tail = typename TTaker<typename T::TTailType, N-1>::type;

public:
  using type = TType<N, typename T::ElementType, T::dim, tail>;
};

template<typename T>
struct TTaker<T, 0> {
  using type = typename T::ElementType;
};

// 'TConcater' constructs a 'TType' by merging the dimensions of 'T0' and 'T1':
template<typename T0, typename T1>
struct TConcater {
private:
  using tail = typename TConcater<typename T0::TTailType, T1>::type;
  using ET = typename T0::ElementType;

  static constexpr int rank = T0::rank + T1::rank;

public:
  using type = TType<rank, ET, T0::dim, tail>;

  TConcater() {
    static_assert(std::is_same<ET, typename T1::ElementType>::value, "");
  }
};

// Specialization of 'TConcater' for appending an empty list.
template<typename T>
struct TConcater<T, typename T::ElementType> {
  using type = T;

  TConcater() {
    static_assert(std::is_same<T, typename T::ElementType>::value, "");
  }
};

// Specialization of 'TConcater' for prepending an empty list.
template<typename T>
struct TConcater<typename T::ElementType, T> {
  using type = T;

  TConcater() {
    static_assert(std::is_same<T, typename T::ElementType>::value, "");
  }
};

// 'TDimPrinter' converts dimension 'i' of tensor type 'T' into a string.
template<typename T, int i>
struct TDimPrinter {
  static std::string str;

  TDimPrinter() {
    static_assert(i >= 0, "");
  }
};

template<typename T, int i>
std::string TDimPrinter<T, i>::str = std::to_string(T::size(i));

// 'TPrinter' turns the list of dimensions of 'T' into a string.
template<typename T>
struct TPrinter {
  static std::string s;
  static std::string str;
};

template<typename T>
std::string TPrinter<T>::s =
  (T::rank == 0)
  ? ""
  : ( (T::rank == 1)
      ? TDimPrinter<T, 0>::str
      : (TDimPrinter<T, 0>::str + "," + TPrinter<typename T::TTailType>::s)
    );

template<>
std::string TPrinter<double>::s = "";

template<typename T>
std::string TPrinter<T>::str = "[" + s + "]";


template<typename T, int i0, int i1>
struct TTransposedTypeBuilder {
private:
  static constexpr int i0_ = (i0 < i1) ? i0 : i1;
  static constexpr int i1_ = (i0 < i1) ? i1 : i0;

  static constexpr int dim0 = T::size(i0_);
  static constexpr int dim1 = T::size(i1_);

  using ET = typename T::ElementType;
  using D0 = TType<1, ET, dim0, ET>;
  using D1 = TType<1, ET, dim1, ET>;

  using head   = typename TTaker<T, i0_>::type;
  using tail   = typename TDropper<T, (i0_+1)>::type;
  using middle = typename TTaker<tail, (i1_-i0_-1)>::type;
  using end    = typename TDropper<tail, (i1_-i0_)>::type;

  using type0 = typename TConcater<D0, end>::type;
  using type1 = typename TConcater<middle, type0>::type;
  using type2 = typename TConcater<D1, type1>::type;

public:
  using type = typename TConcater<head, type2>::type;
};

template<typename T, int d0>
struct TTransposedTypeBuilder<T, d0, d0> {
  using type = T;
};


int main() {
  using ttype = TTypeBuilder<5, double, 1, 2, 3, 4, 5>::type;

  using ttype_t3 = TTaker<ttype, 3>::type;
  std::cout << "Taken 3: " << TPrinter<ttype_t3>::str << "\n";

  using ttype_t0 = TTaker<ttype, 0>::type;
  std::cout << "Taken 0: " << TPrinter<ttype_t0>::str << "\n";

  using ttype_d2 = TDropper<ttype, 2>::type;
  std::cout << "Dropped 2: " << TPrinter<ttype_d2>::str << "\n";

  using ttype_d0 = TDropper<ttype, 0>::type;
  std::cout << "Dropped 0: " << TPrinter<ttype_d0>::str << "\n";

  using ttype_c1 = TConcater<ttype,    ttype_t3>::type;
  using ttype_c2 = TConcater<ttype_t0, ttype_c1>::type;
  using ttype_c3 = TConcater<ttype_d2, ttype_c2>::type;
  using ttype_c4 = TConcater<ttype_c3, ttype_d0>::type;

  // Should print this: "[3,4,5,1,2,3,4,5,1,2,3,1,2,3,4,5]"
  std::cout << TPrinter<ttype_c4>::str << std::endl;

  using ttype_trans0 = TTransposedTypeBuilder<ttype, 2, 3>::type;
  // Should print this: "[1,2,4,3,5]"
  std::cout << "Transposed (2,3): " << TPrinter<ttype_trans0>::str << "\n";

  using ttype_trans1 = TTransposedTypeBuilder<ttype_trans0, 0, 3>::type;
  // Should print this: "[3,2,4,1,5]"
  std::cout << "Transposed (0,3): " << TPrinter<ttype_trans1>::str << "\n";

  using ttype_trans2 = TTransposedTypeBuilder<ttype_trans1, 1, 4>::type;
  // Should print this: "[3,5,4,1,2]"
  std::cout << "Transposed (1,4): " << TPrinter<ttype_trans2>::str << "\n";

  using ttype_trans3 = TTransposedTypeBuilder<ttype_c4,
                                          0,
                                          (ttype_c4::rank-1)>::type;
  // Should print this: "[5,4,5,1,2,3,4,5,1,2,3,1,2,3,4,3]"
  std::cout << "Transposed: " << TPrinter<ttype_trans3>::str << "\n";
}
