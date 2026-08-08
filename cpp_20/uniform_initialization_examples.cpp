// C++20 uniform initialization examples
// Reference: https://en.cppreference.com/w/cpp/language/aggregate_initialization
// Compile with: g++ -std=c++20 -O2 cpp_20/uniform_initialization_examples.cpp -o bin/cpp20_uniform_initialization && ./bin/cpp20_uniform_initialization

#include <iostream>
#include <string>

// Problem solved: initializes selected aggregate members by name and reduces positional mistakes.
// Before C++20: aggregate initialization required remembering member declaration order.
// Tips for uniform initialization:
// - Designated initializers make aggregate construction self-documenting.
// - C++ designators must follow the declaration order and cannot be mixed with positional members.
// - Designated initialization applies to aggregates, not classes with private members or constructors.
// - Use ordinary constructors when invariants require validation or controlled initialization.

struct Server {
    std::string host;
    int port;
    bool secure;
};

struct Window {
    int width;
    int height = 0;
    std::string title;
};

int main() {
    std::cout << "C++20 uniform initialization examples:\n";

    // 1. Designators document which aggregate member each value initializes.
    Server server{.host = "localhost", .port = 443, .secure = true};
    std::cout << "server = " << server.host << ':' << server.port
              << ", secure = " << std::boolalpha << server.secure << "\n";

    // 2. Members not mentioned by a designator are value-initialized.
    Window window{.width = 1280, .title = "C++20"};
    std::cout << "window = " << window.width << 'x' << window.height
              << " " << window.title << "\n";

    // 3. Designators must follow declaration order; this would be ill-formed:
    // Server invalid{.port = 443, .host = "localhost", .secure = true};

    return 0;
}
