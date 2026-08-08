// C++20 condition variable examples
// Reference: https://en.cppreference.com/w/cpp/thread/condition_variable_any
// Compile with: g++ -std=c++20 -O2 -pthread cpp_20/condition_variable_examples.cpp -o bin/cpp20_condition_variable && ./bin/cpp20_condition_variable

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

// Problem solved: combines condition-variable waiting with cooperative cancellation.
// Before C++20: code needed a separate stop flag and careful notification to wake a worker.
// Tips for C++20 condition variables:
// - condition_variable_any supports stop-token-aware waits with a cancellation predicate.
// - Cancellation is cooperative; the waiting operation returns and the thread must finish cleanly.
// - Use condition_variable for ordinary mutex waits and condition_variable_any for flexible lock types.
// - Continue protecting the predicate state with the same lock used by the wait.

int main() {
    std::cout << "C++20 condition variable examples:\n";

    std::mutex mutex;
    std::condition_variable_any condition;
    bool ready = false;

    std::jthread worker([&](std::stop_token stopToken) {
        std::unique_lock<std::mutex> lock(mutex);
        bool completed = condition.wait(lock, stopToken, [&] {
            return ready;
        });
        std::cout << "wait completed normally = " << std::boolalpha
                  << completed << "\n";
    });

    worker.request_stop();
    condition.notify_all();

    return 0;
}
