#pragma once

#include <array>
#include <limits>
#include <utility>
#include <stdexcept>

#include "CircularMemoryIterator.hpp"

#if __cplusplus < 201703L
#if __cplusplus == 1
#warning C++ Earlier then C++98
#elif __cplusplus == 199711L
#warning C++98
#elif __cplusplus == 201103L
#warning C++11
#elif __cplusplus == 201402L
#warning C++14
#endif
#Error "Requires C++17 or newer"
#endif
namespace ns_CircularMemory
{
namespace Details
{
template <typename T>
using IsSignedInteger = std::enable_if_t</*std::conjunction_v<std::is_integral_v<T>,*/ std::is_signed_v<T> /*>*/, bool>;
template <typename T> using IsUnsignedInteger = std::enable_if_t<std::is_integral_v<T> && std::is_unsigned_v<T>, bool>;
template <typename T> using remove_cvref_t    = std::remove_cv_t<std::remove_reference_t<T>>;
template <typename T> using remove_cvref      = std::remove_cv<std::remove_reference_t<T>>;
template <typename T1, typename T2>
using IsSameBaseType = std::enable_if_t<std::is_same_v<remove_cvref_t<T1>, remove_cvref_t<T2>>, bool>;

template <typename T1, /*typename T2, IsSameBaseType<T1, T2> = true,*/ IsSignedInteger<T1> = true>
constexpr bool key_lesserThen(T1 t_a, T1 t_b)
{
  return t_b - t_a < static_cast<T1>(0);
}
template <typename T1,/* typename T2, IsSameBaseType<T1, T2> = true,*/ IsUnsignedInteger<T1> = true>
constexpr bool key_lesserThen(T1 t_a, T1 t_b)
{
  using T           = remove_cvref_t<T1>;
  const T halfPoint = std::numeric_limits<T>::max()/ 2;
  const T t1        = t_b + halfPoint - t_a;
  return t1 > halfPoint;
}

// TODO : Will cleanup the code a fair bit
// template <typename T> class KeyType final
// {
// public:
//   constexpr bool operator<(const T t_a)
// }

} // namespace Details

} // namespace ns_CircularMemory

/**
 * @brief
It's akin to a circular buffer, but reading counter is not present. It keeps the last T_max_size element.
When all spot are filled, it will instead replace the oldest element.
 *
 * @tparam Type Type of to contain it the structure, needs to be copyable and movable
 * @tparam T_max_size The maximal count of element
 */
