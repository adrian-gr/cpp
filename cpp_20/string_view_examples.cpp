// C++20 std::string_view examples
// Reference: https://en.cppreference.com/w/cpp/string/basic_string_view
// Compile with: g++ -std=c++20 -O2 cpp_20/string_view_examples.cpp -o bin/cpp20_string_view && ./bin/cpp20_string_view

#include <iostream>
#include <string_view>

// Problem solved: adds expressive prefix/suffix checks and constexpr text processing to non-owning views.
// Before C++20: code compared substrings manually or used temporary std::string objects.
// Tips for std::string_view usage:
// - Use starts_with and ends_with for clear boundary checks.
// - Continue to treat string_view as a non-owning reference with a strict lifetime requirement.
// - constexpr string_view operations are useful for compile-time parsing and validation.
// - Use std::string when the text must be owned or modified.

constexpr bool isCppSource(std::string_view path) {
    return path.ends_with(".cpp") || path.ends_with(".hpp");
}

constexpr std::string_view fileName(std::string_view path) {
    std::size_t separator = path.find_last_of('/');
    return separator == std::string_view::npos ? path : path.substr(separator + 1);
}

int main() {
    std::cout << "C++20 std::string_view examples:\n";

    // 1. C++20 adds direct prefix and suffix checks.
    std::string_view path = "src/example.cpp";
    std::cout << "starts with src = " << std::boolalpha
              << path.starts_with("src/") << "\n";
    std::cout << "is C++ source = " << isCppSource(path) << "\n";

    // 2. string_view operations can be evaluated at compile time.
    constexpr auto name = fileName("include/example.hpp");
    static_assert(name == "example.hpp");
    std::cout << "file name = " << name << "\n";

    return 0;
}
