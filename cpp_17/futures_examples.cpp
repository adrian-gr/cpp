// C++17 future examples
// Reference: https://en.cppreference.com/w/cpp/thread/future
// Compile with: g++ -std=c++17 -O2 -pthread cpp_17/futures_examples.cpp -o bin/cpp17_futures && ./bin/cpp17_futures

#include <future>
#include <iostream>
#include <string>

// Problem solved: lets multiple consumers safely observe one asynchronous result.
// Before C++17: code shared a result manually or used repeated promise channels.
// Tips for C++17 future usage:
// - Convert a future to shared_future when multiple consumers need to call get.
// - shared_future copies share the same result and can each call get safely after completion.
// - Use wait and wait_for to coordinate readiness separately from result retrieval.
// - Futures transport exceptions, so handle failures at the point where get is called.

int main() {
    std::cout << "C++17 future examples:\n";

    // 1. share transfers the future state into a reusable shared_future.
    std::future<std::string> source = std::async(std::launch::async, [] {
        return std::string("ready");
    });
    std::shared_future<std::string> firstReader = source.share();
    std::shared_future<std::string> secondReader = firstReader;

    firstReader.wait();
    std::cout << "first reader = " << firstReader.get() << "\n";
    std::cout << "second reader = " << secondReader.get() << "\n";

    // 2. A shared future can be passed to independent consumers.
    auto numeric = std::async(std::launch::async, [] {
        return 42;
    }).share();
    std::cout << "shared numeric result = " << numeric.get() << "\n";
    std::cout << "shared numeric result again = " << numeric.get() << "\n";

    return 0;
}
