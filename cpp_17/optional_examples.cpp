// C++17 std::optional examples
// Compile with: g++ -std=c++17 -O2 cpp_17/optional_examples.cpp -o bin/cpp17_optional && ./bin/cpp17_optional

#include <iostream>
#include <optional>
#include <string>

// Tips for std::optional usage:
// - Use std::optional when a function may legitimately have no result.
// - Prefer optional over sentinel values such as -1, empty strings, or null pointers.
// - Check has_value(), use operator bool, or use value_or before accessing a value.
// - Use value() when absence is exceptional and throwing std::bad_optional_access is acceptable.
// - Avoid optional for required values and avoid nested optional unless two absence states are meaningful.

std::optional<int> findUserId(const std::string& name) {
    if (name == "Ada") {
        return 42;
    }
    return std::nullopt;
}

std::optional<std::string> makeGreeting(bool includeGreeting) {
    if (!includeGreeting) {
        return std::nullopt;
    }
    return "Hello from std::optional";
}

int main() {
    std::cout << "C++17 std::optional examples:\n";

    // 1. An optional can contain a value or represent an empty result.
    std::optional<int> userId = findUserId("Ada");
    if (userId) {
        std::cout << "Ada's user id = " << *userId << "\n";
    }

    std::optional<int> missingId = findUserId("Unknown");
    std::cout << "missing id = " << missingId.value_or(-1) << "\n";

    // 2. value() provides checked access and throws when the optional is empty.
    try {
        std::cout << "missing value = " << missingId.value() << "\n";
    } catch (const std::bad_optional_access& error) {
        std::cout << "access failed: " << error.what() << "\n";
    }

    // 3. emplace and reset manage the contained object's lifetime.
    std::optional<std::string> greeting;
    greeting.emplace("ready");
    std::cout << "greeting = " << *greeting << "\n";
    greeting.reset();
    std::cout << "greeting has value = " << std::boolalpha
              << greeting.has_value() << "\n";

    // 4. Functions can return optional to distinguish success from no result.
    if (auto result = makeGreeting(true)) {
        std::cout << *result << "\n";
    }

    return 0;
}
