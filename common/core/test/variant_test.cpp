#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <sstream>
#include "ara/core/variant.h"

using namespace ara::core;

enum Qual { Ptr, ConstPtr, LRef, ConstLRef, RRef, ConstRRef };

struct get_qual_t {
  constexpr Qual operator()(int *) const { return Ptr; }
  constexpr Qual operator()(const int *) const { return ConstPtr; }
  constexpr Qual operator()(int &) const { return LRef; }
  constexpr Qual operator()(const int &) const { return ConstLRef; }
  constexpr Qual operator()(int &&) const { return RRef; }
  constexpr Qual operator()(const int &&) const { return ConstRRef; }
};

constexpr get_qual_t get_qual{};

struct CopyConstruction : std::exception {};
struct CopyAssignment : std::exception {};
struct MoveConstruction : std::exception {};
struct MoveAssignment : std::exception {};

struct copy_thrower_t {
  constexpr copy_thrower_t() {}
  [[noreturn]] copy_thrower_t(const copy_thrower_t &) {
    throw CopyConstruction{};
  }
  copy_thrower_t(copy_thrower_t &&) = default;
  copy_thrower_t &operator=(const copy_thrower_t &) { throw CopyAssignment{}; }
  copy_thrower_t &operator=(copy_thrower_t &&) = default;
};

inline bool operator<(const copy_thrower_t &,
                      const copy_thrower_t &) noexcept {
  return false;
}

inline bool operator>(const copy_thrower_t &,
                      const copy_thrower_t &) noexcept {
  return false;
}

inline bool operator<=(const copy_thrower_t &,
                       const copy_thrower_t &) noexcept {
  return true;
}

inline bool operator>=(const copy_thrower_t &,
                       const copy_thrower_t &) noexcept {
  return true;
}

inline bool operator==(const copy_thrower_t &,
                       const copy_thrower_t &) noexcept {
  return true;
}

inline bool operator!=(const copy_thrower_t &,
                       const copy_thrower_t &) noexcept {
  return false;
}

struct move_thrower_t {
  constexpr move_thrower_t() {}
  move_thrower_t(const move_thrower_t &) = default;
  [[noreturn]] move_thrower_t(move_thrower_t &&) { throw MoveConstruction{}; }
  move_thrower_t &operator=(const move_thrower_t &) = default;
  move_thrower_t &operator=(move_thrower_t &&) { throw MoveAssignment{}; }
};

inline bool operator<(const move_thrower_t &,
                      const move_thrower_t &) noexcept {
  return false;
}

inline bool operator>(const move_thrower_t &,
                      const move_thrower_t &) noexcept {
  return false;
}

inline bool operator<=(const move_thrower_t &,
                       const move_thrower_t &) noexcept {
  return true;
}

inline bool operator>=(const move_thrower_t &,
                       const move_thrower_t &) noexcept {
  return true;
}

inline bool operator==(const move_thrower_t &,
                       const move_thrower_t &) noexcept {
  return true;
}

inline bool operator!=(const move_thrower_t &,
                       const move_thrower_t &) noexcept {
  return false;
}

TEST(Get, HoldsAlternative) {
  ara::core::Variant<int, std::string> v(42);
  EXPECT_TRUE(ara::core::holds_alternative<0>(v));
  EXPECT_FALSE(ara::core::holds_alternative<1>(v));
  EXPECT_TRUE(ara::core::holds_alternative<int>(v));
  EXPECT_FALSE(ara::core::holds_alternative<std::string>(v));

  /* constexpr */ {
    constexpr ara::core::Variant<int, const char *> cv(42);
    static_assert(ara::core::holds_alternative<0>(cv), "");
    static_assert(!ara::core::holds_alternative<1>(cv), "");
    static_assert(ara::core::holds_alternative<int>(cv), "");
    static_assert(!ara::core::holds_alternative<const char *>(cv), "");
  }
}

TEST(Get, MutVarMutType) {
  ara::core::Variant<int> v(42);
  EXPECT_EQ(42, ara::core::get<int>(v));
  // Check qualifier.
  EXPECT_EQ(LRef, get_qual(ara::core::get<int>(v)));
  EXPECT_EQ(RRef, get_qual(ara::core::get<int>(variant_move(v))));
}

