// C++14 uniform initialization examples
// Reference: https://en.cppreference.com/w/cpp/language/list_initialization
// Compile with: g++ -std=c++14 -O2 cpp_14/uniform_initialization_examples.cpp -o bin/cpp14_uniform_initialization && ./bin/cpp14_uniform_initialization

#include <iostream>
#include <map>
#include <string>
#include <vector>

// Problem solved: keeps nested objects and standard containers easy to initialize consistently.
// Before C++14: nested initialization often needed temporary objects or verbose insert calls.
// Tips for uniform initialization:
// - Use nested braces to construct container elements and aggregate members directly.
// - Brace initialization works well with return-by-value factory functions.
// - Watch for initializer_list overloads when a type has competing constructors.
// - Prefer meaningful aggregate members over deeply nested anonymous initializer lists.

struct Configuration {
    std::string name;
    std::vector<int> ports;
};

Configuration makeConfiguration() {
    return {"development", {8080, 8081}};
}

int main() {
    std::cout << "C++14 uniform initialization examples:\n";

    // 1. Nested braces initialize an aggregate and its vector member.
    Configuration configuration{"production", {443, 8443}};
    std::cout << configuration.name << " ports = "
              << configuration.ports[0] << ", " << configuration.ports[1] << "\n";

    // 2. A factory can return an aggregate using the same syntax.
    Configuration generated = makeConfiguration();
    std::cout << "generated config = " << generated.name << "\n";

    // 3. Map entries can be initialized directly as pairs.
    std::map<std::string, int> priorities{{"low", 1}, {"high", 10}};
    std::cout << "high priority = " << priorities.at("high") << "\n";

    return 0;
}
