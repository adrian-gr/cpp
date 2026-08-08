// C++17 std::exchange examples
// Reference: https://en.cppreference.com/w/cpp/utility/exchange
// Compile with: g++ -std=c++17 -O2 cpp_17/exchange_examples.cpp -o bin/cpp17_exchange && ./bin/cpp17_exchange

#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <variant>

// Problem solved: makes one-step state transitions readable in modern value types.
// Before C++17: code used temporary variables and repetitive assignment logic for optional or variant state.
// Tips for std::exchange usage:
// - Use exchange when the old state must be consumed while installing a new state.
// - It works naturally with optional and variant values.
// - Remember that the returned old value is copied or moved according to the value type.
// - Use atomic exchange instead when the state is shared between threads.

int main() {
    std::cout << "C++17 std::exchange examples:\n";

    // 1. Exchange an optional value and inspect the old state.
    std::optional<std::string> message = "pending";
    auto previousMessage = std::exchange(message, std::nullopt);
    std::cout << "previous message = " << *previousMessage
              << ", has current message = " << std::boolalpha
              << message.has_value() << "\n";

    // 2. Exchange the active alternative of a variant.
    std::variant<int, std::string> state = 42;
    auto previousState = std::exchange(state, std::string("ready"));
    std::cout << "previous variant index = " << previousState.index()
              << ", current index = " << state.index() << "\n";

    // 3. Use exchange to consume a pending flag.
    bool pending = true;
    bool wasPending = std::exchange(pending, false);
    std::cout << "was pending = " << wasPending
              << ", pending now = " << pending << "\n";

    return 0;
}