TEST(Get, MutVarConstType) {
  ara::core::Variant<const int> v(42);
  EXPECT_EQ(42, ara::core::get<const int>(v));
  // Check qualifier.
  EXPECT_EQ(ConstLRef, get_qual(ara::core::get<const int>(v)));
  EXPECT_EQ(ConstRRef, get_qual(ara::core::get<const int>(variant_move(v))));
}

TEST(Get, ConstVarMutType) {
  const ara::core::Variant<int> v(42);
  EXPECT_EQ(42, ara::core::get<int>(v));
  // Check qualifier.
  EXPECT_EQ(ConstLRef, get_qual(ara::core::get<int>(v)));
  EXPECT_EQ(ConstRRef, get_qual(ara::core::get<int>(variant_move(v))));

  /* constexpr */ {
    constexpr ara::core::Variant<int> cv(42);
    static_assert(42 == ara::core::get<int>(cv), "");
    // Check qualifier.
    static_assert(ConstLRef == get_qual(ara::core::get<int>(cv)), "");
    static_assert(ConstRRef == get_qual(ara::core::get<int>(variant_move(cv))), "");
  }
}

TEST(Get, ConstVarConstType) {
  const ara::core::Variant<const int> v(42);
  EXPECT_EQ(42, ara::core::get<const int>(v));
  // Check qualifier.
  EXPECT_EQ(ConstLRef, get_qual(ara::core::get<const int>(v)));
  EXPECT_EQ(ConstRRef, get_qual(ara::core::get<const int>(variant_move(v))));

  /* constexpr */ {
    constexpr ara::core::Variant<const int> cv(42);
    static_assert(42 == ara::core::get<const int>(cv), "");
    // Check qualifier.
    static_assert(ConstLRef == get_qual(ara::core::get<const int>(cv)), "");
    static_assert(ConstRRef == get_qual(ara::core::get<const int>(variant_move(cv))),
                  "");
  }
}

TEST(Get, ValuelessByException) {
  ara::core::Variant<int, move_thrower_t> v(42);
  EXPECT_THROW(v = move_thrower_t{}, MoveConstruction);
  EXPECT_TRUE(v.valueless_by_exception());
  EXPECT_THROW(ara::core::get<int>(v), ara::core::bad_variant_access);
  EXPECT_THROW(ara::core::get<move_thrower_t>(v), ara::core::bad_variant_access);
}

TEST(GetIf, MutVarMutType) {
  ara::core::Variant<int> v(42);
  EXPECT_EQ(42, *ara::core::get_if<int>(&v));
  // Check qualifier.
  EXPECT_EQ(Ptr, get_qual(ara::core::get_if<int>(&v)));
}

TEST(GetIf, MutVarConstType) {
  ara::core::Variant<const int> v(42);
  EXPECT_EQ(42, *ara::core::get_if<const int>(&v));
  // Check qualifier.
  EXPECT_EQ(ConstPtr, get_qual(ara::core::get_if<const int>(&v)));
}

TEST(GetIf, ConstVarMutType) {
  const ara::core::Variant<int> v(42);
  EXPECT_EQ(42, *ara::core::get_if<int>(&v));
  // Check qualifier.
  EXPECT_EQ(ConstPtr, get_qual(ara::core::get_if<int>(&v)));

  /* constexpr */ {
    static constexpr ara::core::Variant<int> cv(42);
    static_assert(42 == *ara::core::get_if<int>(&cv), "");
    // Check qualifier.
    static_assert(ConstPtr == get_qual(ara::core::get_if<int>(&cv)), "");
  }
}

TEST(GetIf, ConstVarConstType) {
  const ara::core::Variant<const int> v(42);
  EXPECT_EQ(42, *ara::core::get_if<const int>(&v));
  // Check qualifier.
  EXPECT_EQ(ConstPtr, get_qual(ara::core::get_if<const int>(&v)));

  /* constexpr */ {
    static constexpr ara::core::Variant<const int> cv(42);
    static_assert(42 == *ara::core::get_if<const int>(&cv), "");
    // Check qualifier.
    static_assert(ConstPtr == get_qual(ara::core::get_if<const int>(&cv)), "");
  }
}

