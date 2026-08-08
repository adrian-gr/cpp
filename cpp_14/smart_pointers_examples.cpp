// C++14 smart pointer examples
// Reference: https://en.cppreference.com/w/cpp/memory
// Compile with: g++ -std=c++14 -O2 cpp_14/smart_pointers_examples.cpp -o bin/cpp14_smart_pointers && ./bin/cpp14_smart_pointers

#include <cstdio>
#include <iostream>
#include <memory>

// Problem solved: makes unique ownership construction concise and exception-safe.
// Before C++14: unique_ptr often required an explicit new expression or helper factory.
// Tips for C++14 smart pointer usage:
// - Prefer make_unique over writing unique_ptr<T>(new T(...)).
// - Use a custom deleter when a resource needs an API other than delete.
// - Keep the deleter part of the pointer's ownership policy and type.
// - Use release only when transferring ownership to an API that documents it.

struct FileCloser {
    void operator()(std::FILE* file) const {
        if (file) {
            std::fclose(file);
        }
    }
};

int main() {
    std::cout << "C++14 smart pointer examples:\n";

    // 1. make_unique avoids an explicit new expression.
    auto number = std::make_unique<int>(42);
    std::cout << "unique value = " << *number << "\n";

    // 2. A custom deleter manages a C library resource.
    std::unique_ptr<std::FILE, FileCloser> file(
        std::tmpfile());
    std::cout << "temporary file opened = " << std::boolalpha
              << static_cast<bool>(file) << "\n";

    // 3. Ownership can be transferred explicitly with move.
    auto transferred = std::move(number);
    std::cout << "source is empty after move = " << !number
              << ", transferred value = " << *transferred << "\n";

    return 0;
}
