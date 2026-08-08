// C++20 noexcept examples
// Reference: https://en.cppreference.com/w/cpp/language/noexcept_spec
// Compile with: g++ -std=c++20 -O2 cpp_20/noexcept_examples.cpp -o bin/cpp20_noexcept && ./bin/cpp20_noexcept

#include <concepts>
#include <iostream>
#include <type_traits>

// Problem solved: combines non-throwing callable requirements with readable named constraints.
// Before C++20: code used nested enable_if and duplicated noexcept conditions.
// Tips for C++20 noexcept usage:
// - Combine concepts with noexcept when both type validity and exception safety matter.
// - Use a requires expression to constrain a callable to a non-throwing operation.
// - Keep noexcept conditions readable; name a concept when the requirement is reused.
// - A concept does not make an operation noexcept automatically; state the guarantee explicitly.

template <typename Callable>
concept NothrowCallable = requires(Callable callable) {
    requires std::is_nothrow_invocable_v<Callable>;
};

template <NothrowCallable Callable>
void execute(Callable callable) noexcept {
    callable();
}

template <typename T>
concept NothrowAddable = requires(T left, T right) {
    { left + right } noexcept -> std::convertible_to<T>;
};

template <NothrowAddable T>
constexpr T add(T left, T right) noexcept {
    return left + right;
}

int main() {
    std::cout << "C++20 noexcept examples:\n";

    // 1. A concept and noexcept can constrain and document a callable.
    auto safeAction = []() noexcept {
        std::cout << "safe action\n";
    };
    static_assert(NothrowCallable<decltype(safeAction)>);
    execute(safeAction);

    // 2. A compound requirement checks both the result type and noexcept status.
    constexpr int sum = add(2, 3);
    static_assert(sum == 5);
    static_assert(noexcept(add(2, 3)));
    std::cout << "add(2, 3) = " << sum << "\n";

    return 0;
}
