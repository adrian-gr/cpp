#include <any>
// Reference: https://en.cppreference.com/w/cpp/utility/any
#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>

// Problem solved: stores one value of an otherwise unknown type behind a type-safe interface.
// Before C++17: code used void pointers, tagged unions, or unsafe manual type-erasure utilities.
// Tips for std::any usage:
// - Use std::any when you need a type-safe container for heterogeneous values
//   and the exact types are not known at compile time.
// - Prefer std::variant when the set of possible types is known and limited,
//   because variant provides stronger type safety and better performance.
// - Avoid std::any for frequent or performance-critical code paths: it uses type
//   erasure and may allocate dynamically.
// - Use std::any_cast carefully and check type() or cast to a pointer form
//   before dereferencing to avoid std::bad_any_cast exceptions.
// - std::any is useful for plugin APIs, configuration values, or untyped data
//   containers, but not as a replacement for regular typed interfaces.

int main() {
    std::cout << "C++17 std::any Examples\n";

    // 1. Storing different types in std::any.
    std::any value = 42;
    std::cout << "value contains int: " << std::any_cast<int>(value) << "\n";

    value = std::string("hello");
    std::cout << "value contains string: " << std::any_cast<std::string>(value) << "\n";

    // 2. Checking the stored type before casting.
    if (value.type() == typeid(std::string)) {
        std::cout << "type is std::string\n";
    }

    // 3. Safe cast with std::any_cast returning nullptr for wrong type.
    if (const std::string* text = std::any_cast<std::string>(&value)) {
        std::cout << "safe cast string: " << *text << "\n";
    }

    // 4. Storing a custom type.
    struct Point { int x, y; };
    value = Point{3, 4};

    try {
        Point p = std::any_cast<Point>(value);
        std::cout << "Point = (" << p.x << ", " << p.y << ")\n";
    } catch (const std::bad_any_cast& e) {
        std::cout << "bad_any_cast: " << e.what() << "\n";
    }

    // 5. Using std::any in a heterogeneous container.
    std::vector<std::any> items;
    items.emplace_back(2026);
    items.emplace_back(3.14);
    items.emplace_back(std::string("C++17"));

    for (const auto& item : items) {
        std::cout << "item type = " << item.type().name();
        if (item.type() == typeid(int)) {
            std::cout << ", int value = " << std::any_cast<int>(item);
        } else if (item.type() == typeid(double)) {
            std::cout << ", double value = " << std::any_cast<double>(item);
        } else if (item.type() == typeid(std::string)) {
            std::cout << ", string value = " << std::any_cast<std::string>(item);
        }
        std::cout << "\n";
    }

    return 0;
}
