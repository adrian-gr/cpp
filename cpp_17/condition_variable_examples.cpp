// C++17 condition variable examples
// Reference: https://en.cppreference.com/w/cpp/thread/condition_variable
// Compile with: g++ -std=c++17 -O2 -pthread cpp_17/condition_variable_examples.cpp -o bin/cpp17_condition_variable && ./bin/cpp17_condition_variable

#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Problem solved: wakes multiple waiting threads when a shared state transition is complete.
// Before C++17: code manually coordinated each waiter or used repeated notifications.
// Tips for condition variable usage:
// - Use notify_all when every waiter may continue after one state change.
// - Keep the predicate tied to protected state, not to the notification itself.
// - Notifications are not queued; the predicate preserves state across missed notifications.
// - Use a queue or other protected data structure when transferring multiple work items.

int main() {
    std::cout << "C++17 condition variable examples:\n";

    std::mutex mutex;
    std::condition_variable condition;
    bool started = false;
    std::vector<std::thread> workers;

    for (int id = 1; id <= 3; ++id) {
        workers.emplace_back([&, id] {
            std::unique_lock<std::mutex> lock(mutex);
            condition.wait(lock, [&] {
                return started;
            });
            std::cout << "worker " << id << " started\n";
        });
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        started = true;
    }
    condition.notify_all();

    for (auto& worker : workers) {
        worker.join();
    }

    return 0;
}
