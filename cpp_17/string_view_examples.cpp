// C++17 std::string_view examples
// Reference: https://en.cppreference.com/w/cpp/string/basic_string_view
// Compile with: g++ -std=c++17 -O2 cpp_17/string_view_examples.cpp -o bin/cpp17_string_view && ./bin/cpp17_string_view

#include <iostream>
#include <string>
#include <string_view>

// Problem solved: lets functions inspect strings without copying or allocating.
// Before C++17: APIs used const std::string&, const char*, or temporary string copies.
// Tips for std::string_view usage:
// - Use string_view for non-owning read-only text parameters.
// - The referenced characters must outlive the view; string_view does not own storage.
// - Do not return a view to a local string or store a view after its source changes.
// - Use std::string when ownership or a null-terminated result is required.

std::size_t countWords(std::string_view text) {
    std::size_t count = 0;
    bool insideWord = false;
    for (char character : text) {
        if (character == ' ') {
            insideWord = false;
        } else if (!insideWord) {
            insideWord = true;
            ++count;
        }
    }
    return count;
}

void printView(std::string_view text) {
    std::cout << "view = " << text << ", size = " << text.size() << "\n";
}

int main() {
    std::cout << "C++17 std::string_view examples:\n";

    // 1. A view can refer to a string literal without allocating.
    std::string_view literal = "C++ string view";
    printView(literal);

    // 2. The same function accepts std::string without creating a copy.
    std::string message = "zero copy parameters";
    printView(message);
    std::cout << "word count = " << countWords(message) << "\n";

    // 3. substr creates another view into the same character storage.
    std::string_view prefix = literal.substr(0, 3);
    std::cout << "prefix = " << prefix << "\n";

    // A view into a destroyed or modified string would dangle and must be avoided.
    return 0;
}
