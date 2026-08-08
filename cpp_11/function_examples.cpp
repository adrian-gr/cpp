// C++11 std::function examples
// Reference: https://en.cppreference.com/w/cpp/utility/functional/function
// Compile with: g++ -std=c++11 -O2 cpp_11/function_examples.cpp -o bin/cpp11_function && ./bin/cpp11_function

#include <functional>
#include <iostream>
#include <string>

// Problem solved: gives callbacks one type-erased interface for many callable object types.
// Before C++11: code used function pointers, virtual callback classes, or unsafe C callbacks.
// Tips for std::function usage:
// - Use std::function when an interface accepts different callable types with one signature.
// - It can store free functions, function objects, lambdas, and bind expressions.
// - Use an exact function signature so incompatible callables are rejected at compile time.
// - Avoid std::function in hot paths when a template parameter can preserve the callable type.
// - Check whether a std::function is empty before invoking it, or it may throw std::bad_function_call.

int add(int left, int right) {
    return left + right;
}

struct Multiplier {
    int factor;

    int operator()(int value) const {
        return value * factor;
    }
};

int main() {
    std::cout << "C++11 std::function examples:\n";

    // 1. Store a free function behind a common callable signature.
    std::function<int(int, int)> operation = add;
    std::cout << "free function result = " << operation(2, 3) << "\n";

    // 2. Replace it with a lambda that has the same signature.
    operation = [](int left, int right) {
        return left * right;
    };
    std::cout << "lambda result = " << operation(2, 3) << "\n";

    // 3. Function objects can be stored in the same type-erased wrapper.
    std::function<int(int)> scale = Multiplier{4};
    std::cout << "function object result = " << scale(5) << "\n";

    // 4. Empty std::function objects can be detected before invocation.
    std::function<void(const std::string&)> printer;
    if (!printer) {
        std::cout << "printer is empty\n";
    }

    return 0;
}
