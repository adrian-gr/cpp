// C++11 atomic examples
// Reference: https://en.cppreference.com/w/cpp/atomic
// Compile with: g++ -std=c++11 -O2 -pthread cpp_11/atomic_examples.cpp -o bin/cpp11_atomic && ./bin/cpp11_atomic

#include <atomic>
#include <iostream>

// Problem solved: provides portable atomic operations for shared state without data races.
// Before C++11: programs used mutexes, compiler intrinsics, or non-portable OS primitives.
// Tips for C++11 atomic usage:
// - Use std::atomic for shared scalar state accessed by multiple threads.
// - Atomic operations prevent data races, but they do not automatically protect invariants across values.
// - Use compare_exchange when an update depends on the current value.
// - Use relaxed for independent counters where ordering with other data is irrelevant.
// - Use release on a publishing store and acquire on the matching load.
// - Use acq_rel for read-modify-write operations that both consume and publish state.
// - Use seq_cst when simplicity and a single global order matter more than optimization.
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

    // 5. Sequential consistency is the default and gives the strongest ordering.
    value.store(30, std::memory_order_seq_cst);
    std::cout << "seq_cst value = " << value.load(std::memory_order_seq_cst) << "\n";

    // 6. Acquire-release ordering publishes data through a flag.
    int publishedValue = 42;
    std::atomic<bool> ready(false);
    ready.store(true, std::memory_order_release);
    if (ready.load(std::memory_order_acquire)) {
        std::cout << "acquire/release published value = " << publishedValue << "\n";
    }

    // 7. Acquire-release exchange both reads the old state and publishes the new state.
    int oldState = value.exchange(40, std::memory_order_acq_rel);
    std::cout << "acq_rel exchange: " << oldState << " -> " << value.load() << "\n";

    return 0;
}
