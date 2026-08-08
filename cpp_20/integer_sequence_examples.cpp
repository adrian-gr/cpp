// C++20 integer sequence examples
// Reference: https://en.cppreference.com/w/cpp/utility/integer_sequence
// Compile with: g++ -std=c++20 -O2 cpp_20/integer_sequence_examples.cpp -o bin/cpp20_integer_sequence && ./bin/cpp20_integer_sequence

#include <concepts>
#include <iostream>
#include <utility>

// Problem solved: constrains compile-time sequence algorithms while keeping pack expansion concise.
// Before C++20: sequence helpers accepted unsupported element types until template instantiation failed.
// Tips for integer_sequence usage:
// - Constrain the element type when the sequence algorithm requires arithmetic or integral values.
// - Use fold expressions for simple reductions and a named helper for more complex policies.
// - Prefer index_sequence for tuple positions and integer_sequence for typed compile-time values.
// - Keep constraints separate from the expansion logic so diagnostics remain readable.

template <std::integral T, T... Values>
constexpr T sumSequence(std::integer_sequence<T, Values...>) {
    return (Values + ... + T{});
}

template <std::integral T, T... Values>
constexpr bool allNonNegative(std::integer_sequence<T, Values...>) {
    return ((Values >= 0) && ...);
}

int main() {
    std::cout << "C++20 integer sequence examples:\n";

    using Numbers = std::integer_sequence<int, 2, 4, 6>;
    constexpr int total = sumSequence(Numbers{});
    constexpr bool valid = allNonNegative(Numbers{});
    static_assert(total == 12);
    static_assert(valid);
    std::cout << "constrained sequence sum = " << total << "\n";
    std::cout << "all values non-negative = " << std::boolalpha << valid << "\n";

    return 0;
}
