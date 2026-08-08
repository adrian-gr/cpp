// C++14 variable template examples
// Reference: https://en.cppreference.com/w/cpp/language/variable_template
// Compile with: g++ -std=c++14 -O2 cpp_14/variable_templates_examples.cpp -o bin/cpp14_variable_templates && ./bin/cpp14_variable_templates

#include <iostream>
#include <type_traits>

// Problem solved: gives families of typed constants a concise, reusable definition.
// Before C++14: code used static members, enum tricks, or repeated function templates.
// Tips for variable templates:
// - Use them for compile-time values parameterized by a type or non-type argument.
// - constexpr variable templates are evaluated at compile time when used in constant expressions.
// - Keep the name and type clear because a variable template can hide substantial computation.
// - Use a function template instead when the value needs runtime arguments or complex control flow.

template <typename T>
constexpr T pi = static_cast<T>(3.14159265358979323846);

template <typename T>
constexpr bool isPointer = std::is_pointer<T>::value;

template <int Value>
constexpr int squared = Value * Value;

int main() {
    std::cout << "C++14 variable template examples:\n";

    // 1. The same named constant has a type-specific value.
    static_assert(pi<float> > 3.14f && pi<float> < 3.15f,
                  "float pi should be available at compile time");
    std::cout << "pi<float> = " << pi<float> << "\n";
    std::cout << "pi<double> = " << pi<double> << "\n";

    // 2. A variable template can wrap a type trait.
    static_assert(isPointer<int*>);
    static_assert(!isPointer<int>);
    std::cout << "isPointer<int*> = " << std::boolalpha << isPointer<int*> << "\n";

    // 3. Non-type template parameters can produce named compile-time values.
    static_assert(squared<5> == 25);
    std::cout << "squared<5> = " << squared<5> << "\n";

    return 0;
}
