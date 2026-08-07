// C++20 concepts examples
// Compile with: g++ -std=c++20 -O2 cpp_20/concepts_examples.cpp -o bin/cpp20_concepts && ./bin/cpp20_concepts

#include <concepts>
#include <iostream>
#include <string>
#include <type_traits>

// Tips for C++20 concepts usage:
// - Use concepts to express template requirements in readable, reusable names.
// - Prefer a named concept when a requirement appears in multiple declarations.
// - Use requires expressions to check syntax, types, and return-type constraints.
// - Constrain templates at the interface so invalid calls produce useful diagnostics.
// - Use a requires clause when a constraint is clearer after the template parameter list.
// - Avoid overly broad concepts; model the operations the algorithm actually needs.

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename T>
concept Printable = requires(const T& value) {
    { std::cout << value } -> std::same_as<std::ostream&>;
};

template <typename T>
concept Incrementable = requires(T value) {
    ++value;
};

template <typename T>
concept Addable = requires(T left, T right) {
    { left + right } -> std::same_as<T>;
};

template <Numeric T>
constexpr T square(T value) {
    return value * value;
}

template <Printable T>
void printValue(const T& value) {
    std::cout << "value: " << value << "\n";
}

void increment(Incrementable auto& value) {
    ++value;
}

template <typename T>
requires Addable<T>
constexpr T add(T left, T right) {
    return left + right;
}

int main() {
    std::cout << "C++20 concepts examples:\n";

    // 1. A named concept constrains a function to arithmetic types.
    constexpr int integerSquare = square(5);
    constexpr double floatingSquare = square(2.5);
    static_assert(integerSquare == 25);
    static_assert(floatingSquare == 6.25);
    std::cout << "square(5) = " << integerSquare << "\n";
    std::cout << "square(2.5) = " << floatingSquare << "\n";

    // 2. A compound requirement checks that a value can be streamed and
    // that the stream operation returns std::ostream&.
    printValue(std::string("C++20"));
    printValue(42);

    // 3. Abbreviated function templates use a concept directly in a parameter.
    int count = 3;
    increment(count);
    std::cout << "incremented count = " << count << "\n";

    // 4. A requires clause constrains a template after its parameter list.
    constexpr int sum = add(20, 22);
    static_assert(Addable<int>);
    static_assert(sum == 42);
    std::cout << "add(20, 22) = " << sum << "\n";

    return 0;
}
