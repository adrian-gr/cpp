// C++17 uniform initialization examples
// Reference: https://en.cppreference.com/w/cpp/language/class_template_argument_deduction
// Compile with: g++ -std=c++17 -O2 cpp_17/uniform_initialization_examples.cpp -o bin/cpp17_uniform_initialization && ./bin/cpp17_uniform_initialization

#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

// Problem solved: reduces repeated type names when initializing templated objects and containers.
// Before C++17: code had to spell template arguments even when the initializer made them obvious.
// Tips for uniform initialization:
// - Class template argument deduction lets braces infer template arguments from constructors.
// - Use explicit template arguments when deduction would make intent ambiguous.
// - CTAD applies to supported constructors and deduction guides, not every aggregate automatically.
// - Braces still reject many narrowing conversions at compile time.

template <typename T>
struct Box {
    T value;
};

template <typename First, typename Second>
struct Pair {
    First first;
    Second second;
};

template <typename T>
Box(T) -> Box<T>;

template <typename First, typename Second>
Pair(First, Second) -> Pair<First, Second>;

int main() {
    std::cout << "C++17 uniform initialization examples:\n";

    // 1. CTAD infers the type from constructor-like aggregate initialization.
    Box boxed{42};
    Pair pair{"answer", 42};
    static_assert(std::is_same<decltype(boxed), Box<int>>::value);
    std::cout << "boxed value = " << boxed.value << "\n";
    std::cout << "pair = " << pair.first << ", " << pair.second << "\n";

    // 2. Containers can combine CTAD with brace initialization.
    std::vector values{1, 2, 3, 4};
    std::cout << "deduced vector size = " << values.size() << "\n";

    // 3. Structured data can be initialized directly with braces.
    std::tuple record{"C++17", 17};
    std::cout << "record number = " << std::get<1>(record) << "\n";

    return 0;
}
