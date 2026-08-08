// C++11 uniform initialization examples
// Reference: https://en.cppreference.com/w/cpp/language/list_initialization
// Compile with: g++ -std=c++11 -O2 cpp_11/uniform_initialization_examples.cpp -o bin/cpp11_uniform_initialization && ./bin/cpp11_uniform_initialization

#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>

// Problem solved: provides one brace-based syntax for values, objects, and containers.
// Before C++11: code used mixed initialization syntax and constructor-specific conventions.
// Tips for uniform initialization:
// - Braces can prevent narrowing conversions and make initialization intent visible.
// - initializer_list constructors may be preferred over other overloads.
// - Use braces consistently, but understand which constructor overload they select.
// - Empty braces value-initialize objects, though overload behavior can still matter.

class Scores {
public:
    Scores(std::initializer_list<int> values) : values_(values) {}

    int total() const {
        int result = 0;
        for (int value : values_) {
            result += value;
        }
        return result;
    }

private:
    std::vector<int> values_;
};

struct Person {
    std::string name;
    int age;
};

int main() {
    std::cout << "C++11 uniform initialization examples:\n";

    // 1. Braces initialize built-in values, aggregates, and containers.
    int count{3};
    Person person{"Ada", 36};
    std::vector<int> values{1, 2, 3};
    std::cout << person.name << " has " << count << " items and total "
              << values[0] + values[1] + values[2] << "\n";

    // 2. Braces select an initializer_list constructor.
    Scores scores{10, 20, 30};
    std::cout << "scores total = " << scores.total() << "\n";

    // int narrowed{3.14}; // Compile-time error: narrowing is rejected.
    return 0;
}
