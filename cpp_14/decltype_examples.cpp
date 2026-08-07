// C++14 decltype examples
// Compile with: g++ -std=c++14 -O2 cpp_14/decltype_examples.cpp -o bin/cpp14_decltype && ./bin/cpp14_decltype

#include <iostream>
#include <type_traits>

// Tips for C++14 decltype usage:
// - Use decltype(auto) when a function should preserve the exact type of its return expression.
// - decltype(auto) can preserve references, unlike plain auto return type deduction.
// - Use std::is_same to make reference and const qualification behavior explicit.
// - Prefer a plain auto return type when returning a value intentionally.

int globalValue = 10;

decltype(auto) valueReference() {
    return (globalValue);
}

auto valueCopy() {
    return globalValue;
}

template <typename Left, typename Right>
decltype(auto) selectLarger(Left& left, Right& right) {
    return left > right ? left : right;
}

int main() {
    std::cout << "C++14 decltype examples:\n";

    // 1. decltype(auto) preserves the reference returned by the expression.
    decltype(auto) reference = valueReference();
    reference = 25;
    static_assert(std::is_same<decltype(reference), int&>::value,
                  "decltype(auto) should preserve the reference");
    std::cout << "globalValue after reference update = " << globalValue << "\n";

    // 2. Plain auto intentionally returns a copy.
    auto copy = valueCopy();
    copy = 50;
    static_assert(std::is_same<decltype(copy), int>::value,
                  "plain auto should deduce a value");
    std::cout << "globalValue after copy update = " << globalValue << "\n";

    // 3. decltype(auto) preserves the conditional expression's reference type.
    int first = 7;
    int second = 12;
    decltype(auto) larger = selectLarger(first, second);
    larger = 20;
    std::cout << "second after larger update = " << second << "\n";

    return 0;
}
