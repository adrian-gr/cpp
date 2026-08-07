// C++11 atomic examples
// Compile with: g++ -std=c++11 -O2 -pthread cpp_11/atomic_examples.cpp -o bin/cpp11_atomic && ./bin/cpp11_atomic

#include <atomic>
#include <iostream>

// Tips for C++11 atomic usage:
// - Use std::atomic for shared scalar state accessed by multiple threads.
// - Atomic operations prevent data races, but they do not automatically protect invariants across values.
// - Use compare_exchange when an update depends on the current value.
// - Choose memory_order deliberately; sequential consistency is the simplest default.
// - An atomic variable is not a replacement for a mutex when protecting a compound data structure.

int main() {
    std::cout << "C++11 atomic examples:\n";

    // 1. Atomic load and store.
    std::atomic<int> value(10);
    std::cout << "initial value = " << value.load() << "\n";
    value.store(20);
    std::cout << "stored value = " << value.load() << "\n";

    // 2. Atomic read-modify-write operation.
    int previous = value.fetch_add(5);
    std::cout << "fetch_add returned = " << previous
              << ", current value = " << value.load() << "\n";

    // 3. Compare-and-exchange updates only when the expected value matches.
    int expected = 25;
    bool exchanged = value.compare_exchange_strong(expected, 100);
    std::cout << "exchange succeeded = " << std::boolalpha << exchanged
              << ", current value = " << value.load() << "\n";

    // 4. Explicit memory order can be used for a relaxed counter.
    value.fetch_add(1, std::memory_order_relaxed);
    std::cout << "relaxed increment = " << value.load(std::memory_order_relaxed) << "\n";

    return 0;
}
