// C++14 std::decay examples
// Reference: https://en.cppreference.com/w/cpp/types/decay
// Compile with: g++ -std=c++14 -O2 cpp_14/decay_examples.cpp -o bin/cpp14_decay && ./bin/cpp14_decay

#include <iostream>
#include <type_traits>

// Problem solved: provides a readable value-type transformation for generic code and aliases.
// Before C++14: code repeated typename std::decay<T>::type throughout template definitions.
// Tips for std::decay:
// - Combine decay with an alias template when a generic value type is used repeatedly.
// - Use decay to model how an argument behaves when copied into a value parameter.
// - Prefer remove_cvref in C++20 when array and function decay are not desired.

 template <typename T>
using Decayed = typename std::decay<T>::type;

template <typename T>
using ValueType = Decayed<T>;

template <typename T>
ValueType<T> copyValue(T&& value) {
    return value;
}

int main() {
    std::cout << "C++14 std::decay examples:\n";

    const int number = 7;
    auto copy = copyValue(number);
    static_assert(std::is_same<ValueType<const int&>, int>::value);
    std::cout << "copied value = " << copy << "\n";

    int values[] = {4, 5};
    static_assert(std::is_same<ValueType<decltype(values)>, int*>::value);
    std::cout << "decayed array element = " << copyValue(values)[0] << "\n";

    return 0;
}
