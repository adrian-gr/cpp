// C++17 rvalue reference examples
// Reference: https://en.cppreference.com/w/cpp/language/reference
// Compile with: g++ -std=c++17 -O2 cpp_17/rvalue_reference_examples.cpp -o bin/cpp17_rvalue_reference && ./bin/cpp17_rvalue_reference

#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

// Problem solved: makes value-category behavior observable and works with guaranteed copy elision.
// Before C++17: return-by-value could require an additional move or copy.
// Tips for rvalue references:
// - decltype((expression)) can reveal whether an expression is an lvalue or rvalue reference.
// - Return local values naturally; C++17 guarantees elision in key prvalue cases.
// - A named rvalue reference is still an lvalue and needs std::move to move again.
// - Keep moved-from objects valid and assignable, but do not assume their old value remains.

std::string makeText() {
    return std::string("constructed directly in the result");
}

template <typename T>
void reportCategory(T&& value) {
    using Parameter = decltype(value);
    std::cout << "parameter is rvalue reference = " << std::boolalpha
              << std::is_rvalue_reference<Parameter>::value << "\n";
}

int main() {
    std::cout << "C++17 rvalue reference examples:\n";

    std::string result = makeText();
    std::cout << "result = " << result << "\n";

    std::string value = "text";
    reportCategory(value);
    reportCategory(std::move(value));

    std::string moved = std::move(value);
    std::cout << "moved value = " << moved << "\n";

    return 0;
}
