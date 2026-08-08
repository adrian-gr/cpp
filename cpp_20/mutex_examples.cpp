// C++20 mutex examples
// Reference: https://en.cppreference.com/w/cpp/thread/shared_timed_mutex
// Compile with: g++ -std=c++20 -O2 -pthread cpp_20/mutex_examples.cpp -o bin/cpp20_mutex && ./bin/cpp20_mutex

#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

// Problem solved: locks multiple mutexes with RAII while avoiding common lock-order deadlocks.
// Before C++20: code used std::lock plus separate wrappers or manually coordinated lock ordering.
// Tips for C++20 mutex usage:
// - Use scoped_lock to lock multiple mutexes with deadlock avoidance.
// - scoped_lock is the preferred RAII wrapper when all mutexes should be held for a scope.
// - Keep lock ownership and the protected data close together in the design.
// - Use a condition_variable when a thread must wait for a state change, rather than polling.
// - Use shared_timed_mutex when shared or exclusive locking needs a timeout.

class Pair {
public:
    void swapValues() {
        std::scoped_lock lock(firstMutex_, secondMutex_);
        std::swap(first_, second_);
    }

    int first() const {
        std::lock_guard<std::mutex> lock(firstMutex_);
        return first_;
    }

    int second() const {
        std::lock_guard<std::mutex> lock(secondMutex_);
        return second_;
    }

private:
    mutable std::mutex firstMutex_;
    mutable std::mutex secondMutex_;
    int first_ = 1;
    int second_ = 2;
};

int main() {
    std::cout << "C++20 mutex examples:\n";

    // scoped_lock safely acquires both mutexes before swapping the values.
    Pair pair;
    pair.swapValues();
    std::cout << "after swap = " << pair.first() << ", " << pair.second() << "\n";

    // scoped_lock can also protect a single mutex with the same RAII style.
    std::mutex mutex;
    {
        std::scoped_lock lock(mutex);
        std::cout << "scoped_lock protects this scope\n";
    }

    // shared_timed_mutex supports time-bounded reader and writer acquisition.
    std::shared_timed_mutex timedMutex;
    std::shared_lock<std::shared_timed_mutex> reader(timedMutex, std::defer_lock);
    bool readerAcquired = reader.try_lock_for(std::chrono::milliseconds(10));
    std::cout << "timed reader acquired = " << readerAcquired << "\n";
    if (readerAcquired) {
        reader.unlock();
    }

    std::unique_lock<std::shared_timed_mutex> writer(timedMutex, std::defer_lock);
    bool writerAcquired = writer.try_lock_for(std::chrono::milliseconds(10));
    std::cout << "timed writer acquired = " << writerAcquired << "\n";

    return 0;
}
