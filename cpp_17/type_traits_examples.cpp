// C++17 type traits examples
// Reference: https://en.cppreference.com/w/cpp/types
// Compile with: g++ -std=c++17 -O2 cpp_17/type_traits_examples.cpp -o bin/cpp17_type_traits && ./bin/cpp17_type_traits

#include <functional>
#include <iostream>
#include <type_traits>

// Problem solved: makes common trait values and detection patterns concise and usable in generic code.
// Before C++17: code used verbose ::value forms and hand-written detection utilities.
// Tips for C++17 type traits usage:
// - Prefer the _v variable templates for readable trait checks.
// - Use void_t to detect whether a type provides a required nested type or expression.
// - Use invoke traits to inspect arbitrary callable objects uniformly.
// - Type traits guide compile-time selection; they do not replace a clear public interface.

template <typename T, typename = void>
struct HasValueType : std::false_type {};

template <typename T>
struct HasValueType<T, std::void_t<typename T::value_type>> : std::true_type {};

struct WithValueType {
    using value_type = int;
};

struct WithoutValueType {};

int main() {
    std::cout << "C++17 type traits examples:\n";

    // 1. _v aliases make common checks concise.
    static_assert(std::is_integral_v<int>);
    static_assert(std::is_same_v<std::remove_cv_t<const int>, int>);
    std::cout << "double is arithmetic = " << std::boolalpha
              << std::is_arithmetic_v<double> << "\n";

    // 2. void_t detects a nested type without causing a hard compilation error.
    static_assert(HasValueType<WithValueType>::value);
    static_assert(!HasValueType<WithoutValueType>::value);
    std::cout << "WithValueType detected = "
              << HasValueType<WithValueType>::value << "\n";

    // 3. Invoke traits inspect callable validity and result types.
    auto convert = [](int value) {
        return value * 1.5;
    };
    static_assert(std::is_invocable_v<decltype(convert), int>);
    static_assert(std::is_same_v<std::invoke_result_t<decltype(convert), int>, double>);
    std::cout << "is nothrow invocable = " << std::is_nothrow_invocable_v<decltype(convert), int>
              << "\n";

    return 0;
}
