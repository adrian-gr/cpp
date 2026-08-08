#include <algorithm>
// Reference: https://en.cppreference.com/w/cpp/language/lambda
#include <iostream>
#include <string>
#include <vector>

// Problem solved: makes local generic and stateful operations concise and reusable.
// Before C++14: generic behavior required function templates or manually typed functors.

int main() {
    std::cout << "C++14 Lambda Feature Examples\n";

    // 1. Generic lambda (auto parameters).
    auto genericAdd = [](auto a, auto b) {
        return a + b;
    };
    std::cout << "genericAdd(3, 5) = " << genericAdd(3, 5) << "\n";
    std::cout << "genericAdd(2.5, 4.5) = " << genericAdd(2.5, 4.5) << "\n";

    // 2. Lambda with init-capture and move capture.
    std::string prefix = "Value:";
    auto moveCapture = [prefix = std::move(prefix)](int value) {
        return prefix + " " + std::to_string(value);
    };
    std::cout << moveCapture(42) << "\n";

    // 3. Generic lambda used in an STL algorithm.
    std::vector<int> numbers = {1, 2, 3, 4, 5};
    std::cout << "squares:";
    std::for_each(numbers.begin(), numbers.end(), [](auto n) {
        std::cout << ' ' << (n * n);
    });
    std::cout << "\n";

    // 4. Folded lambda-like behavior via generic lambdas and initializer capture.
    auto join = [separator = std::string(", ")] (auto const& items) {
        std::string result;
        bool first = true;
        for (auto const& item : items) {
            if (!first) {
                result += separator;
            }
            first = false;
            result += item;
        }
        return result;
    };
    std::cout << "joined = " << join(std::vector<std::string>{"a", "b", "c"}) << "\n";

    return 0;
}
