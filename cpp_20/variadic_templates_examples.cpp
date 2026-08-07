// C++20 variadic template examples
// Compile with: g++ -std=c++20 -O2 cpp_20/variadic_templates_examples.cpp -o bin/cpp20_variadic && ./bin/cpp20_variadic

#include <concepts>
#include <iostream>

// Tips for C++20 variadic template usage:
// - Constrain each pack element when the algorithm requires a specific operation.
// - A requires expression can validate an entire parameter pack with a fold.
// - Prefer named concepts when pack constraints are reused across several functions.
// - Keep diagnostics readable by separating complex pack constraints into concepts.

template <typename... Values>
concept AllIntegral = (std::integral<Values> && ...);

template <AllIntegral... Values>
constexpr auto sum(Values... values) {
    return (values + ...);
}

template <typename... Values>
concept AllStreamable = requires(Values... values) {
    (std::cout << ... << values);
};

template <AllStreamable... Values>
void printValues(const Values&... values) {
    ((std::cout << values << ' '), ...);
    std::cout << "\n";
}

int main() {
    std::cout << "C++20 variadic template examples:\n";

    constexpr int result = sum(1, 2, 3, 4);
    static_assert(result == 10);
    std::cout << "constrained sum = " << result << "\n";
    printValues("values:", 20, 2.5);

    return 0;
}
