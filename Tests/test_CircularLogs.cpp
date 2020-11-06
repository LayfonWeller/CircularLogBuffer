#define DEBUG_LOGITERATOR 1
#define DEBUG
#include <assert.h>
#include "CircularLogs.hpp"
#include <array>

#include <string_view>
#include <algorithm>
#include <iostream>

namespace std
{
template <typename type0, typename type1>
std::ostream &operator<<(std::ostream &os, const std::pair<type0, type1> value)
{
    os << "{" << (int)value.first << ":" << value.second << "}";
    return os;
}
} // namespace std

template <typename CirLogs> void testCircularLogs(CirLogs test = CirLogs())
{
    int i = 0;
    do {
        std::cout << " [" << i << "] => ";
        std::copy(std::make_reverse_iterator(test.end()),
                  std::make_reverse_iterator(test.begin()),
                  std::ostream_iterator<typename CirLogs::value_type>(std::cout,
                                                                      ", "));

        std::cout << " <==> ";

        std::copy(test.begin(), test.end(),
                  std::ostream_iterator<typename CirLogs::value_type>(std::cout,
                                                                      ", "));

        std::cout << "\n";

        test.push_back(i);

        std::cout << (size_t)test.size() << "/" << (size_t)test.max_size()
                  << "\n";

        for (auto f = 0u; f < 10; ++f) {
            try {
                typename CirLogs::reference val =  test.at(f);
                std::cout << "[AT] : " << f << " resulted with " << val << "\n";
            } catch (const std::out_of_range &err) {
                std::cout << "[AT] Invalid index "<< f << " : " << err.what() << "\n";
            }
        }

        for (auto f = 0u; f < 10; ++f) {
            typename CirLogs::value_type val;
            if (test.get(f, val)) {
                std::cout << "[GET] : " << f << " resulted with " << val << "\n";
            } else  {
                std::cout << "[GET] : Invalid index " << f << "\n";
            }
        }

        for (auto f = 0u; f < 10; ++f) {
            try {
                typename CirLogs::reference val =  test.r_at(f);
                std::cout << "[R_AT] : " << f << " resulted with " << val << "\n";
            } catch (const std::out_of_range &err) {
                std::cout << "[R_AT] Invalid index "<< f << " : " << err.what() << "\n";
            }
        }

        for (auto f = 0u; f < 10; ++f) {
            typename CirLogs::value_type val;
            if (test.r_get(f, val)) {
                std::cout << "[R_GET] : " << f << " resulted with " << val << "\n";
            } else  {
                std::cout << "[R_GET] : Invalid index " << f << "\n";
            }
        }

    } while (i++ < 20);

    std::cout << "\n\n\nSorting = \n";
    std::stable_sort(std::rbegin(test), std::rend(test));

    std::cout << " [" << i << "] =>";
    std::copy(
        test.crbegin(), test.crend(),
        std::ostream_iterator<typename CirLogs::value_type>(std::cout, ", "));

    std::cout << " <==> ";

    std::copy(
        test.cbegin(), test.cend(),
        std::ostream_iterator<typename CirLogs::value_type>(std::cout, ", "));

    std::cout << "\n";
}

int main()
{
    std::cout << "\n\nCircularLogs = \n";
    testCircularLogs<CircularLogs<int, 10>>();

    std::cout << "\n\nIndexedCircularLogs = \n";
    testCircularLogs<IndexedCircularLogs<int, 10>>(
        IndexedCircularLogs<int, 10>(33));

    return 0;
}