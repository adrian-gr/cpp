// C++20 thread examples
// Compile with: g++ -std=c++20 -O2 -pthread cpp_20/thread_examples.cpp -o bin/cpp20_thread && ./bin/cpp20_thread

#include <chrono>
#include <iostream>
#include <stop_token>
#include <thread>

// Tips for C++20 thread usage:
// - Prefer std::jthread when a thread should be joined automatically on scope exit.
// - Use stop_token for cooperative cancellation; it does not forcibly terminate a thread.
// - A worker must check its stop token regularly and return promptly after cancellation.
// - Use a condition variable or another wake-up mechanism when a sleeping worker must stop quickly.

void work(std::stop_token stopToken) {
    for (int step = 1; step <= 5; ++step) {
        if (stopToken.stop_requested()) {
            std::cout << "work cancelled\n";
            return;
        }
        std::cout << "work step " << step << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

int main() {
    std::cout << "C++20 thread examples:\n";

    // 1. jthread automatically joins when it leaves scope.
    std::jthread worker(work);
    worker.request_stop();

    // 2. A jthread can also complete normally without an explicit join.
    std::jthread shortTask([] {
        std::cout << "short task completed\n";
    });

    return 0;
}
