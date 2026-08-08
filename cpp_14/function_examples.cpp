// C++14 std::function examples
// Reference: https://en.cppreference.com/w/cpp/utility/functional/function
// Compile with: g++ -std=c++14 -O2 cpp_14/function_examples.cpp -o bin/cpp14_function && ./bin/cpp14_function

#include <functional>
#include <iostream>
#include <string>

// Problem solved: adapts stateful and generic callables to a runtime callback signature.
// Before C++14: callback APIs commonly required fixed functor classes or separate overloads.
// Tips for C++14 std::function usage:
// - Use std::function to expose a stable callback signature at an API boundary.
// - Generic lambdas can be adapted to a concrete std::function signature when stored.
// - Capture state by value for independent callback state and by reference only when its lifetime is safe.
// - Prefer a template callback parameter when type erasure and runtime replacement are unnecessary.

int main() {
    std::cout << "C++14 std::function examples:\n";

    // 1. A generic lambda becomes a concrete callable when assigned to std::function.
    auto genericAdd = [](auto left, auto right) {
        return left + right;
    };
    std::function<int(int, int)> integerAdd = genericAdd;
    std::cout << "generic lambda as int callback = " << integerAdd(4, 5) << "\n";

    // 2. A callback can capture configuration by value.
    int offset = 10;
    std::function<int(int)> addOffset = [offset](int value) {
        return value + offset;
    };
    offset = 100;
    std::cout << "captured offset result = " << addOffset(5) << "\n";

    // 3. std::function supports recursive callbacks after initialization.
    std::function<int(int)> factorial;
    factorial = [&factorial](int value) {
        return value <= 1 ? 1 : value * factorial(value - 1);
    };
    std::cout << "recursive callback factorial(5) = " << factorial(5) << "\n";

    return 0;
}
