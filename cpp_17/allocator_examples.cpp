// C++17 allocator examples
// Reference: https://en.cppreference.com/w/cpp/memory/memory_resource
// Compile with: g++ -std=c++17 -O2 cpp_17/allocator_examples.cpp -o bin/cpp17_allocator && ./bin/cpp17_allocator

#include <iostream>
#include <memory_resource>
#include <string>
#include <vector>

// Problem solved: lets containers use interchangeable runtime memory-resource strategies.
// Before C++17: custom allocators were usually statically typed and difficult to switch at runtime.
// Tips for C++17 allocator usage:
// - Use pmr containers when the memory resource should be selected at runtime.
// - Ensure a memory resource outlives every object allocated from it.
// - Use monotonic_buffer_resource for batches with a shared lifetime and bulk release.
// - Do not use a monotonic resource when individual deallocation must reclaim memory immediately.

int main() {
    std::cout << "C++17 allocator examples:\n";

    // 1. A polymorphic allocator lets a vector use a runtime-selected resource.
    std::byte buffer[256];
    std::pmr::monotonic_buffer_resource resource(buffer, sizeof(buffer));
    std::pmr::vector<int> values{&resource};
    values.push_back(10);
    values.push_back(20);
    values.push_back(30);
    std::cout << "pmr vector sum = " << values[0] + values[1] + values[2] << "\n";

    // 2. pmr::string can use the same resource as the container.
    std::pmr::vector<std::pmr::string> names{&resource};
    names.emplace_back("Ada");
    names.emplace_back("Grace");
    std::cout << "first name = " << names.front() << "\n";

    // 3. Resource memory is released together when the resource is destroyed.
    std::cout << "resource-backed names = " << names.size() << "\n";

    return 0;
}
