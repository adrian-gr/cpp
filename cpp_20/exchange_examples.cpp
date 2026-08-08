// C++20 std::exchange examples
// Reference: https://en.cppreference.com/w/cpp/utility/exchange
// Compile with: g++ -std=c++20 -O2 cpp_20/exchange_examples.cpp -o bin/cpp20_exchange && ./bin/cpp20_exchange

#include <iostream>
#include <string>
#include <utility>

// Problem solved: expresses ownership and state handoff while preserving move-only types.
// Before C++20: code needed verbose move construction, assignment, and reset sequences.
// Tips for std::exchange usage:
// - Use exchange in move constructors to take a resource and leave a safe empty state.
// - Keep the replacement state valid so the moved-from object remains destructible.
// - std::exchange is not atomic; combine it with synchronization for shared state.
// - Prefer a named operation when the transition has validation or multiple invariants.

class ResourceOwner {
public:
    explicit ResourceOwner(std::string resource)
        : resource_(std::move(resource)) {}

    ResourceOwner(ResourceOwner&& other) noexcept
        : resource_(std::exchange(other.resource_, "empty")) {}

    const std::string& resource() const {
        return resource_;
    }

private:
    std::string resource_;
};

int main() {
    std::cout << "C++20 std::exchange examples:\n";

    // 1. A move constructor can take ownership and reset the source in one expression.
    ResourceOwner original("file handle");
    ResourceOwner moved(std::move(original));
    std::cout << "moved resource = " << moved.resource() << "\n";
    std::cout << "source resource = " << original.resource() << "\n";

    // 2. Exchange also provides a concise state transition for ordinary values.
    std::string mode = "starting";
    std::cout << "old mode = " << std::exchange(mode, "running")
              << ", new mode = " << mode << "\n";

    return 0;
}
