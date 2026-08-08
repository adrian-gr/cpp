// C++20 std::decay examples
// Reference: https://en.cppreference.com/w/cpp/types/decay
// Compile with: g++ -std=c++20 -O2 cpp_20/decay_examples.cpp -o bin/cpp20_decay && ./bin/cpp20_decay

#include <iostream>
#include <type_traits>
#include <utility>

// Problem solved: lets modern forwarding code choose between decay and preservation deliberately.
// Before C++20: code often used decay even when it only needed to remove references and cv-qualifiers.
// Tips for std::decay:
// - Use decay_t when you want parameter-by-value semantics, including array/function decay.
// - Use remove_cvref_t instead when arrays and functions should retain their original kinds.
// - Apply the transformation at ownership boundaries, not indiscriminately in generic code.
// - Verify transformations with is_same_v when a type distinction affects behavior.

template <typename T>
using StoredType = std::decay_t<T>;

template <typename T>
using PreservedType = std::remove_cvref_t<T>;

int main() {
    std::cout << "C++20 std::decay examples:\n";

    int values[] = {1, 2, 3};
    static_assert(std::is_same_v<StoredType<decltype(values)>, int*>);
    static_assert(std::is_same_v<PreservedType<decltype(values)>, int[3]>);
    std::cout << "decayed array is pointer = "
              << std::boolalpha
              << std::is_same_v<StoredType<decltype(values)>, int*> << "\n";

    auto operation = [](int value) {
        return value * 2;
    };
    static_assert(std::is_same_v<StoredType<decltype(operation)>, decltype(operation)>);
    std::cout << "stored operation result = " << operation(6) << "\n";

    return 0;
}
