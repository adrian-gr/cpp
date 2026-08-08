// C++20 variable template examples
// Reference: https://en.cppreference.com/w/cpp/language/variable_template
// Compile with: g++ -std=c++20 -O2 cpp_20/variable_templates_examples.cpp -o bin/cpp20_variable_templates && ./bin/cpp20_variable_templates

#include <concepts>
#include <iostream>
#include <type_traits>

// Problem solved: combines reusable typed constants with direct constraints on template parameters.
// Before C++20: code used unconstrained variable templates and separate static_assert checks.
// Tips for C++20 variable templates:
// - Constrain a variable template when only a family of types should be accepted.
// - Use concepts for semantic requirements and variable templates for values or policies.
// - Keep compile-time policies small and composable so diagnostics remain understandable.
// - Use constexpr or constinit according to whether the value must be constant-evaluated or initialized early.

template <std::integral T>
inline constexpr bool isSignedIntegral = std::is_signed_v<T>;

template <std::floating_point T>
inline constexpr T tolerance = static_cast<T>(0.0001);

template <typename T>
concept HasValueType = requires {
    typename T::value_type;
};

template <typename T>
inline constexpr bool hasNestedValueType = HasValueType<T>;

int main() {
    std::cout << "C++20 variable template examples:\n";

    // 1. A constrained variable template accepts only integral types.
    static_assert(isSignedIntegral<int>);
    std::cout << "isSignedIntegral<int> = " << std::boolalpha
              << isSignedIntegral<int> << "\n";

    // 2. A concept constrains a type-specific floating-point value.
    std::cout << "tolerance<double> = " << tolerance<double> << "\n";

    // 3. A variable template can expose a type property through a constrained specialization.
    static_assert(hasNestedValueType<std::string>);
    static_assert(!hasNestedValueType<int>);
    std::cout << "string has value_type = "
              << hasNestedValueType<std::string> << "\n";

    return 0;
}
