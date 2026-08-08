// C++11 condition variable examples
// Reference: https://en.cppreference.com/w/cpp/thread/condition_variable
// Compile with: g++ -std=c++11 -O2 -pthread cpp_11/condition_variable_examples.cpp -o bin/cpp11_condition_variable && ./bin/cpp11_condition_variable

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// Problem solved: lets a thread sleep until shared state changes instead of polling.
// Before C++11: code used busy loops, platform-specific events, or ad hoc sleep intervals.
// Tips for condition variables:
// - Always wait with a mutex and a predicate that describes the required state.
// - Use the predicate overload because wakeups can be spurious.
// - Change the protected state while holding the same mutex used by the waiter.
// - Notify after changing the state; notify_one wakes one waiter and notify_all wakes all.

int main() {
    std::cout << "C++11 condition variable examples:\n";

    std::mutex mutex;
    std::condition_variable condition;
    bool ready = false;

    std::thread worker([&] {
        std::unique_lock<std::mutex> lock(mutex);
        condition.wait(lock, [&] {
            return ready;
        });
        std::cout << "worker observed ready state\n";
    });

    {
        std::lock_guard<std::mutex> lock(mutex);
        ready = true;
    }
    condition.notify_one();
    worker.join();

    return 0;
}
