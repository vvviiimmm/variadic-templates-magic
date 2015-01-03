#ifndef __BASIC_H__
#define __BASIC_H__

#include <type_traits>

// List declaration
template <int... Is> struct List {};

// Head
// Example:
//    Head<List<1,2,3>>::value === 1
// ===========================================================================
template <class T> struct Head;

template <int... Is> struct Head<List<Is...>> {

  template <int H, int... Tl>
  struct HeadImpl : std::integral_constant<int, H> {};

  static const int value = typename HeadImpl<Is...>::value;
};

template <> struct Head<List<>> {};
// ===========================================================================

// Tail
// Example:
//    Tail<List<1,2,3>> === List<2,3>
// ===========================================================================
template <class T> struct Tail_;

template <int... Is> struct Tail_<List<Is...>> {
  template <int H, int... Tl> struct TailImpl { using type = List<Tl...>; };

  using type = typename TailImpl<Is...>::type;
};

template <> struct Tail_<List<>> {};

template <class T> using Tail = typename Tail_<T>::type;
// ===========================================================================

// Length of a List
// Example:
//    Length<List<1,2,3>>::value === 3
// ===========================================================================
template <class T> struct Length;

template <int... Is>
struct Length<List<Is...>> : std::integral_constant<int, sizeof...(Is)> {};
// ===========================================================================

// Test whether List is Empty
// Example:
//    IsEmpty<List<1,2,3>>::value === false
//    IsEmpty<List<>>::value === true
// ===========================================================================
template <class T> struct IsEmpty;

template <int... Is> struct IsEmpty<List<Is...>> : std::false_type {};

template <> struct IsEmpty<List<>> : std::true_type {};
// ===========================================================================

// Makes a List [0, N)
// Example:
//    MakeSeq<6> === List<0,1,2,3,4,5>
// ===========================================================================
template <int N, int... Is>
struct MakeSeqImpl : MakeSeqImpl<N - 1, N - 1, Is...> {};

template <int... Is> struct MakeSeqImpl<0, Is...> { using type = List<Is...>; };

template <int N, int... Is>
using MakeSeq = typename MakeSeqImpl<N, Is...>::type;
// ===========================================================================

// Makes a list by replicating single value "Num" "N" times
// Example:
//    ReplicateValue<3, 42> === List<42, 42, 42>
// ===========================================================================
template <int N, int Num, int... Is>
struct ReplicateValue_ : ReplicateValue_<N - 1, Num, Num, Is...> {};

template <int... Is> struct ReplicateValue_<1, Is...> {
  using type = List<Is...>;
};

template <int N, int Num, int... Is>
using ReplicateValue = typename ReplicateValue_<N, Num, Is...>::type;
// ===========================================================================

// Concatanate two Lists
// Example:
//    Concat<List<1,2,3>, List<4,5,6>> === List<1, 2, 3, 4, 5, 6>
// ===========================================================================
template <class T, class U> struct Concat_;

template <int... Is, int... Js> struct Concat_<List<Is...>, List<Js...>> {
  using type = List<Is..., Js...>;
};

template <class T, class U> using Concat = typename Concat_<T, U>::type;
// ===========================================================================

// Prepend a value to the beginning of the List
// Example:
//    Prepend<3, List<4,5,6>> === List<3, 4, 5, 6>
// ===========================================================================
template <int T, class U> struct Prepend_;

template <int T, int... Js> struct Prepend_<T, List<Js...>> {
  using type = List<T, Js...>;
};

template <int T, class U> using Prepend = typename Prepend_<T, U>::type;
// ===========================================================================

// Append a value to the end of the List
// Example:
//    Append<7, List<4,5,6>> === List<4, 5, 6, 7>
// ===========================================================================
template <int T, class U> struct Append_;

template <int T, int... Js> struct Append_<T, List<Js...>> {
  using type = List<Js..., T>;
};

template <int T, class U> using Append = typename Append_<T, U>::type;
// ===========================================================================

// Take first N values from a List producing a new List
// Example:
//    Take<2, List<1,2,3>> === List<1, 2>
// ===========================================================================
template <int N, class T> struct Take_;

template <int N, int... Is> struct Take_<N, List<Is...>> {
  template <int M, class L> struct TakeImpl;

  template <int M, int... Zs> struct TakeImpl<M, List<Zs...>> {
    using type = Concat<List<Head<List<Zs...>>::value>,
                        typename TakeImpl<M - 1, Tail<List<Zs...>>>::type>;
  };

  template <int... Zs> struct TakeImpl<0, List<Zs...>> { using type = List<>; };

  using type = typename TakeImpl<N, List<Is...>>::type;
};

