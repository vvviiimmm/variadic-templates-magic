#ifndef __BASIC_H__
#define __BASIC_H__

#include <type_traits>

// Basic element
template <int E> struct Element : std::integral_constant<int, E> {};

// List declaration
template <int... Is> struct List {};

// Head
// Example:
//    Head<List<1,2,3>> === Element<1>
// ===========================================================================
template <class T> struct Head_;

template <int... Is> struct Head_<List<Is...>> {
  // template <int H, int... Tl> using HeadImpl = Element<H>;

  template <int H, int... Tl> struct HeadImpl { using type = Element<H>; };

  using type = typename HeadImpl<Is...>::type;
};

template <> struct Head_<List<>> {};

template <class T> using Head = typename Head_<T>::type;
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

template <int... Is>
struct IsEmpty<List<Is...>> : std::integral_constant<bool, false> {};

template <> struct IsEmpty<List<>> : std::integral_constant<bool, true> {};
// ===========================================================================

// Makes a List from 0 up to N-1
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
//    Concat<List<1,2,3>, List<4,5,6>> === List<1, 2, 3, 4, 5, 6 >
// ===========================================================================
template <class T, class U> struct Concat_;

template <int... Is, int... Js> struct Concat_<List<Is...>, List<Js...>> {
  using type = List<Is..., Js...>;
};

template <class T, class U> using Concat = typename Concat_<T, U>::type;
// ===========================================================================

// Take first N values from a List producing a new List
// Example:
//    Take<2, List<1,2,3>> === List<1, 2>
// ===========================================================================
template <int N, class T> struct Take_;

template <int N, int... Is> struct Take_<N, List<Is...>> {
  template <int M, class L> struct TakeImpl;

  template <int M, int... Zs> struct TakeImpl<M, List<Zs...>> {
    using tail = typename TakeImpl<M - 1, Tail<List<Zs...>>>::type;
    using type = Concat<List<Head<List<Zs...>>::value>, tail>;
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

// Replicate list
// Makes a List by replicating input list N times
// Example:
//    ReplicateList<2, List<1,2,3>> === List<1,2,3,1,2,3>
// ===========================================================================
template <int N, class T> struct ReplicateList_;

template <int N, int... Is> struct ReplicateList_<N, List<Is...>> {
  using tail = typename ReplicateList_<N - 1, List<Is...>>::type;
  using type = typename Concat<List<Is...>, tail>;
};

template <int... Is> struct ReplicateList_<1, List<Is...>> {
  using type = List<Is...>;
};

template <int N, class T>
using ReplicateList = typename ReplicateList_<N, T>::type;
// ===========================================================================

// Zips two Lists
// Example:
//    Zip<List<1,2,3>, List<4,5,6>> === List<1, 4, 2, 5, 3, 6 >
// ===========================================================================
template <class T, class U> struct Zip_;

template <int... Is, int... Js> struct Zip_<List<Is...>, List<Js...>> {
  using List1 = List<Head<List<Is...>>::value, Head<List<Js...>>::value>;
  using List2 = typename Zip_<Tail<List<Is...>>, Tail<List<Js...>>>::type;
  using type = Concat<List1, List2>;
};

template <> struct Zip_<List<>, List<>> { using type = List<>; };

template <class T, class U> using Zip = typename Zip_<T, U>::type;

// 1-ary functions on Element
// Example:
//    Id< Element <2> >::value === 2
//    Negate< Element <2> >::value === -2
//    Abs< Element <-2> >::value === 2
// ===========================================================================
template <class T> struct Id { using type = T; };

template <class T> struct Negate : std::integral_constant<int, -T::value> {};

template <class T>
struct Abs
    : std::conditional<(T::value > 0), std::integral_constant<int, T::value>,
                       std::integral_constant<int, -T::value>>::type {};

// Composition of 1-ary functions on Element
// Example:
//    Compose<Negate, Abs>::Result< Element <-2> >::value === -2
// ===========================================================================
template <template <class> class F, template <class> class G> struct Compose {
  template <class X>
  struct Result : std::integral_constant<int, F<Element<G<X>::value>>::value> {
  };
};

// 2-ary functions on Element
// Example:
//    Add< Element <2>, Element<3> >::value === 5
//    Substract< Element <2>, Element<3> >::value === -1
// ===========================================================================
template <class T, class V>
struct Add : std::integral_constant<int, T::value + V::value> {};

template <class T, class V>
struct Substract : std::integral_constant<int, T::value - V::value> {};

// List folding operation (left)
// Takes List, Initial value and Function and produces a single value
// Example:
//    Foldl< List<1,2,3>, 0, Add>::value === 6
//    Foldl< List<1,2,3>, 0, Substract>::value === -6
// ===========================================================================
template <class L, int Init, template <class, class> class F> struct Foldl;

template <int... Is, int Init, template <class, class> class F>
struct Foldl<List<Is...>, Init, F> {
  static const int value = F<
      Element<0>,
      Element<Foldl<Tail<List<Is...>>,
                    Element<F<Element<Init>, Head<List<Is...>>>::value>::value,
                    F>::value>>::value;
};

template <int Init, template <class, class> class F>
struct Foldl<List<>, Init, F> : std::integral_constant<int, Init> {};
// ===========================================================================

// Map, apply function to each element of a List producing new List
// Example:
//    Map< Abs, List<1, -2, 3> > === List <1, 2, 3>
//    Map< Negate, List<-1, 2, -3> > === List <1, -2, 3>
// ===========================================================================
template <template <class> class F, class T> struct Map;

template <template <class> class F, int... Is> struct Map<F, List<Is...>> {
  using tail = typename Map<F, Tail<List<Is...>>>::type;
  using type = Concat<List<F<Head<List<Is...>>>::value>, tail>;
};

template <template <class> class F> struct Map<F, List<>> {
  using type = List<>;
};
// ===========================================================================

#endif __BASIC_H__