// C++20 allocator examples
// Reference: https://en.cppreference.com/w/cpp/memory/memory_resource
// Compile with: g++ -std=c++20 -O2 cpp_20/allocator_examples.cpp -o bin/cpp20_allocator && ./bin/cpp20_allocator

#include <array>
#include <iostream>
#include <memory_resource>
#include <string>

// Problem solved: provides precise control over short-lived allocation arenas and resource reuse.
// Before C++20: code used custom pools or platform allocators and manually coordinated their lifetime.
// Tips for C++20 allocator usage:
// - Use unsynchronized_pool_resource when allocations are confined to one thread.
// - Use synchronized_pool_resource when the same resource is accessed by multiple threads.
// - Use release to return all pooled memory at once when a batch is complete.
// - Keep allocated objects alive only while their resource remains valid.

int main() {
    std::cout << "C++20 allocator examples:\n";

    // 1. A pool resource efficiently serves many same-thread small allocations.
    std::pmr::unsynchronized_pool_resource pool;
    std::pmr::string first("first", &pool);
    std::pmr::string second("second", &pool);
    std::cout << "pool strings = " << first << ", " << second << "\n";

    // 2. A monotonic resource uses a fixed initial buffer and bulk release.
    std::array<std::byte, 512> buffer{};
    std::pmr::monotonic_buffer_resource arena(buffer.data(), buffer.size(), &pool);
    std::pmr::string message(&arena);
    message = "temporary arena allocation";
    std::cout << "arena message = " << message << "\n";
    arena.release();

    // 3. The resource can be reused after releasing its previous batch.
    std::pmr::string nextMessage(&arena);
    nextMessage = "new batch";
    std::cout << "reused arena = " << nextMessage << "\n";

    return 0;
}
