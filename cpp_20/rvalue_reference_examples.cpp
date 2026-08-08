// C++20 rvalue reference examples
// Reference: https://en.cppreference.com/w/cpp/language/reference
// Compile with: g++ -std=c++20 -O2 cpp_20/rvalue_reference_examples.cpp -o bin/cpp20_rvalue_reference && ./bin/cpp20_rvalue_reference

#include <concepts>
#include <iostream>
#include <string>
#include <utility>

// Problem solved: combines efficient forwarding with readable compile-time requirements.
// Before C++20: forwarding helpers used SFINAE or produced errors deep in instantiation.
// Tips for rvalue references:
// - Constrain forwarding functions with concepts that describe accepted source types.
// - Use std::forward to preserve lvalues and rvalues; use std::move for explicit transfer.
// - Return values by value when ownership should be transferred to the caller.
// - Do not forward a reference beyond the lifetime of the object it refers to.

template <typename T>
concept StringConvertible = std::convertible_to<T, std::string>;

template <StringConvertible T>
std::string makeOwned(T&& value) {
    return std::string(std::forward<T>(value));
}

template <typename T>
concept Movable = std::is_move_constructible_v<T>;

template <Movable T>
T moveValue(T&& value) {
    return std::move(value);
}

int main() {
    std::cout << "C++20 rvalue reference examples:\n";

    std::string source = "source";
    std::string copied = makeOwned(source);
    std::string moved = makeOwned(std::move(source));
    std::cout << "copied = " << copied << ", moved = " << moved << "\n";

    std::string original = "transfer";
    std::string transferred = moveValue(std::move(original));
    std::cout << "transferred = " << transferred << "\n";

    return 0;
}
