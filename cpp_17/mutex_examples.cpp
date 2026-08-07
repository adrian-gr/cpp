// C++17 mutex examples
// Compile with: g++ -std=c++17 -O2 -pthread cpp_17/mutex_examples.cpp -o bin/cpp17_mutex && ./bin/cpp17_mutex

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <thread>

// Tips for C++17 mutex usage:
// - Use shared_mutex when many readers can safely access data concurrently.
// - Use a unique_lock for writers and a shared_lock for readers.
// - Shared locking is useful only when reads are genuinely independent and frequent.
// - A shared_mutex does not eliminate the need to protect every access consistently.

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

    return 0;
}
