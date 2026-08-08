// C++17 mutex examples
// Reference: https://en.cppreference.com/w/cpp/thread/shared_timed_mutex
// Compile with: g++ -std=c++17 -O2 -pthread cpp_17/mutex_examples.cpp -o bin/cpp17_mutex && ./bin/cpp17_mutex

#include <chrono>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

// Problem solved: allows concurrent readers while retaining exclusive locking for writers.
// Before C++17: a single mutex serialized readers or required a custom reader-writer lock.
// Tips for C++17 mutex usage:
// - Use shared_mutex when many readers can safely access data concurrently.
// - Use a unique_lock for writers and a shared_lock for readers.
// - Shared locking is useful only when reads are genuinely independent and frequent.
// - A shared_mutex does not eliminate the need to protect every access consistently.
// - Use shared_timed_mutex when a reader or writer must stop waiting after a deadline.

class SharedValue {
public:
    int read() const {
        std::shared_lock<std::shared_mutex> lock(mutex_);
        return value_;
    }

    void write(int value) {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        value_ = value;
    }

private:
    mutable std::shared_mutex mutex_;
    int value_ = 0;
};

int main() {
    std::cout << "C++17 mutex examples:\n";

    // 1. Multiple readers can use a shared lock.
    SharedValue sharedValue;
    sharedValue.write(42);
    std::thread firstReader([&sharedValue] {
        std::cout << "first reader = " << sharedValue.read() << "\n";
    });
    std::thread secondReader([&sharedValue] {
        std::cout << "second reader = " << sharedValue.read() << "\n";
    });
    firstReader.join();
    secondReader.join();

    // 2. Writers obtain exclusive access.
    sharedValue.write(100);
    std::cout << "after writer = " << sharedValue.read() << "\n";

    // 3. shared_timed_mutex adds time-bounded shared and exclusive acquisition.
    std::shared_timed_mutex timedMutex;
    std::shared_lock<std::shared_timed_mutex> timedReader(
        timedMutex, std::defer_lock);
    bool readerAcquired = timedReader.try_lock_for(std::chrono::milliseconds(10));
    std::cout << "timed reader acquired = " << readerAcquired << "\n";
    if (readerAcquired) {
        timedReader.unlock();
    }

    std::unique_lock<std::shared_timed_mutex> timedWriter(
        timedMutex, std::defer_lock);
    bool writerAcquired = timedWriter.try_lock_for(std::chrono::milliseconds(10));
    std::cout << "timed writer acquired = " << writerAcquired << "\n";

    return 0;
}
