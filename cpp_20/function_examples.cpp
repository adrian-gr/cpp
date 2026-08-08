// C++20 std::function examples
// Reference: https://en.cppreference.com/w/cpp/utility/functional/function
// Compile with: g++ -std=c++20 -O2 cpp_20/function_examples.cpp -o bin/cpp20_function && ./bin/cpp20_function

#include <concepts>
#include <functional>
#include <iostream>
#include <string>

// Problem solved: validates callable interfaces before adapting them to runtime type erasure.
// Before C++20: callback constraints were expressed through SFINAE or discovered during errors.
// Tips for C++20 std::function usage:
// - Use concepts to constrain generic callable APIs before converting a callable to std::function.
// - Use std::function when runtime replacement or type erasure is required.
// - Keep a templated overload when callers benefit from avoiding type-erasure overhead.
// - Use std::move_only_function in C++23 when callbacks may be move-only; std::function requires copyability.

template <typename Callable>
concept StringConsumer = requires(Callable callable, const std::string& text) {
    { std::invoke(callable, text) } -> std::same_as<void>;
};

template <StringConsumer Callable>
void runCallback(Callable callable, const std::string& text) {
    std::function<void(const std::string&)> erased = std::move(callable);
    erased(text);
}

int main() {
    std::cout << "C++20 std::function examples:\n";

    // 1. Validate a callable with a concept before type erasure.
    runCallback([](const std::string& text) {
        std::cout << "callback received: " << text << "\n";
    }, "C++20");

    // 2. std::function can be replaced at runtime while keeping one interface.
    std::function<int(int)> transform = [](int value) {
        return value + 1;
    };
    std::cout << "first transform = " << transform(10) << "\n";
    transform = [](int value) {
        return value * value;
    };
    std::cout << "replacement transform = " << transform(10) << "\n";

    return 0;
}
