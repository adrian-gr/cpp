// C++20 three-way comparison examples
// Reference: https://en.cppreference.com/w/cpp/language/operator_comparison
// Compile with: g++ -std=c++20 -O2 -Wall -Wextra cpp_20/three_way_comparison_examples.cpp -o bin/cpp20_three_way && ./bin/cpp20_three_way

#include <compare>
#include <cmath>
#include <iostream>

// Problem solved: defines ordering once and generates consistent equality and relational operators.
// Before C++20: types needed several comparison overloads with duplicated ordering logic.
// Tips for C++20 three-way comparison usage:
// - Use <=> to express ordering once and let the compiler generate == and relational operators.
// - A defaulted operator<=> compares members in declaration order.
// - strong_ordering means every pair has a definite less, equal, or greater result.
// - partial_ordering can represent unordered values, such as comparisons involving NaN.
// - Use the comparison category that matches the semantics of the type; do not force unordered values into a total order.

struct Version {
    int major;
    int minor;

    auto operator<=>(const Version&) const = default;
};

int main() {
    std::cout << "C++20 three-way comparison examples:\n";

    // 1. A defaulted spaceship operator compares members lexicographically.
    Version current{2, 4};
    Version older{2, 1};
    auto versionResult = current <=> older;
    static_assert(std::is_same<decltype(versionResult), std::strong_ordering>::value,
                  "integer members should produce strong ordering");
    std::cout << "current > older = " << std::boolalpha
              << (current > older) << "\n";

    // 2. The result can be compared with a named ordering category.
    if (versionResult == std::strong_ordering::greater) {
        std::cout << "current version is greater\n";
    }

    // 3. Floating-point comparison can be unordered when NaN is involved.
    double value = 1.0;
    double notANumber = std::nan("");
    auto floatingResult = value <=> notANumber;
    static_assert(std::is_same<decltype(floatingResult), std::partial_ordering>::value,
                  "floating-point comparison should produce partial ordering");
    std::cout << "1.0 <=> NaN is unordered = "
              << (floatingResult == std::partial_ordering::unordered) << "\n";

    return 0;
}
