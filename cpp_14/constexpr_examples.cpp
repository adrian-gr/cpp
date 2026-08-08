#include <iostream>

// Reference: https://en.cppreference.com/w/cpp/language/constexpr
// Problem solved: allows readable multi-step calculations to run during compilation.
// Before C++14: C++11 constexpr functions were restricted to a single return statement.
// Tips for C++14 constexpr usage:
// - C++14 allows constexpr functions to contain local variables and loops.
// - Prefer constexpr for deterministic calculations that benefit from compile-time evaluation.
// - Keep a constexpr function usable at runtime when that makes the API more flexible.
// - Use static_assert to verify results that must be known during compilation.
// - Avoid side effects and runtime-only operations in compile-time code paths.

constexpr int sumUpTo(int limit) {
    int total = 0;
    for (int value = 1; value <= limit; ++value) {
        total += value;
    }
    return total;
}

constexpr int absoluteValue(int value) {
    if (value < 0) {
        return -value;
    }
    return value;
}

constexpr int power(int base, int exponent) {
    int result = 1;
    for (int count = 0; count < exponent; ++count) {
        result *= base;
    }
    return result;
}

// Relaxed C++14 constexpr permits local variables and loops in one function body.
constexpr int greatestCommonDivisor(int first, int second) {
    while (second != 0) {
        int remainder = first % second;
        first = second;
        second = remainder;
    }
    return first;
}

int main() {
    std::cout << "C++14 constexpr examples:\n";

    // 1. A constexpr function with local variables and a loop.
    constexpr int total = sumUpTo(5);
    static_assert(total == 15, "sumUpTo should be evaluated at compile time");
    std::cout << "sumUpTo(5) = " << total << "\n";

    // 2. A constexpr function with conditional statements.
    constexpr int positiveValue = absoluteValue(-7);
    static_assert(positiveValue == 7,
                  "absoluteValue should be evaluated at compile time");
    std::cout << "absoluteValue(-7) = " << positiveValue << "\n";

    // 3. Multiple constexpr calculations can be composed.
    constexpr int result = power(2, 5) + sumUpTo(3);
    static_assert(result == 38, "composed constexpr calculation is incorrect");
    std::cout << "power(2, 5) + sumUpTo(3) = " << result << "\n";

    // 4. Relaxed constexpr supports mutable local state and a loop.
    constexpr int divisor = greatestCommonDivisor(48, 18);
    static_assert(divisor == 6, "gcd should be evaluated at compile time");
    std::cout << "greatestCommonDivisor(48, 18) = " << divisor << "\n";

    return 0;
}