template <typename Type, size_t T_max_size> struct CircularMemory {

  using size_type              = uint8_t;
  using value_type             = Type;
  using difference_type        = std::ptrdiff_t;
  using reference              = value_type &;
  using const_reference        = const value_type &;
  using pointer                = value_type *;
  using const_pointer          = const value_type *;
  using iterator               = CircularMemoryIterator<value_type, T_max_size>;
  using const_iterator         = CircularMemoryIterator<const value_type, T_max_size>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  [[nodiscard]] constexpr iterator begin() noexcept
  {
    return iterator(m_container.data(), (m_full ? m_index : 0), false);
  }
  [[nodiscard]] constexpr const_iterator begin() const noexcept
  {
    return const_iterator(m_container.data(), (m_full ? m_index : 0), false);
  }
  [[nodiscard]] constexpr iterator       end() noexcept { return iterator(m_container.data(), m_index, m_full); }
  [[nodiscard]] constexpr const_iterator end() const noexcept
  {
    return const_iterator(m_container.data(), m_index, m_full);
  }
  [[nodiscard]] constexpr const_iterator         cbegin() const noexcept { return begin(); }
  [[nodiscard]] constexpr const_iterator         cend() const noexcept {return end();};
  [[nodiscard]] constexpr reverse_iterator       rbegin() noexcept { return std::make_reverse_iterator(end()); }
  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return std::make_reverse_iterator(cend()); }
  [[nodiscard]] constexpr reverse_iterator       rend() noexcept { return std::make_reverse_iterator(begin()); }
  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return std::make_reverse_iterator(cbegin()); }
  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  constexpr reference       operator[](const difference_type &diff) noexcept { return *std::next(begin(), diff); }
  constexpr const_reference operator[](const difference_type &diff) const noexcept
  {
    return *std::next(cbegin(), diff);
  }

  [[nodiscard]] constexpr bool is_valid_index(const difference_type &diff) const noexcept
  {
    return diff < size() && diff >= 0;
  }

  [[nodiscard]] constexpr reference at(const difference_type &diff)
  {
    if (is_valid_index(diff))
      return *std::next(begin(), diff);
    throw std::out_of_range("Invalid range required");
  }

  [[nodiscard]] constexpr const_reference at(const difference_type &diff) const
  {
    if (is_valid_index(diff))
      return *std::next(cbegin(), diff);
    throw std::out_of_range("Invalid range required");
  }

  [[nodiscard]] constexpr bool get(const difference_type &diff, reference value)
  {
    if (is_valid_index(diff)) {
      value = *std::next(begin(), diff);
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr bool get(const difference_type &diff, const_reference value) const
  {
    if (is_valid_index(diff)) {
      value = *std::next(cbegin(), diff);
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr reference r_at(const difference_type &diff)
  {
    if (is_valid_index(diff))
      return *std::next(rbegin(), diff);
    throw std::out_of_range("Invalid range required");
  }

  [[nodiscard]] constexpr const_reference r_at(const difference_type &diff) const
  {
    if (is_valid_index(diff))
      return *std::next(crbegin(), diff);
    throw std::out_of_range("Invalid range required");
  }

  [[nodiscard]] constexpr bool r_get(const difference_type &diff, reference value)
  {
    if (is_valid_index(diff)) {
      value = *std::next(rbegin(), diff);
      return true;
    }
    return false;
  }

  [[nodiscard]] constexpr bool r_get(const difference_type &diff, const_reference value) const
  {
    if (is_valid_index(diff)) {
      value = *std::next(crbegin(), diff);
      return true;
    }
    return false;
  }

  constexpr void push_back(const value_type &a)
  {
    m_container.at(m_index++) = a;
    if (m_index == T_max_size) {
      m_index = 0;
      m_full  = true;
    }
  }
  constexpr void push_back(value_type &&a)
  {
    m_container.at(m_index++) = std::move(a);

    if (m_index == T_max_size) {
      m_index = 0;
      m_full  = true;
    }
  }

  [[nodiscard]] constexpr size_type max_size() const noexcept { return T_max_size; }

  [[nodiscard]] constexpr size_type size() const noexcept
  {
    if (m_full)
      return T_max_size;
    return m_index;
  }

  constexpr pointer data() noexcept { return m_container.data(); }

  constexpr const_pointer data() const noexcept { return m_container.data(); }

  constexpr bool empty() const noexcept
  {
    if (m_full)
      return false;
    return m_index == 0;
  }

  constexpr void clear() noexcept
  {
    m_full  = false;
    m_index = 0;
  }

public:
  bool                               m_full  = false;
  uint8_t                            m_index = 0;
  std::array<value_type, T_max_size> m_container;
};

template <typename Type, size_t T_max_size, typename WrappingIndexType = uint8_t>
struct IndexedCircularMemory : public CircularMemory<std::pair<WrappingIndexType, Type>, T_max_size> {

  using key_type    = WrappingIndexType;
  using mapped_type = Type;
  using value_type  = std::pair<key_type, mapped_type>;
  using parent_type = CircularMemory<value_type, T_max_size>;

  key_type m_wrappingIndex;

  constexpr IndexedCircularMemory(const key_type t_wrappingIndex = std::numeric_limits<key_type>::max()) noexcept
      : m_wrappingIndex(std::move(t_wrappingIndex))
  {
  }
  ~IndexedCircularMemory() noexcept                                          = default;
  constexpr IndexedCircularMemory(const IndexedCircularMemory &other) noexcept = default;
  constexpr IndexedCircularMemory(IndexedCircularMemory &&other) noexcept      = default;
  // cppcheck-suppress operatorEq
  constexpr IndexedCircularMemory &operator=(const IndexedCircularMemory &other) noexcept = default;
  // cppcheck-suppress operatorEq
  constexpr IndexedCircularMemory &operator=(IndexedCircularMemory &&other) noexcept = default;

  constexpr void push_back(const mapped_type &a) { parent_type::push_back(std::make_pair(++m_wrappingIndex, a)); }
  constexpr void push_back(mapped_type &&a) { parent_type::push_back(std::make_pair(++m_wrappingIndex, std::move(a))); }

  [[nodiscard]] constexpr typename parent_type::iterator get_index(const key_type t_req_index)
  {
    const auto start_iterator = this->begin();
    const auto end_iterator   = this->rbegin();
    if (start_iterator != this->end() && end_iterator != this->rend()) {
      const key_type start_index = start_iterator->first;
      const key_type last_index  = end_iterator->first;

      constexpr auto lessEqThen = [](const key_type t_a, const key_type t_b) constexpr
      {
        return t_a == t_b || ns_CircularMemory::Details::key_lesserThen(t_a, t_b);
      };
      const bool isBiggerOrEqThenStartIndex = lessEqThen(start_index, t_req_index);
      const bool isLessOrEqThenLastIndex    = lessEqThen(t_req_index, last_index);
      const bool isBetweenOrIndexes         = isBiggerOrEqThenStartIndex && isLessOrEqThenLastIndex;
      if (isBetweenOrIndexes) {
        return std::next(start_iterator, static_cast<key_type>(t_req_index - start_index));
      } else {
        return this->end();
      }
    }
    return this->end();
  }

  [[nodiscard]] constexpr typename parent_type::const_iterator get_index(const key_type t_req_index) const
  {
    const auto start_iterator = this->cbegin();
    const auto end_iterator   = this->crbegin();
    if (start_iterator != this->cend() && end_iterator != this->crend()) {
      const key_type start_index = start_iterator->first;
      const key_type last_index  = end_iterator->first;

      constexpr auto lessEqThen = [](const key_type t_a, const key_type t_b) constexpr
      {
        return t_a == t_b || ns_CircularMemory::Details::key_lesserThen(t_a, t_b);
      };
      const bool isBiggerOrEqThenStartIndex = lessEqThen(start_index, t_req_index);
      const bool isLessOrEqThenLastIndex    = lessEqThen(t_req_index, last_index);
      const bool isBetweenOrIndexes         = isBiggerOrEqThenStartIndex && isLessOrEqThenLastIndex;
      if (isBetweenOrIndexes) {
        return std::next(start_iterator, static_cast<key_type>(t_req_index - start_index));
      } else {
        return this->cend();
      }
    }
    return this->cend();
  }
};
