#include <iostream>
#include <type_traits>

// Tips for C++20 constexpr usage:
// - Use consteval when a function must always be evaluated at compile time.
// - Use std::is_constant_evaluated when compile-time and runtime paths need different behavior.
// - C++20 permits more standard library operations in constant expressions.
// - Prefer constexpr for flexible APIs and consteval only for compile-time-only contracts.
// - Avoid making a function consteval when callers legitimately need runtime evaluation.

consteval int compileTimeSquare(int value) {
    return value * value;
}

constexpr int chooseValue() {
    if (std::is_constant_evaluated()) {
        return 100;
    }
    return 200;
}

constexpr int factorial(int value) {
    int result = 1;
    for (int current = 2; current <= value; ++current) {
        result *= current;
    }
    return result;
}

int main() {
    std::cout << "C++20 constexpr examples:\n";

    // 1. consteval guarantees compile-time evaluation.
    constexpr int square = compileTimeSquare(6);
    static_assert(square == 36, "consteval function should run at compile time");
    std::cout << "compileTimeSquare(6) = " << square << "\n";

    // 2. A constexpr function can detect constant evaluation.
    constexpr int compileTimeValue = chooseValue();
    static_assert(compileTimeValue == 100,
                  "constant-evaluation branch should be selected");
    std::cout << "compile-time chooseValue() = " << compileTimeValue << "\n";
    std::cout << "runtime chooseValue() = " << chooseValue() << "\n";

    // 3. C++20 constexpr functions support ordinary local state and loops.
    constexpr int factorialResult = factorial(5);
    static_assert(factorialResult == 120, "factorial should be evaluated at compile time");
    std::cout << "factorial(5) = " << factorialResult << "\n";

    return 0;
}
