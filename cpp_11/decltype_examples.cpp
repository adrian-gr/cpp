// C++11 decltype examples
// Compile with: g++ -std=c++11 -O2 cpp_11/decltype_examples.cpp -o bin/cpp11_decltype && ./bin/cpp11_decltype

#include <iostream>
#include <type_traits>
#include <utility>

// Tips for decltype usage:
// - Use decltype when a declaration should preserve the exact type of an expression.
// - Remember that decltype(variable) gives the declared type, while decltype((variable))
//   also considers whether the expression is an lvalue.
// - Use decltype in generic code when return types depend on function arguments.
// - Prefer auto for straightforward value deduction and use decltype when exact type
//   and reference or const qualifiers need to be preserved.

int add(int left, int right) {
    return left + right;
}

template <typename Left, typename Right>
auto multiply(const Left& left, const Right& right)
    -> decltype(left * right) {
    return left * right;
}

template <typename Container>
auto firstElement(Container& container)
    -> decltype(container[0]) {
    return container[0];
}

int main() {
    std::cout << "C++11 decltype examples:\n";

    // 1. Deduce the exact declared type of a variable.
    int number = 42;
    decltype(number) anotherNumber = 7;
    static_assert(std::is_same<decltype(anotherNumber), int>::value,
                  "decltype should preserve the declared type");
    std::cout << "anotherNumber = " << anotherNumber << "\n";

    // 2. decltype preserves references for parenthesized lvalue expressions.
    decltype((number)) reference = number;
    reference = 100;
    static_assert(std::is_same<decltype(reference), int&>::value,
                  "parenthesized lvalue should produce a reference type");
    std::cout << "number after reference update = " << number << "\n";

    // 3. Use decltype with an existing function call.
    decltype(add(1, 2)) sum = add(10, 20);
    static_assert(std::is_same<decltype(sum), int>::value,
                  "decltype should deduce the function return type");
    std::cout << "sum = " << sum << "\n";

    // 4. Trailing return type enables generic functions based on an expression.
    std::cout << "multiply(2, 2.5) = " << multiply(2, 2.5) << "\n";

    // 5. Use decltype to preserve an element reference in generic code.
    int values[] = {10, 20, 30};
    firstElement(values) = 99;
    std::cout << "first value = " << values[0] << "\n";

    return 0;
}
