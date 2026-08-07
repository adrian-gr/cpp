// C++17 variadic template examples
// Compile with: g++ -std=c++17 -O2 cpp_17/variadic_templates_examples.cpp -o bin/cpp17_variadic && ./bin/cpp17_variadic

#include <iostream>
#include <string>

// Tips for C++17 variadic template usage:
// - Fold expressions replace much of the recursive boilerplate used in C++11.
// - Unary folds are useful when combining every argument with one operation.
// - Binary folds provide an initial value for empty packs and control the result type.
// - Choose a clear initial value so the accumulator has the intended type.

template <typename... Values>
auto sum(Values... values) {
    return (values + ...);
}

template <typename... Values>
bool allPositive(Values... values) {
    return ((values > 0) && ...);
}

template <typename... Values>
void printValues(const Values&... values) {
    ((std::cout << values << ' '), ...);
    std::cout << "\n";
}

int main() {
    std::cout << "C++17 variadic template examples:\n";
    std::cout << "sum(1, 2, 3, 4) = " << sum(1, 2, 3, 4) << "\n";
    std::cout << "allPositive(1, 2, 3) = " << std::boolalpha
              << allPositive(1, 2, 3) << "\n";
    printValues(std::string("fold"), 17, 2.5);
    return 0;
}
