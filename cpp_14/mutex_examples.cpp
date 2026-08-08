// C++14 mutex examples
// Reference: https://en.cppreference.com/w/cpp/thread/shared_timed_mutex
// Compile with: g++ -std=c++14 -O2 -pthread cpp_14/mutex_examples.cpp -o bin/cpp14_mutex && ./bin/cpp14_mutex

#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>

// Problem solved: bounds lock acquisition to a time limit when waiting indefinitely is undesirable.
// Before C++14: code used polling loops, platform APIs, or unbounded mutex waits.
// Tips for C++14 mutex usage:
// - Use timed_mutex when a thread should stop waiting after a bounded interval.
// - Treat a timeout as a normal state that the program must handle explicitly.
// - Do not use timeouts to hide lock-ordering or contention problems.
// - Prefer lock_guard for unconditional ownership and unique_lock for flexible ownership.
// - Use shared_timed_mutex when both shared and exclusive acquisition need timeouts.

int main() {
    std::cout << "C++14 mutex examples:\n";

    // 1. try_lock_for waits for at most the requested duration.
    std::timed_mutex timedMutex;
    std::unique_lock<std::timed_mutex> lock(
        timedMutex, std::defer_lock);
    bool acquired = lock.try_lock_for(std::chrono::milliseconds(10));
    std::cout << "timed lock acquired = " << std::boolalpha << acquired << "\n";

    // 2. defer_lock lets unique_lock be constructed without locking immediately.
    if (acquired) {
        std::cout << "critical section is protected\n";
        lock.unlock();
    }

    // 3. lock_guard remains the simplest choice for a complete scope.
    std::mutex mutex;
    {
        std::lock_guard<std::mutex> guard(mutex);
        std::cout << "lock_guard protects this scope\n";
    }

    // 4. shared_timed_mutex supports timed shared and exclusive locking.
    std::shared_timed_mutex sharedTimedMutex;
    std::shared_lock<std::shared_timed_mutex> sharedLock(
        sharedTimedMutex, std::defer_lock);
    bool sharedAcquired = sharedLock.try_lock_for(std::chrono::milliseconds(10));
    std::cout << "timed shared lock acquired = " << sharedAcquired << "\n";
    if (sharedAcquired) {
        sharedLock.unlock();
    }

    std::unique_lock<std::shared_timed_mutex> exclusiveLock(
        sharedTimedMutex, std::defer_lock);
    bool exclusiveAcquired = exclusiveLock.try_lock_for(
        std::chrono::milliseconds(10));
    std::cout << "timed exclusive lock acquired = " << exclusiveAcquired << "\n";

    return 0;
}
