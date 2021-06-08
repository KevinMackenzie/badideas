//
// Bad idea credit: Me
//
// Purpose: It can be beneficial to prove that conditions hold at compile time
//
//      This proof-of-concept allows proofs on trivial data types
//        with the assmuptions similar to whats enforced by the borrow checker
//        in rust.  There is a fair amount of syntactic overhead for the user
//        and conditions cannot be applied to member functions...yet
//
#include <cstdio>

#include <concepts>
#include <type_traits>

#include <optional>
#include <vector>

using std::nullopt;
using std::optional;

template <typename T1, typename T2, typename V1, typename V2,
          V1 (*F1)(const T1 &), V2 (*F2)(const T2 &), bool (*FC)(V1 &&, V2 &&)>
struct CheckedBinOp {
private:
  const T1 &_t1;
  const T2 &_t2;

  constexpr CheckedBinOp(const T1 &t1, const T2 &t2) : _t1(t1), _t2(t2) {}

public:
  constexpr const auto &first() const { return _t1; }
  constexpr const auto &second() const { return _t2; }

  constexpr static optional<CheckedBinOp> New(const T1 &t1, const T2 &t2) {
    if (FC(F1(t1), F2(t2)))
      return CheckedBinOp(t1, t2);
    return nullopt;
  }
};

template <typename T> T ident(const T &t) { return t; }
template <typename T> bool lt(T &&i, T &&j) { return i < j; }

template <typename T>
using CheckedLt = CheckedBinOp<T, T, T, T, ident<T>, ident<T>, lt<T>>;
void do_something(const CheckedLt<int> &a) {
  printf("Less Than success! %d, %d\n", a.first(), a.second());
}

template <typename T> concept Sized = requires(T t) {
  { t.size() }
  ->std::same_as<size_t>;
};
template <Sized T> size_t sized_size(const T &s) { return s.size(); }

template <Sized T>
using CheckedSize = CheckedBinOp<size_t, T, size_t, size_t, ident<size_t>,
                                 sized_size<T>, lt<size_t>>;

template <typename T> const T &index(const CheckedSize<std::vector<T>> &a) {
  return a.second()[a.first()];
}

int main(int argc, char **argv) {
  if (argc < 4) {
    printf("Not enough args\n");
    return 1;
  }
  int a = atoi(argv[1]);
  int b = atoi(argv[2]);

  if (auto cc = CheckedLt<int>::New(a, b)) {
    do_something(*cc);
  }

  auto v = std::vector<int>{1, 2, 3, 4};
  size_t i = atoi(argv[3]);
  if (auto cs = CheckedSize<std::vector<int>>::New(i, v)) {
    printf("Index successs! %d\n", index(*cs));
  }

  return 0;
}