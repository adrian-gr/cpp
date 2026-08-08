// C++14 condition variable examples
// Reference: https://en.cppreference.com/w/cpp/thread/condition_variable
// Compile with: g++ -std=c++14 -O2 -pthread cpp_14/condition_variable_examples.cpp -o bin/cpp14_condition_variable && ./bin/cpp14_condition_variable

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// Problem solved: allows a wait to stop after a bounded amount of time.
// Before C++14: code used manual clocks and polling loops around condition variables.
// Tips for timed condition-variable waits:
// - Use wait_for for a relative timeout and wait_until for a deadline.
// - Treat timeout as a normal result and decide how the caller should recover.
// - Keep using a predicate so spurious wakeups cannot be mistaken for success.
// - A timeout does not cancel the producer or change the protected state.

int main() {
    std::cout << "C++14 condition variable examples:\n";

    std::mutex mutex;
    std::condition_variable condition;
    bool ready = false;

    std::unique_lock<std::mutex> lock(mutex);
    bool completed = condition.wait_for(lock, std::chrono::milliseconds(1), [&] {
        return ready;
    });
    std::cout << "wait completed before timeout = " << std::boolalpha
              << completed << "\n";

    ready = true;
    lock.unlock();
    condition.notify_one();
    std::cout << "state changed after timeout\n";

    return 0;
}