template <int N, class T> using Take = typename Take_<N, T>::type;
// ===========================================================================

// Drop first N values from a List producing a new List
// Example:
//    Drop<2, List<1,2,3,4>> === List<3, 4>
// ===========================================================================
template <int N, class T> struct Drop_;

template <int N, int... Is>
struct Drop_<N, List<Is...>> : Drop_<N - 1, Tail<List<Is...>>> {};

template <int... Is> struct Drop_<0, List<Is...>> { using type = List<Is...>; };

template <int N, class T> using Drop = typename Drop_<N, T>::type;
// ===========================================================================

// Makes range from ['Start', 'End')
// Example:
//    Range<3, 9> === List<3, 4, 5, 6, 7, 8>
// ===========================================================================
template <int Start, int End> using Range = Drop<Start, MakeSeq<End>>;

// Replicate list
// Makes a List by replicating input list N times
// Example:
//    ReplicateList<2, List<1,2,3>> === List<1,2,3,1,2,3>
// ===========================================================================
template <int N, class T> struct ReplicateList_;

template <int N, int... Is> struct ReplicateList_<N, List<Is...>> {
  using type = typename Concat<
      List<Is...>, typename ReplicateList_<N - 1, List<Is...>>::type>;
};

template <int... Is> struct ReplicateList_<1, List<Is...>> {
  using type = List<Is...>;
};

template <int N, class T>
using ReplicateList = typename ReplicateList_<N, T>::type;
// ===========================================================================

// Zips two Lists
// Example:
//    Zip<List<1,2,3>, List<4,5,6>> === List<1, 4, 2, 5, 3, 6>
// ===========================================================================
template <class T, class U> struct Zip_;

template <int... Is, int... Js> struct Zip_<List<Is...>, List<Js...>> {
  using type =
      Concat<List<Head<List<Is...>>::value, Head<List<Js...>>::value>,
             typename Zip_<Tail<List<Is...>>, Tail<List<Js...>>>::type>;
};

template <> struct Zip_<List<>, List<>> { using type = List<>; };

template <class T, class U> using Zip = typename Zip_<T, U>::type;
// ===========================================================================

// 1-ary functions
// Example:
//    Id<2>::value == 2
//    Negate<2>::value == -2
//    Abs<-2>::value == 2
// ===========================================================================
template <int I> struct Id : std::integral_constant<int, I> {};

template <int I> struct Negate : std::integral_constant<int, -I> {};

template <int I>
struct Abs : std::conditional<(I > 0), std::integral_constant<int, I>,
                              std::integral_constant<int, -I>>::type {};

// Composition of 1-ary functions
// Example:
//    Compose<Negate, Abs>::Result<-2>::value == -2
// ===========================================================================
template <template <int> class F, template <int> class G> struct Compose {
  template <int X>
  struct Result : std::integral_constant<int, F<G<X>::value>::value> {};
};

// 2-ary functions
// Example:
//    Add<2, 3>::value == 5
//    Substract<2, 3>::value == -1
// ===========================================================================
template <int T, int V> struct Add : std::integral_constant<int, T + V> {};

template <int T, int V>
struct Substract : std::integral_constant<int, T - V> {};

// List folding operation (left)
// Takes List, Initial value and Function and produces a single value
// Example:
//    Foldl<Add, 42, List<1,2,3>>::value == 48
//    Foldl<Substract, 0, List<1,2,3>>::value == -6
// ===========================================================================
template <template <int, int> class F, int Init, class L> struct Foldl;

template <template <int, int> class F, int Init, int... Is>
struct Foldl<F, Init, List<Is...>> {
  static const int value =
      F<0, Foldl<F, F<Init, Head<List<Is...>>::value>::value,
                 Tail<List<Is...>>>::value>::value;
};

template <template <int, int> class F, int Init>
struct Foldl<F, Init, List<>> : std::integral_constant<int, Init> {};
// ===========================================================================

// Map, apply function to each element of a List producing new List
// Example:
//    Map<Abs, List<1, -2, 3>> === List <1, 2, 3>
//    Map<Negate, List<-1, 2, -3>> === List <1, -2, 3>
// ===========================================================================
template <template <int> class F, class T> struct Map_;

template <template <int> class F, int... Is> struct Map_<F, List<Is...>> {
  using type = Concat<List<F<Head<List<Is...>>::value>::value>,
                      typename Map_<F, Tail<List<Is...>>>::type>;
};

template <template <int> class F> struct Map_<F, List<>> {
  using type = List<>;
};

template <template <int> class F, class T>
using Map = typename Map_<F, T>::type;
// ===========================================================================

