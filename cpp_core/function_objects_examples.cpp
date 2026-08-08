// Core function pointers, functors, and bind examples
// Compile with: g++ -std=c++11 -O2 cpp_core/function_objects_examples.cpp -o bin/core_function_objects && ./bin/core_function_objects

#include <functional>
#include <iostream>

// Tips:
// - Use a function object when behavior needs state or a custom call operator.
// - Use lambdas for short local behavior and named functors for reusable policy objects.
// - std::function is useful for type-erased callbacks but may add runtime overhead.
// - Ensure bind placeholders and captured references have safe lifetimes.

int add(int left, int right) { return left + right; }

struct Multiplier {
    int factor;
    int operator()(int value) const { return factor * value; }
};

int main() {
    std::cout << "Core callable examples:\n";
    int (*functionPointer)(int, int) = add;
    std::function<int(int)> twice = Multiplier{2};
    auto addTen = std::bind(add, std::placeholders::_1, 10);
    std::cout << "function pointer = " << functionPointer(2, 3) << "\n";
    std::cout << "functor = " << twice(5) << "\n";
    std::cout << "bind result = " << addTen(7) << "\n";
    return 0;
}
