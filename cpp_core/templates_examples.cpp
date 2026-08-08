// Core templates and generic programming examples
// Reference: https://en.cppreference.com/w/cpp/language/templates
// Compile with: g++ -std=c++11 -O2 cpp_core/templates_examples.cpp -o bin/core_templates && ./bin/core_templates

#include <iostream>
#include <string>

// Problem solved: writes reusable algorithms and types without duplicating code for each data type.
// Before templates: code repeated type-specific functions or used unsafe void-pointer interfaces.
// Tips for templates:
// - Keep template requirements simple and make them visible through the operations used.
// - Prefer templates when behavior is identical across types and varies only by type.
// - Use meaningful template names and small examples before introducing advanced metaprogramming.
// - Watch compile-time cost and error readability in heavily nested template code.

// Function templates work with any type that supports the required operation.
template <typename T>
T maximum(T left, T right) {
    return left < right ? right : left;
}

// Class templates package an operation with a reusable type.
template <typename T>
class Box {
public:
    explicit Box(T value) : value_(value) {}

    const T& value() const {
        return value_;
    }

private:
    T value_;
};

int main() {
    std::cout << "Core templates examples:\n";
    std::cout << "maximum = " << maximum(3, 7) << "\n";
    std::cout << "maximum text = " << maximum(std::string("alpha"), std::string("beta")) << "\n";

    Box<int> integerBox(42);
    Box<std::string> textBox("generic box");
    std::cout << "integer box = " << integerBox.value() << "\n";
    std::cout << "text box = " << textBox.value() << "\n";

    return 0;
}