// Even, Odd predicates
// Example:
//    Even<42>::value == true
//    Odd<13>::value == true
// ===========================================================================
template <int N>
struct Even
    : std::conditional<(N % 2 == 0), std::true_type, std::false_type>::type {};

template <int N>
struct Odd
    : std::conditional<(N % 2 != 0), std::true_type, std::false_type>::type {};
// ===========================================================================

// All, returns true if all elements of the list satisfy some predicate F
// Example:
//    All<Even, List<2, 4, 6>> == true
//    All<Odd, List<3, 5, 9>> === true
// ===========================================================================
template <template <int> class F, class T> struct All;

template <template <int> class F, int... Is> struct All<F, List<Is...>> {
  static const bool value =
      F<Head<List<Is...>>::value>::value && All<F, Tail<List<Is...>>>::value;
};

template <template <int> class F> struct All<F, List<>> {
  static const bool value = true;
};

// TakeWhile, takes elements from the list until predicate is not satisfied
// Example:
//    TakeWhile<Even, List<2, 4, 5, 6, 7>> === List<2, 4>
//    TakeWhile<Odd, List<2, 5, 9>> === List<>
// ===========================================================================
template <template <int> class F, class T> struct TakeWhile_;

template <template <int> class F, int... Is> struct TakeWhile_<F, List<Is...>> {
  using type = typename std::conditional<
      F<Head<List<Is...>>::value>::value,
      Prepend<Head<List<Is...>>::value,
              typename TakeWhile_<F, Tail<List<Is...>>>::type>,
      List<>>::type;
};

template <template <int> class F> struct TakeWhile_<F, List<>> {
  using type = List<>;
};

template <template <int> class F, class T>
using TakeWhile = typename TakeWhile_<F, T>::type;
// ===========================================================================

// Filter operation, produce a new list containing elements that satisfy
// some predicate 'F'
// Example:
//    Filter<Even, MakeSeq<10>> === List<0, 2, 4, 6, 8>;
// ===========================================================================
template <template <int> class F, class T> struct Filter_;

template <template <int> class F, int... Is> struct Filter_<F, List<Is...>> {
  using type = Concat<
      typename std::conditional<F<Head<List<Is...>>::value>::value,
                                List<Head<List<Is...>>::value>, List<>>::type,
      typename Filter_<F, Tail<List<Is...>>>::type>;
};

template <template <int> class F> struct Filter_<F, List<>> {
  using type = List<>;
};

template <template <int> class F, class T>
using Filter = typename Filter_<F, T>::type;
// ===========================================================================

// Prime numbers (naive approach), find all primes up to Num-1
// For each tested number N checks whether every number from [2..N-1] is not a
// factor of N
// Example:
//    PrimesNaive<20> === List<2, 3, 5, 7, 11, 13, 17, 19>;
// ===========================================================================
template <int Num> struct PrimesNaive_ {
  template <int Tested> struct IsPrime {
    template <int M>
    struct IsFactor : std::conditional<(Tested % M == 0), std::true_type,
                                       std::false_type>::type {};

    static const bool value = std::conditional<
        (Length<Filter<IsFactor, Range<2, Tested>>>::value == 0),
        std::true_type, std::false_type>::type::value;
  };

  using type = Filter<IsPrime, Range<2, Num>>;
};

template <int Num> using PrimesNaive = typename PrimesNaive_<Num>::type;
// ===========================================================================

// Primes up to N-1
// For each tested number N checks whether it has factors in previously
// calculated primes
// Example:
//    Primes<20> === List<2, 3, 5, 7, 11, 13, 17, 19>;
// ===========================================================================
template <int Num> struct Primes_ {
  template <int R, int Tested, class T> struct IsPrime;

  template <int R, int Tested, int... Is>
  struct IsPrime<R, Tested, List<Is...>> {
    template <int M>
    struct IsFactor : std::conditional<(Tested % M == 0), std::true_type,
                                       std::false_type>::type {};

    using type = typename IsPrime<
        R - 1, Tested + 1,
        typename std::conditional<
            std::conditional<
                (Length<Filter<IsFactor, List<Is...>>>::value == 0),
                std::true_type, std::false_type>::type::value,
            Append<Tested, List<Is...>>, List<Is...>>::type>::type;
  };

  template <int Tested, int... Is> struct IsPrime<0, Tested, List<Is...>> {
    using type = List<Is...>;
  };

  using type = typename IsPrime<Num - 1, 2, List<>>::type;
};

template <int Num> using Primes = typename Primes_<Num>::type;
// ===========================================================================

#endif __BASIC_H__