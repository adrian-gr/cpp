// Core ownership and lifetime examples
// Compile with: g++ -std=c++11 -O2 cpp_core/ownership_lifetime_examples.cpp -o bin/core_ownership && ./bin/core_ownership

#include <iostream>
#include <memory>
#include <utility>

// Tips:
// - Decide whether a function borrows, shares, or takes ownership of an object.
// - Use unique_ptr for one owner and shared_ptr only for genuine shared ownership.
// - Keep non-owning references valid for no longer than their source object.
// - Move ownership explicitly and leave moved-from objects in a valid state.

struct Resource {
    explicit Resource(int id) : id(id) {}
    int id;
};

void inspect(const Resource& resource) {
    std::cout << "borrowed resource = " << resource.id << "\n";
}

int main() {
    std::cout << "Core ownership and lifetime examples:\n";

    std::unique_ptr<Resource> owner(new Resource(42));
    inspect(*owner);

    auto transferred = std::move(owner);
    std::cout << "ownership transferred = " << std::boolalpha
              << (!owner && transferred != nullptr) << "\n";

    return 0;
}
