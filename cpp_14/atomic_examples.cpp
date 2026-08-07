// C++14 atomic examples
// Compile with: g++ -std=c++14 -O2 -pthread cpp_14/atomic_examples.cpp -o bin/cpp14_atomic && ./bin/cpp14_atomic

#include <atomic>
#include <iostream>

// Tips for C++14 atomic usage:
// - Use atomic_flag for the simplest guaranteed lock-free atomic flag.
// - Use acquire and release ordering when publishing data between threads.
// - Relaxed ordering is suitable for counters when no other synchronization is required.
// - Memory ordering controls visibility and ordering, not whether an operation is atomic.
// - Prefer a mutex when the operation involves multiple related pieces of state.

int main() {
    std::cout << "C++14 atomic examples:\n";

    // 1. atomic_flag supports a small spin-lock-like state transition.
    std::atomic_flag flag = ATOMIC_FLAG_INIT;
    bool wasSet = flag.test_and_set(std::memory_order_acquire);
    std::cout << "flag was already set = " << std::boolalpha << wasSet << "\n";
    flag.clear(std::memory_order_release);
    std::cout << "flag cleared\n";

    // 2. A relaxed atomic is useful for an independent statistic.
    std::atomic<unsigned> counter(0);
    counter.fetch_add(3, std::memory_order_relaxed);
    counter.fetch_add(2, std::memory_order_relaxed);
    std::cout << "relaxed counter = "
              << counter.load(std::memory_order_relaxed) << "\n";

    // 3. Release and acquire can publish a value through an atomic variable.
    int publishedValue = 42;
    std::atomic<bool> ready(false);
    ready.store(true, std::memory_order_release);
    if (ready.load(std::memory_order_acquire)) {
        std::cout << "published value = " << publishedValue << "\n";
    }

    return 0;
}
