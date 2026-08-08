// C++11 rvalue reference examples
// Reference: https://en.cppreference.com/w/cpp/language/reference
// Compile with: g++ -std=c++11 -O2 cpp_11/rvalue_reference_examples.cpp -o bin/cpp11_rvalue_reference && ./bin/cpp11_rvalue_reference

#include <iostream>
#include <string>
#include <utility>

// Problem solved: lets functions distinguish temporary values from persistent objects.
// Before C++11: APIs could not express rvalue binding and often copied every argument.
// Tips for rvalue references:
// - T&& can bind to a temporary, while T& normally binds to an lvalue.
// - A named rvalue-reference variable is an lvalue inside the function body.
// - Use std::move when deliberately treating a named object as movable.
// - Binding does not automatically move an object; the receiving operation must move from it.

void inspect(const std::string& value) {
    std::cout << "const lvalue reference: " << value << "\n";
}

void inspect(std::string&& value) {
    std::cout << "rvalue reference: " << value << "\n";
}

void takeRvalue(std::string&& value) {
    std::cout << "named rvalue reference is an lvalue: " << value << "\n";
    inspect(std::move(value));
}

int main() {
    std::cout << "C++11 rvalue reference examples:\n";

    std::string name = "Ada";
    inspect(name);
    inspect(std::string("temporary"));

    std::string value = "movable";
    takeRvalue(std::move(value));

    return 0;
}
