#pragma once

#include <array>
#include <limits>
#include <utility>
#include <stdexcept>

#include "CircularLogsIterator.hpp"

/**
 * @brief
It's akin to a circular buffer, but reading counter is not present. It keeps the last T_max_size element.
When all spot are filled, it will instead replace the oldest element.
 *
 * @tparam Type Type of to contain it the structure, needs to be copyable and movable
 * @tparam T_max_size The maximal count of element
 */
template <typename Type, size_t T_max_size> struct CircularLogs {

    using size_type              = uint8_t;
    using value_type             = Type;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type &;
    using const_reference        = const value_type &;
    using pointer                = value_type *;
    using const_pointer          = const value_type *;
    using iterator               = LogIterator<value_type, T_max_size>;
    using const_iterator         = LogIterator<const value_type, T_max_size>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    [[nodiscard]] constexpr iterator begin()
    {
        return iterator(m_container.data(), (m_full ? m_index : 0), false);
    }
    [[nodiscard]] constexpr iterator end()
    {
        return iterator(m_container.data(), m_index, m_full);
    }
    [[nodiscard]] constexpr const_iterator cbegin() const
    {
        return const_iterator(m_container.data(), (m_full ? m_index : 0),
                              false);
    }
    [[nodiscard]] constexpr const_iterator cend() const
    {
        return const_iterator(m_container.data(), m_index, m_full);
    }
    [[nodiscard]] constexpr reverse_iterator rbegin()
    {
        return std::make_reverse_iterator(end());
    }
    [[nodiscard]] constexpr reverse_iterator rend()
    {
        return std::make_reverse_iterator(begin());
    }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const
    {
        return std::make_reverse_iterator(cend());
    }
    [[nodiscard]] constexpr const_reverse_iterator crend() const
    {
        return std::make_reverse_iterator(cbegin());
    }

    constexpr reference operator[](const difference_type &diff) noexcept
    {
        return *std::next(begin(), diff);
    }
    constexpr const_reference operator[](const difference_type &diff) const noexcept
    {
        return *std::next(cbegin(), diff);
    }

    [[nodiscard]] constexpr bool is_valid_index(const difference_type &diff) const {
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

    [[nodiscard]] constexpr size_type max_size() const noexcept
    {
        return T_max_size;
    }

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

template <typename Type, size_t T_max_size,
          typename WrappingIndexType = uint8_t>
struct IndexedCircularLogs
    : public CircularLogs<std::pair<WrappingIndexType, Type>, T_max_size> {

    using key_type    = WrappingIndexType;
    using mapped_type = Type;
    using value_type  = std::pair<key_type, mapped_type>;

    key_type m_wrappingIndex;

    constexpr IndexedCircularLogs(
        const key_type t_wrappingIndex =
            std::numeric_limits<key_type>::max()) noexcept
        : m_wrappingIndex(std::move(t_wrappingIndex))
    {
    }
    ~IndexedCircularLogs() noexcept = default;
    constexpr IndexedCircularLogs(const IndexedCircularLogs &other) noexcept =
        default;
    constexpr IndexedCircularLogs(IndexedCircularLogs &&other) noexcept =
        default;
    // cppcheck-suppress operatorEq
    constexpr IndexedCircularLogs &
    operator=(const IndexedCircularLogs &other) noexcept = default;
    // cppcheck-suppress operatorEq
    constexpr IndexedCircularLogs &
    operator=(IndexedCircularLogs &&other) noexcept = default;

    constexpr void push_back(const mapped_type &a)
    {
        CircularLogs<value_type, T_max_size>::push_back(
            std::make_pair(++m_wrappingIndex, a));
    }
    constexpr void push_back(mapped_type &&a)
    {
        CircularLogs<value_type, T_max_size>::push_back(
            std::make_pair(++m_wrappingIndex, std::move(a)));
    }
};