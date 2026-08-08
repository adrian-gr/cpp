// C++11 variadic template examples
// Reference: https://en.cppreference.com/w/cpp/language/parameter_pack
// Compile with: g++ -std=c++11 -O2 cpp_11/variadic_templates_examples.cpp -o bin/cpp11_variadic && ./bin/cpp11_variadic

#include <iostream>
#include <string>

// Problem solved: accepts an arbitrary number of strongly typed arguments at compile time.
// Before C++11: code used unsafe C varargs, overload duplication, or fixed-size interfaces.
// Tips for C++11 variadic template usage:
// - Use variadic templates when a function must accept an arbitrary number of typed arguments.
// - In C++11, recursive overloads with a base case are the usual way to process a parameter pack.
// - Prefer variadic templates over unsafe C-style variadic functions because types are checked.
// - Avoid deeply recursive or overly generic interfaces when a fixed number of arguments is clearer.

void printValues() {
    std::cout << "\n";
}

template <typename First, typename... Rest>
void printValues(const First& first, const Rest&... rest) {
    std::cout << first;
    if (sizeof...(rest) > 0) {
        std::cout << ' ';
    }
    printValues(rest...);
}

template <typename T>
T sum(const T& value) {
    return value;
}

template <typename T, typename... Rest>
T sum(const T& first, const Rest&... rest) {
    return first + sum(rest...);
}

int main() {
    std::cout << "C++11 variadic template examples:\n";
    printValues("values:", 1, 2.5, 'x');
    std::cout << "sum(1, 2, 3, 4) = " << sum(1, 2, 3, 4) << "\n";
    return 0;
}
