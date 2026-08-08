// C++14 future examples
// Reference: https://en.cppreference.com/w/cpp/thread/future
// Compile with: g++ -std=c++14 -O2 -pthread cpp_14/futures_examples.cpp -o bin/cpp14_futures && ./bin/cpp14_futures

#include <chrono>
#include <future>
#include <iostream>
#include <thread>

// Problem solved: schedules work and returns a result without manually managing worker threads.
// Before C++14: code paired raw threads with promises or packaged_task for common asynchronous calls.
// Tips for C++14 future usage:
// - Use launch::async when the task should run asynchronously.
// - Use launch::deferred when lazy execution is useful and the caller will call get or wait.
// - Specify a launch policy when the scheduling behavior matters; the default may defer work.
// - Use wait_for to inspect readiness without blocking indefinitely.

int calculate() {
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    return 42;
}

int main() {
    std::cout << "C++14 future examples:\n";

    // 1. launch::async requests asynchronous execution.
    auto asynchronous = std::async(std::launch::async, calculate);
    std::cout << "async result = " << asynchronous.get() << "\n";

    // 2. launch::deferred runs the callable when get or wait is called.
    auto deferred = std::async(std::launch::deferred, [] {
        return 20 + 22;
    });
    std::cout << "deferred ready before get = " << std::boolalpha
              << (deferred.wait_for(std::chrono::milliseconds(0)) ==
                  std::future_status::ready) << "\n";
    std::cout << "deferred result = " << deferred.get() << "\n";

    // 3. wait_for can report timeout without consuming the result.
    auto delayed = std::async(std::launch::async, calculate);
    auto status = delayed.wait_for(std::chrono::milliseconds(0));
    std::cout << "delayed status is timeout = "
              << (status == std::future_status::timeout) << "\n";
    std::cout << "delayed result = " << delayed.get() << "\n";

    return 0;
}
