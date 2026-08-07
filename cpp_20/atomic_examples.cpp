// C++20 atomic examples
// Compile with: g++ -std=c++20 -O2 -pthread cpp_20/atomic_examples.cpp -o bin/cpp20_atomic && ./bin/cpp20_atomic

#include <atomic>
#include <iostream>

// Tips for C++20 atomic usage:
// - Use atomic_ref to apply atomic operations to an existing suitably aligned object.
// - The referenced object must outlive the atomic_ref and must not be accessed non-atomically while shared.
// - Use wait and notify_one/all to block efficiently instead of repeatedly polling an atomic value.
// - Atomic wait observes value changes; it does not replace a condition variable for complex predicates.
// - Keep the memory-ordering contract documented when coordinating multiple threads.

int main() {
    std::cout << "C++20 atomic examples:\n";

    // 1. atomic_ref gives an atomic view of an existing integer.
    int sharedValue = 10;
    std::atomic_ref<int> atomicValue(sharedValue);
    atomicValue.fetch_add(5, std::memory_order_relaxed);
    std::cout << "atomic_ref value = " << sharedValue << "\n";

    // 2. wait and notify provide efficient atomic state coordination.
    std::atomic<int> state(0);
    state.store(1, std::memory_order_release);
    state.notify_one();
    state.wait(0, std::memory_order_acquire);
    std::cout << "state after wait = " << state.load() << "\n";

    // 3. C++20 still supports compare-exchange for conditional updates.
    int expected = 1;
    bool updated = state.compare_exchange_strong(expected, 2,
                                                  std::memory_order_acq_rel);
    std::cout << "compare-exchange succeeded = " << std::boolalpha << updated
              << ", state = " << state.load() << "\n";

    return 0;
}
