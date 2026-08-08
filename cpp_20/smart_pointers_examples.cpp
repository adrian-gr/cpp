// C++20 smart pointer examples
// Reference: https://en.cppreference.com/w/cpp/memory
// Compile with: g++ -std=c++20 -O2 -pthread cpp_20/smart_pointers_examples.cpp -o bin/cpp20_smart_pointers && ./bin/cpp20_smart_pointers

#include <atomic>
#include <iostream>
#include <memory>
#include <string>

// Problem solved: coordinates shared_ptr ownership changes safely between threads.
// Before C++20: code used external locks around shared_ptr operations or unsafe shared access.
// Tips for C++20 smart pointer usage:
// - Use atomic_load and atomic_store for concurrent access to the same shared_ptr object.
// - The pointed-to object is not automatically thread-safe; protect its mutable state separately.
// - Prefer unique_ptr when ownership has one clear thread or owner.
// - Use weak_ptr for observers and cycle prevention, especially in graphs and callbacks.

struct Snapshot {
    explicit Snapshot(std::string value) : value(std::move(value)) {}
    std::string value;
};

int main() {
    std::cout << "C++20 smart pointer examples:\n";

    // 1. atomic_load and atomic_store coordinate shared_ptr replacement.
    std::shared_ptr<Snapshot> current = std::make_shared<Snapshot>("initial");
    auto replacement = std::make_shared<Snapshot>("updated");
    std::atomic_store_explicit(&current, replacement, std::memory_order_release);
    auto observed = std::atomic_load_explicit(&current, std::memory_order_acquire);
    std::cout << "observed snapshot = " << observed->value << "\n";

    // 2. weak_ptr observes the current object without extending its lifetime.
    std::weak_ptr<Snapshot> observer = current;
    if (auto locked = observer.lock()) {
        std::cout << "weak observation = " << locked->value << "\n";
    }

    // 3. unique_ptr remains the simplest choice for exclusive ownership.
    auto exclusive = std::make_unique<int>(2020);
    std::cout << "exclusive value = " << *exclusive << "\n";

    return 0;
}
