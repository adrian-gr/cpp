// C++11 allocator examples
// Reference: https://en.cppreference.com/w/cpp/memory/allocator
// Compile with: g++ -std=c++11 -O2 cpp_11/allocator_examples.cpp -o bin/cpp11_allocator && ./bin/cpp11_allocator

#include <iostream>
#include <memory>
#include <string>

// Problem solved: separates object construction from the policy used to obtain storage.
// Before C++11: containers and custom pools relied on raw allocation APIs and manual lifetime rules.
// Tips for C++11 allocator usage:
// - Prefer standard containers unless a custom allocation strategy is required.
// - Construct and destroy objects through the allocator's lifetime functions.
// - Always deallocate with the same allocator and size information used for allocation.
// - Do not confuse allocated raw storage with a constructed object.

int main() {
    std::cout << "C++11 allocator examples:\n";

    std::allocator<std::string> allocator;
    std::string* text = allocator.allocate(1);
    allocator.construct(text, "allocated string");

    std::cout << "constructed value = " << *text << "\n";

    allocator.destroy(text);
    allocator.deallocate(text, 1);
    std::cout << "object destroyed and storage released\n";

    return 0;
}
