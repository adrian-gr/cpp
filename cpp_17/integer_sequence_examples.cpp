// C++17 integer sequence examples
// Reference: https://en.cppreference.com/w/cpp/utility/integer_sequence
// Compile with: g++ -std=c++17 -O2 cpp_17/integer_sequence_examples.cpp -o bin/cpp17_integer_sequence && ./bin/cpp17_integer_sequence

#include <iostream>
#include <utility>

// Problem solved: combines compile-time value sequences with fold expressions and simpler pack processing.
// Before C++17: code needed recursive pack expansion to consume an integer sequence.
// Tips for integer_sequence usage:
// - Use fold expressions to reduce an integer sequence without a recursive base case.
// - make_integer_sequence<T, N> generates values from zero through N - 1.
// - Use index_sequence when positions matter and integer_sequence when values matter.
// - Keep large compile-time sequences bounded because they increase instantiation work.

template <typename T, T... Values>
constexpr T sumSequence(std::integer_sequence<T, Values...>) {
    return (Values + ... + T{});
}

template <std::size_t... Indices>
void printIndices(std::index_sequence<Indices...>) {
    ((std::cout << Indices << ' '), ...);
    std::cout << "\n";
}

int main() {
    std::cout << "C++17 integer sequence examples:\n";

    constexpr auto numbers = std::integer_sequence<int, 1, 3, 5, 7>{};
    constexpr int total = sumSequence(numbers);
    static_assert(total == 16);
    std::cout << "sequence sum = " << total << "\n";

    // A generated index sequence is consumed directly by a fold expression.
    printIndices(std::make_index_sequence<5>{});

    return 0;
}
