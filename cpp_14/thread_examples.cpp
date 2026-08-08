// C++14 thread examples
// Reference: https://en.cppreference.com/w/cpp/thread
// Compile with: g++ -std=c++14 -O2 -pthread cpp_14/thread_examples.cpp -o bin/cpp14_thread && ./bin/cpp14_thread

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

// Problem solved: represents asynchronous work and its result without manual thread bookkeeping.
// Before C++14: code paired raw threads with shared result storage and ad hoc error propagation.
// Tips for C++14 thread usage:
// - Use std::async when a task's result is more important than managing a thread directly.
// - std::launch::async requests that the task run asynchronously instead of being deferred.
// - Always retrieve a future's result when exceptions from the task must be observed.
// - Avoid using async for tiny tasks when its scheduling overhead dominates the work.

int calculate(int first, int second) {
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    return first + second;
}

int main() {
    std::cout << "C++14 thread examples:\n";

    // 1. Launch a task and obtain its result through a future.
    std::future<int> result = std::async(std::launch::async, calculate, 20, 22);
    std::cout << "async result = " << result.get() << "\n";

    // 2. Launch multiple tasks and combine their results.
    auto first = std::async(std::launch::async, calculate, 1, 2);
    auto second = std::async(std::launch::async, calculate, 3, 4);
    std::cout << "combined result = " << first.get() + second.get() << "\n";

    return 0;
}
