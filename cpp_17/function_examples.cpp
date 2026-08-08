// C++17 std::function examples
// Reference: https://en.cppreference.com/w/cpp/utility/functional/function
// Compile with: g++ -std=c++17 -O2 cpp_17/function_examples.cpp -o bin/cpp17_function && ./bin/cpp17_function

#include <functional>
#include <iostream>
#include <typeinfo>

// Problem solved: supports runtime-polymorphic callbacks while allowing limited target inspection.
// Before C++17: callback code relied on fixed function pointers or custom type-erased wrappers.
// Tips for C++17 std::function usage:
// - Use target_type and target<T>() when a type-erased callback must be inspected carefully.
// - Treat target inspection as an exceptional escape hatch; normally invoke through the signature.
// - Use std::function for runtime polymorphism and std::invoke for uniform callable invocation.
// - Prefer direct callable objects when the concrete type is known and performance is critical.

int main() {
    std::cout << "C++17 std::function examples:\n";

    // 1. Store a lambda and inspect its erased target type.
    auto operation = [](int value) {
        return value * 2;
    };
    std::function<int(int)> callback = operation;
    std::cout << "callback result = " << callback(6) << "\n";
    std::cout << "target type available = " << std::boolalpha
              << (callback.target_type() == typeid(decltype(operation))) << "\n";

    // 2. target<T>() returns the stored object only when T matches exactly.
    if (const auto* stored = callback.target<decltype(operation)>()) {
        std::cout << "stored target result = " << (*stored)(7) << "\n";
    }

    // 3. A copy of std::function copies the callable's state.
    std::function<int(int)> copy = callback;
    std::cout << "copied callback result = " << copy(8) << "\n";

    return 0;
}
