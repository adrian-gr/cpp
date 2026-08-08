// C++20 atomic examples
// Reference: https://en.cppreference.com/w/cpp/atomic
// Compile with: g++ -std=c++20 -O2 -pthread cpp_20/atomic_examples.cpp -o bin/cpp20_atomic && ./bin/cpp20_atomic

#include <atomic>
#include <iostream>

// Problem solved: extends atomic coordination with efficient waiting and atomic views of objects.
// Before C++20: code polled atomics or required condition variables and duplicated storage.
// Tips for C++20 atomic usage:
// - Use atomic_ref to apply atomic operations to an existing suitably aligned object.
// - The referenced object must outlive the atomic_ref and must not be accessed non-atomically while shared.
// - Use wait and notify_one/all to block efficiently instead of repeatedly polling an atomic value.
// - Atomic wait observes value changes; it does not replace a condition variable for complex predicates.
// - Use relaxed for independent counters, acquire/release for publication, and seq_cst for simplicity.
// - Use acq_rel for read-modify-write operations that both consume and publish synchronization state.
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

    // 4. Relaxed ordering is appropriate for an independent statistic.
    std::atomic<int> events(0);
    events.fetch_add(1, std::memory_order_relaxed);
    std::cout << "relaxed events = " << events.load(std::memory_order_relaxed) << "\n";

    // 5. Sequential consistency provides the strongest and simplest ordering.
    state.store(3, std::memory_order_seq_cst);
    std::cout << "seq_cst state = " << state.load(std::memory_order_seq_cst) << "\n";

    // 6. Acquire-release exchange synchronizes both the read and the write.
    int previousState = state.exchange(4, std::memory_order_acq_rel);
    std::cout << "acq_rel exchange: " << previousState
              << " -> " << state.load() << "\n";

    return 0;
}
