// Core exception handling examples
// Compile with: g++ -std=c++11 -O2 cpp_core/exceptions_examples.cpp -o bin/core_exceptions && ./bin/core_exceptions

#include <iostream>
#include <stdexcept>
#include <string>

// Tips:
// - Throw exceptions for failures that the current function cannot reasonably handle.
// - Catch by const reference to preserve the dynamic exception type and avoid copying.
// - Prefer standard exception types or derive from std::exception.
// - Do not use exceptions for ordinary control flow.

int parsePositive(int value) {
    if (value <= 0) {
        throw std::invalid_argument("value must be positive");
    }
    return value;
}

int main() {
    std::cout << "Core exceptions examples:\n";

    try {
        std::cout << "valid value = " << parsePositive(5) << "\n";
        std::cout << "invalid value = " << parsePositive(-1) << "\n";
    } catch (const std::invalid_argument& error) {
        std::cout << "handled error: " << error.what() << "\n";
    }

    try {
        throw std::runtime_error("unexpected operation failure");
    } catch (const std::exception& error) {
        std::cout << "base exception handler: " << error.what() << "\n";
    }

    return 0;
}
