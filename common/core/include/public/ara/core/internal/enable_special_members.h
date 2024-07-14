#ifndef TUSIMPLEAP_ARA_CORE_INTERNAL_ENABLE_SPECIAL_MEMBERS_H_
#define TUSIMPLEAP_ARA_CORE_INTERNAL_ENABLE_SPECIAL_MEMBERS_H_

#include <type_traits>

namespace ara {
namespace core {
namespace internal {

struct EnableDefaultConstructor_tag {
  explicit constexpr EnableDefaultConstructor_tag() = default;
};

/**
 * @brief A mixin helper to conditionally enable or disable the default
 * constructor.
 * @sa EnableSpecialMembers
 */
template <bool _Switch, typename _Tag = void>
struct EnableDefaultConstructor {
  constexpr EnableDefaultConstructor() noexcept = default;
  constexpr EnableDefaultConstructor(
      EnableDefaultConstructor const&) noexcept = default;
  constexpr EnableDefaultConstructor(
      EnableDefaultConstructor&&) noexcept = default;
  EnableDefaultConstructor& operator=(
      EnableDefaultConstructor const&) noexcept = default;
  EnableDefaultConstructor& operator=(
      EnableDefaultConstructor&&) noexcept = default;

  // Can be used in other ctors.
  constexpr explicit EnableDefaultConstructor(
      EnableDefaultConstructor_tag) {}
};

/**
 * @brief A mixin helper to conditionally enable or disable the default
 * destructor.
 * @sa EnableSpecialMembers
 */
template <bool _Switch, typename _Tag = void>
struct EnableDestructor {};

/**
 * @brief A mixin helper to conditionally enable or disable the copy/move
 * special members.
 * @sa EnableSpecialMembers
 */
template <
    bool _Copy,
    bool _CopyAssignment,
    bool _Move,
    bool _MoveAssignment,
    typename _Tag = void>
struct EnableCopyMove {};

/**
 * @brief A mixin helper to conditionally enable or disable the special
 * members.
 *
 * The @c _Tag type parameter is to make mixin bases unique and thus avoid
 * ambiguities.
 */
template <
    bool _Default,
    bool _Destructor,
    bool _Copy,
    bool _CopyAssignment,
    bool _Move,
    bool _MoveAssignment,
    typename _Tag = void>
struct EnableSpecialMembers
    : private EnableDefaultConstructor<_Default, _Tag>,
      private EnableDestructor<_Destructor, _Tag>,
      private EnableCopyMove<
          _Copy,
          _CopyAssignment,
          _Move,
          _MoveAssignment,
          _Tag> {};

// Boilerplate follows.

template <typename _Tag>
struct EnableDefaultConstructor<false, _Tag> {
  constexpr EnableDefaultConstructor() noexcept = delete;
  constexpr EnableDefaultConstructor(
      EnableDefaultConstructor const&) noexcept = default;
  constexpr EnableDefaultConstructor(
      EnableDefaultConstructor&&) noexcept = default;
  EnableDefaultConstructor& operator=(
      EnableDefaultConstructor const&) noexcept = default;
  EnableDefaultConstructor& operator=(
      EnableDefaultConstructor&&) noexcept = default;

  // Can be used in other ctors.
  constexpr explicit EnableDefaultConstructor(
      EnableDefaultConstructor_tag) {}
};

template <typename _Tag>
struct EnableDestructor<false, _Tag> {
  ~EnableDestructor() noexcept = delete;
};

template <typename _Tag>
struct EnableCopyMove<false, true, true, true, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <typename _Tag>
struct EnableCopyMove<true, false, true, true, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <typename _Tag>
struct EnableCopyMove<false, false, true, true, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <typename _Tag>
struct EnableCopyMove<true, true, false, true, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <typename _Tag>
struct EnableCopyMove<false, true, false, true, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <typename _Tag>
struct EnableCopyMove<true, false, false, true, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <typename _Tag>
struct EnableCopyMove<false, false, false, true, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = default;
};

template <typename _Tag>
struct EnableCopyMove<true, true, true, false, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <typename _Tag>
struct EnableCopyMove<false, true, true, false, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <typename _Tag>
struct EnableCopyMove<true, false, true, false, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <typename _Tag>
struct EnableCopyMove<false, false, true, false, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <typename _Tag>
struct EnableCopyMove<true, true, false, false, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <typename _Tag>
struct EnableCopyMove<false, true, false, false, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = default;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <typename _Tag>
struct EnableCopyMove<true, false, false, false, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

template <typename _Tag>
struct EnableCopyMove<false, false, false, false, _Tag> {
  constexpr EnableCopyMove() noexcept = default;
  constexpr EnableCopyMove(EnableCopyMove const&) noexcept = delete;
  constexpr EnableCopyMove(EnableCopyMove&&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove const&) noexcept = delete;
  EnableCopyMove& operator=(EnableCopyMove&&) noexcept = delete;
};

}  // namespace internal
}  // namespace core
}  // namespace ara

#endif //TUSIMPLEAP_ARA_CORE_INTERNAL_ENABLE_SPECIAL_MEMBERS_H_
