// C++17 variable template examples
// Reference: https://en.cppreference.com/w/cpp/language/variable_template
// Compile with: g++ -std=c++17 -O2 cpp_17/variable_templates_examples.cpp -o bin/cpp17_variable_templates && ./bin/cpp17_variable_templates

#include <iostream>
#include <type_traits>

// Problem solved: makes shared compile-time variables safe across translation units with inline variables.
// Before C++17: header-defined variables risked multiple definitions or required out-of-class definitions.
// Tips for C++17 variable templates:
// - Use inline constexpr variable templates for header-friendly definitions.
// - Combine variable templates with _v type-trait aliases for readable policies.
// - Prefer named constants when a value is part of an API contract.
// - Do not use variable templates for state that should be runtime-configurable.

template <typename T>
inline constexpr bool isNumeric = std::is_arithmetic_v<T>;

template <typename T>
inline constexpr T epsilon = static_cast<T>(0.000001);

template <typename T, T Value>
inline constexpr T value = Value;

int main() {
    std::cout << "C++17 variable template examples:\n";

    // 1. Inline variable templates avoid one-definition problems in headers.
    static_assert(isNumeric<int>);
    static_assert(isNumeric<double>);
    std::cout << "isNumeric<int> = " << std::boolalpha << isNumeric<int> << "\n";

    // 2. Values can be specialized by type while retaining one interface.
    std::cout << "epsilon<float> = " << epsilon<float> << "\n";
    std::cout << "epsilon<double> = " << epsilon<double> << "\n";

    // 3. A type and non-type parameter can define a typed constant.
    static_assert(value<int, 42> == 42);
    std::cout << "value<int, 42> = " << value<int, 42> << "\n";

    return 0;
}