TEST(GetIf, ValuelessByException) {
  ara::core::Variant<int, move_thrower_t> v(42);
  EXPECT_THROW(v = move_thrower_t{}, MoveConstruction);
  EXPECT_TRUE(v.valueless_by_exception());
  EXPECT_EQ(nullptr, ara::core::get_if<int>(&v));
  EXPECT_EQ(nullptr, ara::core::get_if<move_thrower_t>(&v));
}

/****************************visit*************************************/

TEST(Visit, MutVarMutType) {
  ara::core::Variant<int> v(42);
  // Check `v`.
  EXPECT_EQ(42, ara::core::get<int>(v));
  // Check qualifier.
  EXPECT_EQ(LRef, ara::core::visit(get_qual, v));
  EXPECT_EQ(RRef, ara::core::visit(get_qual, variant_move(v)));
}

TEST(Visit, MutVarConstType) {
  ara::core::Variant<const int> v(42);
  EXPECT_EQ(42, ara::core::get<const int>(v));
  // Check qualifier.
  EXPECT_EQ(ConstLRef, ara::core::visit(get_qual, v));
  EXPECT_EQ(ConstRRef, ara::core::visit(get_qual, variant_move(v)));
}

TEST(Visit, ConstVarMutType) {
  const ara::core::Variant<int> v(42);
  EXPECT_EQ(42, ara::core::get<int>(v));
  // Check qualifier.
  EXPECT_EQ(ConstLRef, ara::core::visit(get_qual, v));
  EXPECT_EQ(ConstRRef, ara::core::visit(get_qual, variant_move(v)));

#ifdef ARA_CORE_CPP11_CONSTEXPR
  /* constexpr */ {
    constexpr ara::core::Variant<int> cv(42);
    static_assert(42 == ara::core::get<int>(cv), "");
    // Check qualifier.
    static_assert(ConstLRef == ara::core::visit(get_qual, cv), "");
    static_assert(ConstRRef == ara::core::visit(get_qual, variant_move(cv)), "");
  }
#endif
}

TEST(Visit, ConstVarConstType) {
  const ara::core::Variant<const int> v(42);
  EXPECT_EQ(42, ara::core::get<const int>(v));
  // Check qualifier.
  EXPECT_EQ(ConstLRef, ara::core::visit(get_qual, v));
  EXPECT_EQ(ConstRRef, ara::core::visit(get_qual, variant_move(v)));

#ifdef ARA_CORE_CPP11_CONSTEXPR
  /* constexpr */ {
    constexpr ara::core::Variant<const int> cv(42);
    static_assert(42 == ara::core::get<const int>(cv), "");
    // Check qualifier.
    static_assert(ConstLRef == ara::core::visit(get_qual, cv), "");
    static_assert(ConstRRef == ara::core::visit(get_qual, variant_move(cv)), "");
  }
#endif
}

struct concat {
  template <typename... Args>
  std::string operator()(const Args &... args) const {
    std::ostringstream strm;
    std::initializer_list<int>({(strm << args, 0)...});
    return variant_move(strm).str();
  }
};

TEST(Visit, Zero) { EXPECT_EQ("", ara::core::visit(concat{})); }

TEST(Visit_Homogeneous, Double) {
  ara::core::Variant<int, std::string> v("hello"), w("world!");
  EXPECT_EQ("helloworld!", ara::core::visit(concat{}, v, w));

#ifdef ARA_CORE_CPP11_CONSTEXPR
  /* constexpr */ {
    constexpr ara::core::Variant<int, double> cv(101), cw(202), cx(3.3);
    struct add_ints {
      constexpr int operator()(int lhs, int rhs) const { return lhs + rhs; }
      constexpr int operator()(int lhs, double) const { return lhs; }
      constexpr int operator()(double, int rhs) const { return rhs; }
      constexpr int operator()(double, double) const { return 0; }
    };  // add
    static_assert(303 == ara::core::visit(add_ints{}, cv, cw), "");
    static_assert(202 == ara::core::visit(add_ints{}, cw, cx), "");
    static_assert(101 == ara::core::visit(add_ints{}, cx, cv), "");
    static_assert(0 == ara::core::visit(add_ints{}, cx, cx), "");
  }
#endif
}

