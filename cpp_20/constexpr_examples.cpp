#include <iostream>
// Reference: https://en.cppreference.com/w/cpp/language/constexpr
#include <type_traits>
#include <vector>

// Problem solved: distinguishes functions that may run at compile time from those required to do so.
// Before C++20: constexpr could not enforce compile-time calls as directly as consteval does.
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

constexpr int countPositive(const std::vector<int>& values) {
    int count = 0;
    for (int value : values) {
        if (value > 0) {
            ++count;
        }
    }
    return count;
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

    // 4. C++20 permits constexpr evaluation of vector operations and dynamic storage.
    constexpr int positiveCount = countPositive(std::vector<int>{-1, 2, 3, -4});
    static_assert(positiveCount == 2,
                  "vector processing should be evaluated at compile time");
    std::cout << "positive count = " << positiveCount << "\n";

    return 0;
}
