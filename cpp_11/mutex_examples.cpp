// C++11 mutex examples
// Reference: https://en.cppreference.com/w/cpp/thread/mutex
// Compile with: g++ -std=c++11 -O2 -pthread cpp_11/mutex_examples.cpp -o bin/cpp11_mutex && ./bin/cpp11_mutex

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Problem solved: protects shared mutable state with portable RAII-based synchronization.
// Before C++11: code used platform-specific locks and manual lock/unlock conventions.
// Tips for C++11 mutex usage:
// - Protect shared mutable state with a mutex when operations are not atomic.
// - Prefer RAII wrappers such as lock_guard and unique_lock over manual lock/unlock.
// - Keep the critical section small so other threads wait for as little time as possible.
// - Always acquire multiple mutexes in a consistent order or use std::lock to avoid deadlock.
// - A mutex protects the data only when every access follows the same locking policy.
// - shared_timed_mutex was introduced in C++14; C++11 requires a custom timed reader-writer lock.

class Counter {
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++value_;
    }

    int value() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return value_;
    }

private:
    mutable std::mutex mutex_;
    int value_ = 0;
};

int main() {
    std::cout << "C++11 mutex examples:\n";

    // 1. lock_guard locks on construction and unlocks on scope exit.
    Counter counter;
    std::vector<std::thread> workers;
    for (int index = 0; index < 4; ++index) {
        workers.emplace_back([&counter] {
            for (int iteration = 0; iteration < 1000; ++iteration) {
                counter.increment();
            }
        });
    }

    for (auto& worker : workers) {
        worker.join();
    }
    std::cout << "counter = " << counter.value() << "\n";

    // 2. unique_lock can be unlocked and locked again when needed.
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    std::cout << "unique_lock owns mutex = " << std::boolalpha
              << lock.owns_lock() << "\n";
    lock.unlock();
    std::cout << "unique_lock after unlock = " << lock.owns_lock() << "\n";

    return 0;
}