TEST(Visit_Homogeneous, Quintuple) {
  ara::core::Variant<int, std::string> v(101), w("+"), x(202), y("="), z(303);
  EXPECT_EQ("101+202=303", ara::core::visit(concat{}, v, w, x, y, z));
}

TEST(Visit_Heterogeneous, Double) {
  ara::core::Variant<int, std::string> v("hello");
  ara::core::Variant<double, const char *> w("world!");
  EXPECT_EQ("helloworld!", ara::core::visit(concat{}, v, w));
}

TEST(Visit_Heterogenous, Quintuple) {
  ara::core::Variant<int, double> v(101);
  ara::core::Variant<const char *> w("+");
  ara::core::Variant<bool, std::string, int> x(202);
  ara::core::Variant<char, std::string, const char *> y('=');
  ara::core::Variant<long, short> z(303L);
  EXPECT_EQ("101+202=303", ara::core::visit(concat{}, v, w, x, y, z));
}

/******************swap*********************/

TEST(Swap, Same) {
  ara::core::Variant<int, std::string> v("hello");
  ara::core::Variant<int, std::string> w("world");
  // Check `v`.
  EXPECT_EQ("hello", ara::core::get<std::string>(v));
  // Check `w`.
  EXPECT_EQ("world", ara::core::get<std::string>(w));
  // Swap.
  using std::swap;
  swap(v, w);
  // Check `v`.
  EXPECT_EQ("world", ara::core::get<std::string>(v));
  // Check `w`.
  EXPECT_EQ("hello", ara::core::get<std::string>(w));
}

TEST(Swap, Different) {
  ara::core::Variant<int, std::string> v(42);
  ara::core::Variant<int, std::string> w("hello");
  // Check `v`.
  EXPECT_EQ(42, ara::core::get<int>(v));
  // Check `w`.
  EXPECT_EQ("hello", ara::core::get<std::string>(w));
  // Swap.
  using std::swap;
  swap(v, w);
  // Check `v`.
  EXPECT_EQ("hello", ara::core::get<std::string>(v));
  // Check `w`.
  EXPECT_EQ(42, ara::core::get<int>(w));
}

#ifdef MPARK_EXCEPTIONS
TEST(Swap, OneValuelessByException) {
  // `v` normal, `w` corrupted.
  ara::core::Variant<int, move_thrower_t> v(42), w(42);
  EXPECT_THROW(w = move_thrower_t{}, MoveConstruction);
  EXPECT_EQ(42, ara::core::get<int>(v));
  EXPECT_TRUE(w.valueless_by_exception());
  // Swap.
  using std::swap;
  swap(v, w);
  // Check `v`, `w`.
  EXPECT_TRUE(v.valueless_by_exception());
  EXPECT_EQ(42, ara::core::get<int>(w));
}

TEST(Swap, BothValuelessByException) {
  // `v`, `w` both corrupted.
  ara::core::Variant<int, move_thrower_t> v(42);
  EXPECT_THROW(v = move_thrower_t{}, MoveConstruction);
  ara::core::Variant<int, move_thrower_t> w(v);
  EXPECT_TRUE(v.valueless_by_exception());
  EXPECT_TRUE(w.valueless_by_exception());
  // Swap.
  using std::swap;
  swap(v, w);
  // Check `v`, `w`.
  EXPECT_TRUE(v.valueless_by_exception());
  EXPECT_TRUE(w.valueless_by_exception());
}
#endif

