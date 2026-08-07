#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

int main() {
    std::cout << "C++17 Lambda Feature Examples\n";

    // 1. Lambda with structured bindings in parameters.
    auto printPair = [](const std::pair<int, int>& p) {
        auto [first, second] = p;
        std::cout << "pair: " << first << ", " << second << "\n";
    };
    printPair({10, 20});

    // 2. Capturing *this* by value in a lambda inside a member-like context.
    struct Counter {
        int value = 0;
        void increment() {
            auto captureThis = [*this]() mutable {
                value += 1;
                return value;
            };
            std::cout << "captureThis result = " << captureThis() << "\n";
            std::cout << "original value = " << value << "\n";
        }
    };
    Counter counter;
    counter.increment();

    // 3. Using lambda with std::invoke_result-like generic behavior via auto return type.
    auto compare = [](auto a, auto b) {
        return a < b;
    };
    std::cout << "compare(2, 3) = " << compare(2, 3) << "\n";
    std::cout << "compare('a', 'z') = " << compare('a', 'z') << "\n";

    // 4. Lambda in parallel-friendly algorithms with std::for_each and execution policy.
    std::vector<int> values = {1, 2, 3, 4, 5};
    std::cout << "values doubled:";
    std::for_each(values.begin(), values.end(), [](int& n) {
        n *= 2;
        std::cout << ' ' << n;
    });
    std::cout << "\n";

    // 5. Lambda with noexcept deduction and folded expression inside.
    auto allPositive = [](auto const& container) {
        return std::all_of(container.begin(), container.end(), [](auto n) {
            return n > 0;
        });
    };
    std::cout << "allPositive = " << allPositive(values) << "\n";

    // 6. constexpr lambda evaluated at compile time when used in a constexpr context.
    constexpr auto square = [](int x) {
        return x * x;
    };
    static_assert(square(4) == 16, "constexpr lambda works");
    std::cout << "square(5) = " << square(5) << "\n";

    return 0;
}
