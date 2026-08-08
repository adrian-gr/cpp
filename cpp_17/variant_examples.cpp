// C++17 std::variant examples
// Reference: https://en.cppreference.com/w/cpp/utility/variant
// Compile with: g++ -std=c++17 -O2 cpp_17/variant_examples.cpp -o bin/cpp17_variant && ./bin/cpp17_variant

#include <iostream>
#include <string>
#include <variant>

// Problem solved: represents a closed set of alternative types with compiler-checked visitation.
// Before C++17: code used unions plus manual tags and unsafe casts.
// Tips for std::variant usage:
// - Use std::variant when the set of possible types is known at compile time.
// - Prefer variant over std::any when alternatives are finite and should be checked by the compiler.
// - Use std::visit to handle every active alternative without unsafe casts.
// - Use get_if when a conditional type-specific lookup is clearer than a visitor.
// - Keep alternatives meaningful and avoid using variant as a replacement for a class hierarchy.

template <typename... Visitors>
struct Overloaded : Visitors... {
    using Visitors::operator()...;
};

template <typename... Visitors>
Overloaded(Visitors...) -> Overloaded<Visitors...>;

using Value = std::variant<int, double, std::string>;

void printValue(const Value& value) {
    std::visit([](const auto& item) {
        std::cout << "visited value = " << item << "\n";
    }, value);
}

int main() {
    std::cout << "C++17 std::variant examples:\n";

    // 1. A variant stores one value from its declared alternatives.
    Value value = 42;
    std::cout << "holds int = " << std::boolalpha
              << std::holds_alternative<int>(value) << "\n";
    printValue(value);

    value = std::string("C++17");
    printValue(value);

    // 2. get_if returns a pointer when the requested alternative is active.
    if (const auto* text = std::get_if<std::string>(&value)) {
        std::cout << "string length = " << text->size() << "\n";
    }

    // 3. An overloaded visitor can provide type-specific behavior.
    Value numeric = 3.5;
    std::visit(Overloaded{
        [](int integer) {
            std::cout << "integer value = " << integer << "\n";
        },
        [](double decimal) {
            std::cout << "double value = " << decimal << "\n";
        },
        [](const std::string& text) {
            std::cout << "text value = " << text << "\n";
        }
    }, numeric);

    return 0;
}
