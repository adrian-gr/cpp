// C++11 thread examples
// Compile with: g++ -std=c++11 -O2 -pthread cpp_11/thread_examples.cpp -o bin/cpp11_thread && ./bin/cpp11_thread

#include <iostream>
#include <thread>
#include <vector>

// Tips for C++11 thread usage:
// - Every joinable thread must be joined or detached before its destructor runs.
// - Prefer join when the created work must finish before the current scope exits.
// - Capture shared state carefully; use a mutex or atomic when threads access it concurrently.
// - Keep thread functions small and give ownership of resources a clear lifetime.
// - Creating a thread is relatively expensive; use a pool for many short tasks.

void printWorker(int id) {
    std::cout << "worker " << id << " completed\n";
}

int main() {
    std::cout << "C++11 thread examples:\n";

    // 1. Start a thread with a regular function and wait for it.
    std::thread worker(printWorker, 1);
    worker.join();

    // 2. Start several threads with a lambda.
    std::vector<std::thread> workers;
    for (int id = 2; id <= 4; ++id) {
        workers.emplace_back([id] {
            printWorker(id);
        });
    }
    for (auto& thread : workers) {
        thread.join();
    }

    // 3. Inspect the current thread and available concurrency.
    std::cout << "hardware concurrency hint = "
              << std::thread::hardware_concurrency() << "\n";

    return 0;
}
