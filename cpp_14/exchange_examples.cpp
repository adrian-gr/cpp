// C++14 std::exchange examples
// Reference: https://en.cppreference.com/w/cpp/utility/exchange
// Compile with: g++ -std=c++14 -O2 cpp_14/exchange_examples.cpp -o bin/cpp14_exchange && ./bin/cpp14_exchange

#include <iostream>
#include <string>
#include <utility>

// Problem solved: replaces a value and returns its old value in one clear operation.
// Before C++14: code manually copied the old value, assigned the new value, and returned the copy.
// Tips for std::exchange usage:
// - Use std::exchange for state transitions that need the previous value.
// - The replacement value is moved into the object when appropriate.
// - std::exchange does not provide synchronization; protect shared state separately.
// - Avoid using it when a simple assignment is clearer.

int main() {
    std::cout << "C++14 std::exchange examples:\n";

    // 1. Replace a scalar and receive the previous value.
    int state = 1;
    int previous = std::exchange(state, 2);
    std::cout << "previous = " << previous << ", current = " << state << "\n";

    // 2. Exchange works with strings and moves the replacement value.
    std::string current = "old";
    std::string replacement = "new";
    std::string oldText = std::exchange(current, std::move(replacement));
    std::cout << "old text = " << oldText << ", current text = " << current << "\n";

    // 3. A common flag transition becomes explicit.
    bool connected = false;
    bool wasConnected = std::exchange(connected, true);
    std::cout << "was connected = " << std::boolalpha << wasConnected
              << ", now connected = " << connected << "\n";

    return 0;
}
