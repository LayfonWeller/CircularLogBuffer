#if defined(DEBUG) && defined(DEBUG_LOGITERATOR) && DEBUG_LOGITERATOR
// #include <assert.h>

#pragma GCC push_options
#pragma GCC optimize("O0")

#define DEBUG_PART(a)                                                    \
    {                                                                    \
        printf("[%s:%d] num=%lu Second Half = %s\n", __FILE__, __LINE__, \
               a->num, (a->partOfSecondHalf ? "true" : "false"));        \
    }
// #define DEBUG_PART(a)                                                          \
//     {                                                                          \
//         assert(num >= 0 && num < max_size);                                    \
//     }
#else
#ifdef DEBUG_LOGITERATOR
#undef DEBUG_LOGITERATOR
#endif
#define DEBUG_LOGITERATOR 0
#define DEBUG_PART(a) \
    {                 \
    }
#endif // DEBUG

#include <iterator>

/**
 * @brief Iterator for the Circular logs
 * @details Is an random iterator type
 * @remark Should not need to be called manually, but it can
 *
 * @tparam Type Type of each element
 * @tparam max_size Number of element in the array
 */
template <typename Type, size_t max_size>
class LogIterator
{
public:
    // iterator traits
    using difference_type = ptrdiff_t;
    using value_type = Type;
    using pointer = value_type *;
    using reference = value_type &;
    using const_pointer = const value_type *;
    using const_reference = const value_type &;
    using iterator_category = std::random_access_iterator_tag;

private:
    pointer root;
    size_t num = 0;
    bool partOfSecondHalf =
        false; // part of the second half is it's after the roll over point
               // (between 0 and index if logged filled)

    constexpr void decrimentIndex(const difference_type &t_howMuch) noexcept
    {
        #if DEBUG_LOGITERATOR
        printf("[%s:%d] this->num=%lu ; Decrement by : %ld\n", __FILE__,
               __LINE__, this->num, t_howMuch);
        #endif
        if (t_howMuch < 0)
            return incrementIndex(-t_howMuch);
        if (t_howMuch == 0)
            return;
        const int newNum = num - t_howMuch;
#if DEBUG_LOGITERATOR
        const auto old = num;
#endif
        if (newNum < 0)
        {
#if DEBUG_LOGITERATOR
            assert(partOfSecondHalf == true);
#endif
            partOfSecondHalf = false;
            num = (max_size + (newNum));
        }
        else
        {
            num = newNum;
        }
#if DEBUG_LOGITERATOR
        assert(num < max_size);
        // printf("[%s:%d] Decrease NUM from %lu by %lu, to %lu\n", __FILE__,
        //        __LINE__, old, t_howMuch, num);
#endif
    }
    constexpr void incrementIndex(const difference_type &t_howMuch) noexcept
    {
        #if DEBUG_LOGITERATOR
        printf("[%s:%d] this->num=%lu ; Incriment by : %ld\n", __FILE__,
               __LINE__, this->num, t_howMuch);
        #endif
        if (t_howMuch < 0)
            return decrimentIndex(-t_howMuch);
        if (t_howMuch == 0)
            return;
        const size_t newNum = num + t_howMuch;
#if DEBUG_LOGITERATOR
        const auto old = num;
#endif
        if (newNum >= max_size)
        {
#if DEBUG_LOGITERATOR
            assert(partOfSecondHalf == false);
#endif
            partOfSecondHalf = true;
            num = newNum - max_size;
        }
        else
        {
            num = newNum;
        }
#if DEBUG_LOGITERATOR
        printf("[%s:%d] Increased NUM from %lu by %lu, to %lu\n", __FILE__,
               __LINE__, old, t_howMuch, num);
        assert(num < max_size);
#endif
    }

public:
    constexpr LogIterator(pointer t_root, int t_num, bool t_partOfSecondHalf)
        : root(t_root), num(t_num), partOfSecondHalf(t_partOfSecondHalf)
    {

        if (num > max_size)
        {
            num += max_size;
        }
#if DEBUG_LOGITERATOR
        DEBUG_PART(this);
        assert(num < max_size);
#endif
    }
    constexpr LogIterator(const LogIterator<Type, max_size> &rawIterator) =
        default;
    constexpr LogIterator &operator++() noexcept
    {
        DEBUG_PART(this);
        incrementIndex(1u);
        return *this;
    }
    constexpr LogIterator operator++(int) noexcept
    {
        DEBUG_PART(this);
        LogIterator retval = *this;
        ++(*this);
        return retval;
    }
    constexpr LogIterator &operator--() noexcept
    {
        DEBUG_PART(this);
        decrimentIndex(1u);
        return *this;
    }
    constexpr LogIterator operator--(int) noexcept
    {
        LogIterator retval = *this;
        --(*this);
        return retval;
    }
    [[nodiscard]] constexpr LogIterator &
    operator+=(const difference_type &movement) noexcept
    {
        DEBUG_PART(this);
        incrementIndex(movement);
        return (*this);
    }
    [[nodiscard]] constexpr LogIterator<Type, max_size>
    operator-=(const difference_type &movement) noexcept
    {
        DEBUG_PART(this);
        decrimentIndex(movement);
        return (*this);
    }
    [[nodiscard]] constexpr bool
    operator==(const LogIterator &other) const noexcept
    {
        return (num == other.num) && (partOfSecondHalf == other.partOfSecondHalf);
    }
    [[nodiscard]] constexpr bool
    operator!=(const LogIterator &other) const noexcept
    {
        return !(*this == other);
    }
    /* Can't be const need additional overload
     * for const otherwise the value returned is editable */
    // cppcheck-suppress functionConst
    [[nodiscard]] constexpr reference operator*() { return root[num]; }
    [[nodiscard]] constexpr const_reference operator*() const
    {
        return root[num];
    }
    /* Can't be const need additional overload
     * for const otherwise the value returned is editable */
    // cppcheck-suppress functionConst
    [[nodiscard]] constexpr pointer operator->() { return &root[num]; }
    [[nodiscard]] constexpr const_pointer operator->() const
    {
        return &root[num];
    }
    [[nodiscard]] constexpr difference_type
    operator-(const LogIterator &b) const noexcept
    {
        DEBUG_PART(this);
        DEBUG_PART((&b));

        const auto thisModNum = this->partOfSecondHalf * max_size + this->num;
        const auto bModNum = b.partOfSecondHalf * max_size + b.num;

        difference_type ret = thisModNum - bModNum;

#if DEBUG_LOGITERATOR
        printf("[%s:%d] (this->num=%lu->%lu, b.num=%lu->%lu)\n", __FILE__,
               __LINE__, this->num, thisModNum, b.num, bModNum);
        printf("[%s:%d] distance=%ld (this->num=%lu, b.num=%lu)\n", __FILE__,
               __LINE__, ret, this->num, b.num);
#endif
        return ret;
    }

