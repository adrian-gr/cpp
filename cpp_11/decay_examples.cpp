// C++11 std::decay examples
// Reference: https://en.cppreference.com/w/cpp/types/decay
// Compile with: g++ -std=c++11 -O2 cpp_11/decay_examples.cpp -o bin/cpp11_decay && ./bin/cpp11_decay

#include <iostream>
#include <type_traits>

// Problem solved: converts a deduced type into the value-like form used when passing by value.
// Before C++11: templates manually removed references, cv-qualifiers, arrays, and function types.
// Tips for std::decay:
// - decay removes references and top-level cv-qualifiers.
// - Arrays decay to pointers and function types decay to function pointers.
// - Use decay when storing a forwarding-reference argument by value.
// - Do not use decay when preserving reference semantics is important.

int function(int value) {
    return value * 2;
}

template <typename T>
void inspect(T&& value) {
    typedef typename std::decay<T>::type ValueType;
    std::cout << "decayed type is int = " << std::boolalpha
              << std::is_same<ValueType, int>::value
              << ", value = " << value << "\n";
}

int main() {
    std::cout << "C++11 std::decay examples:\n";

    const int number = 42;
    static_assert(std::is_same<std::decay<const int&>::type, int>::value,
                  "decay should produce int");
    inspect(number);

    int values[] = {1, 2, 3};
    static_assert(std::is_same<std::decay<decltype(values)>::type, int*>::value,
                  "arrays should decay to pointers");
    std::cout << "array decays to pointer = " << std::boolalpha
              << std::is_same<std::decay<decltype(values)>::type, int*>::value << "\n";

    static_assert(std::is_same<std::decay<decltype(function)>::type,
                               int (*)(int)>::value,
                  "functions should decay to function pointers");
    std::cout << "function result through pointer = " << std::decay<decltype(function)>::type{}(3) << "\n";

    return 0;
}
