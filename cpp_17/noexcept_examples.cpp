// C++17 noexcept examples
// Compile with: g++ -std=c++17 -O2 cpp_17/noexcept_examples.cpp -o bin/cpp17_noexcept && ./bin/cpp17_noexcept

#include <iostream>
#include <type_traits>
#include <utility>

// Tips for C++17 noexcept usage:
// - Since C++17, noexcept is part of a function type and can affect overloads and pointers.
// - Use std::is_nothrow_invocable to inspect whether a callable can be invoked safely.
// - Keep noexcept guarantees accurate because violating them calls std::terminate.
// - Prefer compile-time traits over manually duplicating exception conditions.

void safeOperation() noexcept {}
void riskyOperation() {}

void call(void (*operation)() noexcept) {
    std::cout << "calling noexcept function\n";
    operation();
}

template <typename Callable>
void invokeIfSafe(Callable callable) {
    static_assert(std::is_nothrow_invocable<Callable>::value,
                  "invokeIfSafe requires a non-throwing callable");
    callable();
}

int main() {
    std::cout << "C++17 noexcept examples:\n";

    // 1. noexcept participates in function pointer types in C++17.
    call(safeOperation);

    // 2. Traits can inspect the exception guarantee of a callable.
    auto safeLambda = []() noexcept {
        std::cout << "safe lambda\n";
    };
    static_assert(std::is_nothrow_invocable<decltype(safeLambda)>::value,
                  "safe lambda should be non-throwing");
    invokeIfSafe(safeLambda);

    // A regular lambda is not accepted by invokeIfSafe because it may throw.
    std::cout << "risky operation is not marked noexcept: "
              << std::boolalpha << noexcept(riskyOperation()) << "\n";

    return 0;
}
