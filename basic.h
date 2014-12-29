#ifndef __BASIC_H__
#define __BASIC_H__

#include <type_traits>

// Basic element
template <int E> struct Element : std::integral_constant<int, E> {};

// Empty type declaration
struct Empty {};

// List declaration
template <int... Is> struct List {};

// Head
// Example:
//		Head<List<1,2,3>> === Element<1>
// ===========================================================================
template <class T> struct Head_;

template <int... Is> struct Head_<List<Is...>> {
  template <int H, int... Tl> using HeadImpl = Element<H>;

  using type = HeadImpl<Is...>;
};

template <> struct Head_<List<>> {};

template <class T> using Head = typename Head_<T>::type;
// ===========================================================================

// Tail
// Example:
//		Tail<List<1,2,3>> === List<2,3>
// ===========================================================================
template <class T> struct Tail_;

template <int... Is> struct Tail_<List<Is...>> {
  template <int H, int... Tl> struct TailImpl { using type = List<Tl...>; };

  using type = typename TailImpl<Is...>::type;
};

template <> struct Tail_<List<>> {};

template <class T> using Tail = typename Tail_<T>::type;
// ===========================================================================

// Replicate list
// Makes a List by replicating input list N times
// Example:
// 		ReplicateList<2, List<1,2,3>> === List<1,2,3,1,2,3>
// ===========================================================================
template <int N, class T> struct ReplicateList;

template <int N, int... Is> struct ReplicateList<N, List<Is...>> {
  template <int Num, int... Ms>
  struct ReplicateImpl : ReplicateImpl<Num - 1, Is..., Ms...> {};

  template <int... Bs> struct ReplicateImpl<1, Bs...> {
    using type = List<Bs...>;
  };

  template <int... Bs> struct ReplicateImpl<0, Bs...> { using type = Empty; };

  using type = typename ReplicateImpl<N, Is...>::type;
};
// ===========================================================================

// Makes a List from 0 up to N-1
// Example:
//		MakeSeq<6> === List<0,1,2,3,4,5>
// ===========================================================================
template <int N, int... Is>
struct MakeSeqImpl : MakeSeqImpl<N - 1, N - 1, Is...> {};

template <int... Is> struct MakeSeqImpl<0, Is...> { using type = List<Is...>; };

template <int N, int... Is>
using MakeSeq = typename MakeSeqImpl<N, Is...>::type;
// ===========================================================================

// Makes a list by replicating single value "Num" "N" times
// Example:
//		ReplicateValue<3, 42> === List<42, 42, 42>
// ===========================================================================
template <int N, int Num, int... Is>
struct ReplicateValueImpl : ReplicateValueImpl<N - 1, Num, Num, Is...> {};

template <int... Is> struct ReplicateValueImpl<1, Is...> {
  using type = List<Is...>;
};

template <int N, int Num, int... Is>
using ReplicateValue = typename ReplicateValueImpl<N, Num, Is...>::type;
// ===========================================================================

// Concatanate two Lists
// Example:
//		Concat<List<1,2,3>, List<4,5,6>> === List<1, 2, 3, 4, 5, 6 >
// ===========================================================================
template <class T, class U> struct Concat_;

template <int... Is, int... Js> struct Concat_<List<Is...>, List<Js...>> {
  using type = List<Is..., Js...>;
};

template <class T, class U> using Concat = typename Concat_<T, U>::type;
// ===========================================================================

// Zips two Lists
// Example:
//		Zip<List<1,2,3>, List<4,5,6>> === List<1, 4, 2, 5, 3, 6 >
// ===========================================================================
template <class T, class U> struct Zip_;

template <int... Is, int... Js> struct Zip_<List<Is...>, List<Js...>> {
  using List1 = List<Head<List<Is...>>::value, Head<List<Js...>>::value>;
  using List2 = typename Zip_<Tail<List<Is...>>, Tail<List<Js...>>>::type;
  using type = Concat<List1, List2>;
};

template <> struct Zip_<List<>, List<>> { using type = List<>; };

template <class T, class U> using Zip = typename Zip_<T, U>::type;

#endif __BASIC_H__