TEST(Swap, DtorsSame) {
  struct Obj {
    Obj(size_t *dtor_count) : dtor_count_(dtor_count) {}
    Obj(const Obj &) = default;
    Obj(Obj &&) = default;
    ~Obj() { ++(*dtor_count_); }
    Obj &operator=(const Obj &) = default;
    Obj &operator=(Obj &&) = default;
    size_t *dtor_count_;
  };  // Obj
  size_t v_count = 0;
  size_t w_count = 0;
  {
    ara::core::Variant<Obj> v{&v_count}, w{&w_count};
    using std::swap;
    swap(v, w);
    // Calls `std::swap(Obj &lhs, Obj &rhs)`, with which we perform:
    // ```
    // {
    //   Obj temp(move(lhs));
    //   lhs = move(rhs);
    //   rhs = move(temp);
    // }  `++v_count` from `temp::~Obj()`.
    // ```
    EXPECT_EQ(1u, v_count);
    EXPECT_EQ(0u, w_count);
  }
  EXPECT_EQ(2u, v_count);
  EXPECT_EQ(1u, w_count);
}

namespace detail {

struct Obj {
  Obj(size_t *dtor_count) : dtor_count_(dtor_count) {}
  Obj(const Obj &) = default;
  Obj(Obj &&) = default;
  ~Obj() { ++(*dtor_count_); }
  Obj &operator=(const Obj &) = default;
  Obj &operator=(Obj &&) = default;
  size_t *dtor_count_;
};  // Obj

static void swap(Obj &lhs, Obj &rhs) noexcept {
  std::swap(lhs.dtor_count_, rhs.dtor_count_);
}

}  // namespace detail

TEST(Swap, DtorsSameWithSwap) {
  size_t v_count = 0;
  size_t w_count = 0;
  {
    ara::core::Variant<detail::Obj> v{&v_count}, w{&w_count};
    using std::swap;
    swap(v, w);
    // Calls `detail::swap(Obj &lhs, Obj &rhs)`, with which doesn't call any destructors.
    EXPECT_EQ(0u, v_count);
    EXPECT_EQ(0u, w_count);
  }
  EXPECT_EQ(1u, v_count);
  EXPECT_EQ(1u, w_count);
}

TEST(Swap, DtorsDifferent) {
  struct V {
    V(size_t *dtor_count) : dtor_count_(dtor_count) {}
    V(const V &) = default;
    V(V &&) = default;
    ~V() { ++(*dtor_count_); }
    V &operator=(const V &) = default;
    V &operator=(V &&) = default;
    size_t *dtor_count_;
  };  // V
  struct W {
    W(size_t *dtor_count) : dtor_count_(dtor_count) {}
    W(const W &) = default;
    W(W &&) = default;
    ~W() { ++(*dtor_count_); }
    W &operator=(const W &) = default;
    W &operator=(W &&) = default;
    size_t *dtor_count_;
  };  // W
  size_t v_count = 0;
  size_t w_count = 0;
  {
    ara::core::Variant<V, W> v{ara::core::in_place_type_t<V>{}, &v_count};
    ara::core::Variant<V, W> w{ara::core::in_place_type_t<W>{}, &w_count};
    using std::swap;
    swap(v, w);
    EXPECT_EQ(1u, v_count);
    EXPECT_EQ(2u, w_count);
  }
  EXPECT_EQ(2u, v_count);
  EXPECT_EQ(3u, w_count);
}

/********************************Hash****************************************/

TEST(Hash, Monostate) {
  ara::core::Variant<int, ara::core::Monostate, std::string> v(ara::core::Monostate{});
  // Construct hash function objects.
  std::hash<ara::core::Monostate> monostate_hash;
  std::hash<ara::core::Variant<int, ara::core::Monostate, std::string>> variant_hash;
  // Check the hash.
  EXPECT_NE(monostate_hash(ara::core::Monostate{}), variant_hash(v));
}

TEST(Hash, String) {
  ara::core::Variant<int, std::string> v("hello");
  EXPECT_EQ("hello", ara::core::get<std::string>(v));
  // Construct hash function objects.
  std::hash<std::string> string_hash;
  std::hash<ara::core::Variant<int, std::string>> variant_hash;
  // Check the hash.
  EXPECT_NE(string_hash("hello"), variant_hash(v));
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
