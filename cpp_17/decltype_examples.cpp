// C++17 decltype examples
// Compile with: g++ -std=c++17 -O2 cpp_17/decltype_examples.cpp -o bin/cpp17_decltype && ./bin/cpp17_decltype

#include <iostream>
#include <tuple>
#include <type_traits>

// Tips for C++17 decltype usage:
// - Use decltype with structured bindings to inspect the binding's declared type.
// - Remember that decltype(name) for a structured binding follows the referenced element type.
// - Use decltype(auto) when generic code must preserve references and qualifiers.
// - Prefer clear named types when decltype would make an interface difficult to read.

template <typename Tuple>
decltype(auto) secondElement(Tuple& tuple) {
    return std::get<1>(tuple);
}

int main() {
    std::cout << "C++17 decltype examples:\n";

    // 1. Structured bindings work naturally with decltype.
    std::pair<int, double> values{7, 2.5};
    auto [count, ratio] = values;
    static_assert(std::is_same<decltype(count), int>::value,
                  "structured binding should expose the element type");
    static_assert(std::is_same<decltype(ratio), double>::value,
                  "structured binding should expose the element type");
    std::cout << "count = " << count << ", ratio = " << ratio << "\n";

    // 2. decltype(auto) preserves a tuple element reference.
    auto tuple = std::make_tuple(10, std::string("C++17"));
    decltype(auto) text = secondElement(tuple);
    text = "decltype(auto) reference";
    static_assert(std::is_same<decltype(text), std::string&>::value,
                  "decltype(auto) should preserve the tuple element reference");
    std::cout << "tuple text = " << std::get<1>(tuple) << "\n";

    // 3. decltype distinguishes a named variable from a parenthesized expression.
    int number = 42;
    static_assert(std::is_same<decltype(number), int>::value,
                  "unparenthesized variable gives its declared type");
    static_assert(std::is_same<decltype((number)), int&>::value,
                  "parenthesized lvalue gives a reference type");

    return 0;
}
