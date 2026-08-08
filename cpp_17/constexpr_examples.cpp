#include <array>
// Reference: https://en.cppreference.com/w/cpp/language/constexpr
#include <iostream>
#include <type_traits>

// Problem solved: permits useful type-dependent and iterative computations at compile time.
// Before C++17: constexpr code had fewer control-flow and library capabilities.
// Tips for C++17 constexpr usage:
// - C++17 constexpr functions support richer control flow, including if statements and loops.
// - Use if constexpr when branches depend on types and one branch should be discarded at compile time.
// - Prefer constexpr algorithms for small deterministic operations that can run at compile time.
// - Use static_assert to test compile-time behavior directly.
// - Keep runtime-only work outside constexpr code so the function remains easy to evaluate.

template <typename T>
constexpr auto describeValue(const T& value) {
    if constexpr (std::is_integral<T>::value) {
        return value * 2;
    } else {
        return value + static_cast<T>(0.5);
    }
}

constexpr int sumArray(const int* values, int size) {
    int total = 0;
    for (int index = 0; index < size; ++index) {
        total += values[index];
    }
    return total;
}

constexpr std::array<int, 4> makeDoubledArray() {
    std::array<int, 4> result{{1, 2, 3, 4}};
    for (int& value : result) {
        value *= 2;
    }
    return result;
}

int main() {
    std::cout << "C++17 constexpr examples:\n";

    // 1. if constexpr selects a type-dependent branch at compile time.
    constexpr int doubled = describeValue(6);
    constexpr double adjusted = describeValue(2.0);
    static_assert(doubled == 12, "integral branch should be selected");
    static_assert(adjusted == 2.5, "floating-point branch should be selected");
    std::cout << "describeValue(6) = " << doubled << "\n";
    std::cout << "describeValue(2.0) = " << adjusted << "\n";

    // 2. C++17 allows constexpr evaluation of an array through a pointer.
    constexpr int values[] = {2, 4, 6, 8};
    constexpr int total = sumArray(values, 4);
    static_assert(total == 20, "sumArray should be evaluated at compile time");
    std::cout << "sumArray = " << total << "\n";

    // 3. Relaxed constexpr can mutate a local standard-library object during evaluation.
    constexpr auto doubledArray = makeDoubledArray();
    static_assert(doubledArray[3] == 8,
                  "array mutation should be evaluated at compile time");
    std::cout << "doubledArray[3] = " << doubledArray[3] << "\n";

    return 0;
}
