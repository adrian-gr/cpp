// C++14 integer sequence examples
// Reference: https://en.cppreference.com/w/cpp/utility/integer_sequence
// Compile with: g++ -std=c++14 -O2 cpp_14/integer_sequence_examples.cpp -o bin/cpp14_integer_sequence && ./bin/cpp14_integer_sequence

#include <iostream>
#include <string>
#include <tuple>
#include <utility>

// Problem solved: expands compile-time index packs without hand-written recursive generators.
// Before C++14: code implemented custom index-sequence metaprogramming or repeated overloads.
// Tips for integer_sequence usage:
// - Use index_sequence to expand tuple or array indices in generic code.
// - Use make_index_sequence<N> for the range [0, N).
// - Keep sequence helpers small and hide implementation details behind a simple public function.
// - Integer sequences are compile-time tools and do not allocate runtime storage.

template <typename Tuple, std::size_t... Indices>
void printTuple(const Tuple& tuple, std::index_sequence<Indices...>) {
    int unused[] = {0, (std::cout << std::get<Indices>(tuple) << ' ', 0)...};
    (void)unused;
    std::cout << "\n";
}

template <typename... Values>
void printTuple(const std::tuple<Values...>& tuple) {
    printTuple(tuple, std::make_index_sequence<sizeof...(Values)>{});
}

int main() {
    std::cout << "C++14 integer sequence examples:\n";

    // make_index_sequence generates one compile-time index for each tuple element.
    auto values = std::make_tuple(std::string("sequence"), 14, 2.5);
    printTuple(values);

    // integer_sequence can represent values rather than indices.
    using Numbers = std::integer_sequence<int, 2, 4, 6, 8>;
    std::cout << "sequence size = " << Numbers::size() << "\n";

    return 0;
}
