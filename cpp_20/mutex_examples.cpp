// C++20 mutex examples
// Compile with: g++ -std=c++20 -O2 -pthread cpp_20/mutex_examples.cpp -o bin/cpp20_mutex && ./bin/cpp20_mutex

#include <iostream>
#include <mutex>
#include <thread>

// Tips for C++20 mutex usage:
// - Use scoped_lock to lock multiple mutexes with deadlock avoidance.
// - scoped_lock is the preferred RAII wrapper when all mutexes should be held for a scope.
// - Keep lock ownership and the protected data close together in the design.
// - Use a condition_variable when a thread must wait for a state change, rather than polling.

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

    return 0;
}
