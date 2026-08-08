// C++14 rvalue reference examples
// Reference: https://en.cppreference.com/w/cpp/language/reference
// Compile with: g++ -std=c++14 -O2 cpp_14/rvalue_reference_examples.cpp -o bin/cpp14_rvalue_reference && ./bin/cpp14_rvalue_reference

#include <iostream>
#include <string>
#include <utility>

// Problem solved: preserves an argument's lvalue or rvalue category through generic wrappers.
// Before C++14: wrappers needed separate overloads for lvalues and rvalues.
// Tips for rvalue references:
// - T&& in a deduced template parameter is a forwarding reference.
// - Use std::forward<T> to preserve the category when forwarding the parameter.
// - std::move unconditionally casts to an rvalue and should be used only for deliberate transfer.
// - Forwarding references are useful for factories, adapters, and generic constructors.

void receive(const std::string& value) {
    std::cout << "received lvalue: " << value << "\n";
}

void receive(std::string&& value) {
    std::cout << "received rvalue: " << value << "\n";
}

template <typename T>
void forwardValue(T&& value) {
    receive(std::forward<T>(value));
}

int main() {
    std::cout << "C++14 rvalue reference examples:\n";

    std::string name = "Ada";
    forwardValue(name);
    forwardValue(std::string("Grace"));

    std::string moved = "ownership";
    std::string destination = std::move(moved);
    std::cout << "destination = " << destination << "\n";
    std::cout << "source remains valid = " << std::boolalpha << moved.empty() << "\n";

    return 0;
}
