// C++20 std::filesystem examples
// Reference: https://en.cppreference.com/w/cpp/filesystem
// Compile with: g++ -std=c++20 -O2 cpp_20/filesystem_examples.cpp -o bin/cpp20_filesystem && ./bin/cpp20_filesystem

#include <filesystem>
#include <iostream>
#include <system_error>

namespace fs = std::filesystem;

// Problem solved: makes directory traversal and error-aware metadata queries portable and composable.
// Before C++20: code combined platform directory APIs with manual error and resource management.
// Tips for std::filesystem usage:
// - Use directory_entry when iterating because it can cache queried metadata.
// - Use directory_options when traversal should tolerate permission errors.
// - Prefer error_code overloads for expected operational failures in tools and services.
// - Avoid following symlinks unless that behavior is intentional and secured.

int main() {
    std::cout << "C++20 std::filesystem examples:\n";

    // 1. Iterate over the repository's C++20 directory with permission errors skipped.
    fs::path directory = fs::current_path() / "cpp_20";
    std::error_code error;
    std::size_t regularFiles = 0;
    for (const fs::directory_entry& entry : fs::directory_iterator(
             directory, fs::directory_options::skip_permission_denied, error)) {
        if (entry.is_regular_file(error)) {
            ++regularFiles;
        }
    }
    std::cout << "regular files in cpp_20 = " << regularFiles << "\n";
    if (error) {
        std::cout << "iteration error = " << error.message() << "\n";
    }

    // 2. weakly_canonical resolves what exists while tolerating a missing suffix.
    fs::path candidate = directory / "missing" / "example.cpp";
    fs::path resolved = fs::weakly_canonical(candidate, error);
    std::cout << "resolved parent = " << resolved.parent_path() << "\n";

    return 0;
}
