#include <concepts>
// Reference: https://en.cppreference.com/w/cpp/language/lambda
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

// Problem solved: gives lambdas explicit template parameters and constraints at the call site.
// Before C++20: generic lambdas relied on abbreviated auto parameters and separate constraints.

int main() {
    std::cout << "C++20 Lambda Feature Examples\n";

    // 1. Explicit template parameter list on a lambda.
    auto add = []<typename T>(T a, T b) {
        return a + b;
    };
    std::cout << "add(2, 3) = " << add(2, 3) << "\n";
    std::cout << "add(2.5, 4.5) = " << add(2.5, 4.5) << "\n";

    // 2. Constrained lambda using a requires clause.
    auto isEven = []<typename T>(T value) requires(std::integral<T>) {
        return value % 2 == 0;
    };
    std::cout << "isEven(4) = " << isEven(4) << "\n";

    // 3. Lambda with explicit non-type template parameter.
    auto multiplyBy = []<int Factor>(int value) {
        return value * Factor;
    };
    std::cout << "multiplyBy<3>(5) = " << multiplyBy.template operator()<3>(5) << "\n";

    // 4. Using lambda in a C++20 ranges pipeline.
    std::vector<int> values = {1, 2, 3, 4, 5, 6};
    auto evenValues = values | std::views::filter([](int n) {
        return n % 2 == 0;
    });
    std::cout << "even values:";
    for (int n : evenValues) {
        std::cout << ' ' << n;
    }
    std::cout << "\n";

    return 0;
}
