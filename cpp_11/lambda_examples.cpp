#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::cout << "C++11 Lambda Examples\n";

    // 1. Basic lambda that captures nothing and returns a value.
    auto add = [](int a, int b) {
        return a + b;
    };
    std::cout << "add(3, 5) = " << add(3, 5) << "\n";

    // 2. Lambda with capture-by-value.
    int x = 10;
    auto offsetValue = [x](int value) {
        return value + x;
    };
    std::cout << "offsetValue(7) = " << offsetValue(7) << "\n";

    // 3. Lambda with capture-by-reference.
    int total = 0;
    auto accumulate = [&total](int value) {
        total += value;
    };
    accumulate(4);
    accumulate(6);
    std::cout << "total after accumulate = " << total << "\n";

    // 4. Using lambda in an STL algorithm.
    std::vector<int> values = {1, 2, 3, 4, 5};
    std::cout << "values greater than 3:";
    std::for_each(values.begin(), values.end(), [](int n) {
        if (n > 3) {
            std::cout << ' ' << n;
        }
    });
    std::cout << "\n";

    // 5. Mutable lambda to modify captured value.
    int count = 0;
    auto mutableCounter = [count]() mutable {
        count += 1;
        return count;
    };
    std::cout << "mutableCounter() = " << mutableCounter() << "\n";
    std::cout << "mutableCounter() again = " << mutableCounter() << "\n";

    // 6. Lambda returning another lambda.
    auto makeMultiplier = [](int factor) {
        return [factor](int value) {
            return value * factor;
        };
    };
    auto triple = makeMultiplier(3);
    std::cout << "triple(7) = " << triple(7) << "\n";

    // 7. Lambda assigned to std::function for type erasure.
    std::function<void(const std::string&)> printer = [](const std::string& text) {
        std::cout << "Printer: " << text << "\n";
    };
    printer("Hello from lambda");

    return 0;
}
