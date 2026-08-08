// C++20 type traits examples
// Reference: https://en.cppreference.com/w/cpp/types
// Compile with: g++ -std=c++20 -O2 cpp_20/type_traits_examples.cpp -o bin/cpp20_type_traits && ./bin/cpp20_type_traits

#include <iostream>
#include <string>
#include <type_traits>

// Problem solved: provides concise, modern type transformations for constrained generic code.
// Before C++20: code needed remove_cv and remove_reference separately or verbose trait aliases.
// Tips for C++20 type traits usage:
// - Use remove_cvref_t when both cv-qualifiers and references should be removed.
// - Use type_identity to prevent unwanted template argument deduction in generic interfaces.
// - Use _v and _t aliases to keep trait expressions readable.
// - Prefer concepts for semantic requirements and type traits for mechanical type properties.

template <typename T>
using ValueType = std::remove_cvref_t<T>;

template <typename T>
struct IdentityHolder {
    using type = T;
};

template <typename T>
void printTypeCategory(const T& value) {
    using Value = ValueType<T>;
    std::cout << value << " is integral = " << std::boolalpha
              << std::is_integral_v<Value> << "\n";
}

template <typename T>
void acceptExact(std::type_identity_t<T> value) {
    std::cout << "non-deduced value = " << value << "\n";
}

int main() {
    std::cout << "C++20 type traits examples:\n";

    // 1. remove_cvref_t replaces two common transformations with one alias.
    static_assert(std::is_same_v<ValueType<const int&>, int>);
    printTypeCategory(42);
    printTypeCategory(std::string("text"));

    // 2. type_identity_t prevents deduction for the parameter type.
    acceptExact<int>(7);

    // 3. Traits remain useful alongside concepts for mechanical properties.
    static_assert(std::is_object_v<std::string>);
    static_assert(std::is_nothrow_move_constructible_v<int>);
    std::cout << "string is an object type = " << std::is_object_v<std::string> << "\n";

    return 0;
}
