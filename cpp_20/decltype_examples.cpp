// C++20 decltype examples
// Reference: https://en.cppreference.com/w/cpp/language/decltype
// Compile with: g++ -std=c++20 -O2 cpp_20/decltype_examples.cpp -o bin/cpp20_decltype && ./bin/cpp20_decltype

#include <concepts>
#include <iostream>
#include <type_traits>
#include <utility>

// Problem solved: combines exact expression types with readable constrained generic interfaces.
// Before C++20: code used SFINAE and verbose detection traits to validate expressions.
// Tips for C++20 decltype usage:
// - Use decltype in concepts and requires expressions to test whether an expression is valid.
// - Prefer named concepts when the same expression requirement is used more than once.
// - Use decltype(auto) when a forwarding helper must preserve the exact result type.
// - Keep constraints readable; a complicated decltype expression may deserve a named concept.

template <typename T>
concept Addable = requires(T left, T right) {
    { left + right } -> std::convertible_to<T>;
};

template <Addable T>
constexpr decltype(auto) add(const T& left, const T& right) {
    return left + right;
}

template <typename T>
decltype(auto) identity(T&& value) {
    return std::forward<T>(value);
}

int main() {
    std::cout << "C++20 decltype examples:\n";

    // 1. A requires expression checks the type of an expression with decltype semantics.
    constexpr int sum = add(2, 3);
    static_assert(std::is_same<decltype(sum), const int>::value,
                  "constexpr variable should be const int");
    std::cout << "add(2, 3) = " << sum << "\n";

    // 2. decltype(auto) preserves the value category through a forwarding helper.
    int number = 10;
    decltype(auto) reference = identity(number);
    reference = 20;
    static_assert(std::is_same<decltype(reference), int&>::value,
                  "forwarded lvalue should remain a reference");
    std::cout << "number after forwarding = " << number << "\n";

    // 3. The helper preserves an rvalue type; copy it into a safe value.
    static_assert(std::is_same<decltype(identity(42)), int&&>::value,
                  "forwarded rvalue should remain an rvalue reference");
    auto copy = identity(42);
    static_assert(std::is_same<decltype(copy), int>::value,
                  "auto should store the forwarded rvalue as a value");
    std::cout << "forwarded rvalue = " << copy << "\n";

    return 0;
}
