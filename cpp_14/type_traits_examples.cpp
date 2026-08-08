// C++14 type traits examples
// Reference: https://en.cppreference.com/w/cpp/types
// Compile with: g++ -std=c++14 -O2 cpp_14/type_traits_examples.cpp -o bin/cpp14_type_traits && ./bin/cpp14_type_traits

#include <iostream>
#include <type_traits>
#include <utility>

// Problem solved: simplifies reusable compile-time type transformations and callable inspection.
// Before C++14: traits required longer ::type and ::value forms and more verbose helper aliases.
// Tips for C++14 type traits usage:
// - Use alias templates to name transformed types clearly.
// - Use integral_constant for custom compile-time boolean properties.
// - Use decay when a generic function needs the value-like form of an argument type.
// - Prefer traits for compile-time policy selection instead of runtime type checks.

template <typename T>
using Decayed = typename std::decay<T>::type;

template <typename T>
using RemoveReference = typename std::remove_reference<T>::type;

template <typename T>
struct IsSmall : std::integral_constant<bool, (sizeof(T) <= sizeof(int))> {};

template <typename Callable, typename Argument>
using Result = typename std::result_of<Callable(Argument)>::type;

int main() {
    std::cout << "C++14 type traits examples:\n";

    // 1. Alias templates make transformed types easier to read.
    static_assert(std::is_same<Decayed<const int&>, int>::value,
                  "decay should remove const and reference");
    static_assert(std::is_same<RemoveReference<double&>, double>::value,
                  "remove_reference should produce double");

    // 2. A custom trait can express a reusable compile-time property.
    static_assert(IsSmall<int>::value, "int should be small on this platform");
    std::cout << "char is small = " << std::boolalpha << IsSmall<char>::value << "\n";

    // 3. result_of inspects the return type of a callable.
    auto multiply = [](double value) {
        return value * 2.0;
    };
    static_assert(std::is_same<Result<decltype(multiply), double>, double>::value,
                  "callable result should be double");
    std::cout << "callable result = " << multiply(3.5) << "\n";

    return 0;
}