    [[nodiscard]] constexpr LogIterator
    operator-(const difference_type &movement) const noexcept
    {
        difference_type distance = this->num - movement;
        const bool nowPartOfFirstHalf =
            (static_cast<difference_type>(this->num) - movement) < 0;
        if (nowPartOfFirstHalf)
        {
            distance += max_size;
        }
#if DEBUG_LOGITERATOR
        // printf("[%s:%d] this->num=%lu, partOfSecondHalf=%s, movement=%ld,
        //        distance           = % ld,
        //        nowPartOfFirstHalf = % s,
        //        assert             = % s\n ", __FILE__,
        //                 __LINE__,
        //        this->num, (this->partOfSecondHalf ? "true" : "false"),
        //        movement, distance, (nowPartOfFirstHalf ? "true" : "false"),
        //        !(!this->partOfSecondHalf && nowPartOfFirstHalf) ? "false"
        //                                                         : "true");
        assert(!(!this->partOfSecondHalf && nowPartOfFirstHalf));
#endif
        return LogIterator(root, distance,
                           nowPartOfFirstHalf ? false : this->partOfSecondHalf);
    }

    [[nodiscard]] constexpr LogIterator<Type, max_size>
    operator+(const size_t b) const noexcept
    {
        const difference_type distance = (this->num + b) % max_size;
        const bool nowPartOfSecondHalf = this->num + b >= max_size;
#if DEBUG_LOGITERATOR
        assert(!(this->partOfSecondHalf && nowPartOfSecondHalf));
        // printf("[%s:%d] num=%lu (this->num(%lu) + b(%lu))%%max_size\n",
        //        __FILE__, __LINE__, distance, this->num, b);
#endif
        return LogIterator<Type, max_size>(
            root, distance, this->partOfSecondHalf || nowPartOfSecondHalf);
    }

    [[nodiscard]] constexpr pointer getPtr() const { return &root[num]; }
    [[nodiscard]] constexpr const_pointer getConstPtr() const
    {
        return &root[num];
    }

    bool operator<(const LogIterator<Type, max_size> &t_second) const
    {
        if (t_second.partOfSecondHalf == true && this->partOfSecondHalf == false)
            return true;
        if (t_second.partOfSecondHalf == false && this->partOfSecondHalf == true)
            return false;
        return this->num < t_second.num;
    }
};

#if defined(DEBUG) && defined(DEBUG_LOGITERATOR) && DEBUG_LOGITERATOR
#pragma GCC pop_options
#endif