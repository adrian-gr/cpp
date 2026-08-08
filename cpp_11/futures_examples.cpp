// C++11 future examples
// Reference: https://en.cppreference.com/w/cpp/thread/future
// Compile with: g++ -std=c++11 -O2 -pthread cpp_11/futures_examples.cpp -o bin/cpp11_futures && ./bin/cpp11_futures

#include <future>
#include <iostream>
#include <thread>

// Problem solved: transfers results and exceptions safely between asynchronous work and its consumer.
// Before C++11: code used shared variables, callbacks, condition variables, or platform-specific APIs.
// Tips for C++11 future usage:
// - Use promise when producer and consumer are separate pieces of code.
// - Use packaged_task when wrapping a callable and exposing its future result.
// - Always call get when the result or task exception must be observed.
// - A future normally allows one get; use shared_future when multiple reads are required.

int main() {
    std::cout << "C++11 future examples:\n";

    // 1. A promise communicates a value from one thread to another.
    std::promise<int> promise;
    std::future<int> result = promise.get_future();
    std::thread producer([&promise] {
        promise.set_value(42);
    });
    std::cout << "promise result = " << result.get() << "\n";
    producer.join();

    // 2. packaged_task connects a callable to a future.
    std::packaged_task<int(int, int)> task([](int left, int right) {
        return left + right;
    });
    std::future<int> taskResult = task.get_future();
    task(20, 22);
    std::cout << "packaged task result = " << taskResult.get() << "\n";

    // 3. Exceptions raised by asynchronous work are rethrown by get.
    std::promise<int> failedPromise;
    std::future<int> failedResult = failedPromise.get_future();
    failedPromise.set_exception(std::make_exception_ptr(std::runtime_error("failure")));
    try {
        failedResult.get();
    } catch (const std::exception& error) {
        std::cout << "future exception = " << error.what() << "\n";
    }

    return 0;
}
