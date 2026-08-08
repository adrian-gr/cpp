// C++11 type traits examples
// Reference: https://en.cppreference.com/w/cpp/types
// Compile with: g++ -std=c++11 -O2 cpp_11/type_traits_examples.cpp -o bin/cpp11_type_traits && ./bin/cpp11_type_traits

#include <iostream>
#include <type_traits>

// Problem solved: makes compile-time type decisions and validation available to templates.
// Before C++11: code used compiler extensions, overload duplication, or manual trait structs.
// Tips for C++11 type traits usage:
// - Use traits to inspect types without creating runtime objects.
// - Use ::value because the _v shorthand was introduced later.
// - Combine traits with enable_if when an overload should exist only for valid types.
// - Prefer clear constraints over deeply nested metaprogramming.

template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
addOne(T value) {
    return value + 1;
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
addOne(T value) {
    return value + static_cast<T>(1.0);
}

int main() {
    std::cout << "C++11 type traits examples:\n";

    // 1. Inspect fundamental type categories at compile time.
    static_assert(std::is_integral<int>::value, "int should be integral");
    static_assert(std::is_floating_point<double>::value,
                  "double should be floating point");
    static_assert(std::is_same<std::remove_const<const int>::type, int>::value,
                  "remove_const should produce int");

    // 2. enable_if selects an overload based on the argument type.
    std::cout << "addOne(4) = " << addOne(4) << "\n";
    std::cout << "addOne(2.5) = " << addOne(2.5) << "\n";

    // 3. Traits can expose whether a type is constructible or copyable.
    std::cout << "string is copy constructible = "
              << std::boolalpha
              << std::is_copy_constructible<std::string>::value << "\n";

    return 0;
}
