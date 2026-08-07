// C++17 atomic examples
// Compile with: g++ -std=c++17 -O2 -pthread cpp_17/atomic_examples.cpp -o bin/cpp17_atomic && ./bin/cpp17_atomic

#include <atomic>
#include <iostream>
#include <type_traits>

// Tips for C++17 atomic usage:
// - Use std::atomic<T>::is_always_lock_free to inspect a type's lock-free guarantee at compile time.
// - Use is_lock_free() when the guarantee depends on the target platform or object alignment.
// - Lock-free does not necessarily mean faster; measure before choosing a design.
// - Keep atomic operations small and use a mutex for larger invariants or data structures.

int main() {
    std::cout << "C++17 atomic examples:\n";

    // 1. C++17 exposes a compile-time lock-free property.
    static_assert(std::atomic<int>::is_always_lock_free,
                  "int should be lock-free on this target");
    std::atomic<int> integer(10);
    std::cout << "int is lock-free = " << std::boolalpha
              << integer.is_lock_free() << "\n";

    // 2. Atomic integral operations support fetch and exchange operations.
    int oldValue = integer.exchange(25, std::memory_order_acq_rel);
    std::cout << "old value = " << oldValue
              << ", new value = " << integer.load() << "\n";

    // 3. Atomic pointers can publish or replace an address safely.
    int first = 1;
    int second = 2;
    std::atomic<int*> pointer(&first);
    pointer.store(&second, std::memory_order_release);
    int* current = pointer.load(std::memory_order_acquire);
    std::cout << "published pointer value = " << *current << "\n";

    return 0;
}
