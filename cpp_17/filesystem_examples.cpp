// C++17 std::filesystem examples
// Reference: https://en.cppreference.com/w/cpp/filesystem
// Compile with: g++ -std=c++17 -O2 cpp_17/filesystem_examples.cpp -o bin/cpp17_filesystem && ./bin/cpp17_filesystem

#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

// Problem solved: provides portable paths and filesystem operations instead of OS-specific APIs.
// Before C++17: code used platform APIs, string concatenation, and fragile path parsing.
// Tips for std::filesystem usage:
// - Use fs::path for path composition instead of concatenating strings with separators.
// - Prefer the error_code overloads when an expected filesystem failure should not throw.
// - Check existence and type before using metadata when the path may disappear concurrently.
// - Treat paths as data; do not assume that a path exists or is safe to access.

int main() {
    std::cout << "C++17 std::filesystem examples:\n";

    // 1. Paths compose correctly for the current platform.
    fs::path source = fs::current_path() / "cpp_17" / "filesystem_examples.cpp";
    std::cout << "path = " << source << "\n";
    std::cout << "filename = " << source.filename() << "\n";
    std::cout << "extension = " << source.extension() << "\n";

    // 2. Query metadata without manually parsing the path.
    std::error_code error;
    bool exists = fs::exists(source, error);
    std::cout << "exists = " << std::boolalpha << exists
              << ", error = " << error.message() << "\n";
    if (exists && !error) {
        std::cout << "file size = " << fs::file_size(source, error) << "\n";
    }

    // 3. Normalize a path lexically without accessing the filesystem.
    fs::path normalized = fs::path("cpp_17/../cpp_17") / "filesystem_examples.cpp";
    std::cout << "normalized = " << normalized.lexically_normal() << "\n";

    return 0;
}
