// C++20 future examples
// Reference: https://en.cppreference.com/w/cpp/thread/future
// Compile with: g++ -std=c++20 -O2 -pthread cpp_20/futures_examples.cpp -o bin/cpp20_futures && ./bin/cpp20_futures

#include <future>
#include <iostream>
#include <thread>

// Problem solved: combines future-based result transport with automatically managed threads.
// Before C++20: code manually joined std::thread while separately coordinating a promise.
// Tips for C++20 future usage:
// - Use jthread for automatic joining and promise/future for one-shot result delivery.
// - A future does not cancel work; pair it with a stop_token when cancellation is required.
// - Use shared_future for multiple readers and future for one consuming reader.
// - Keep the promise alive until the producer has either set a value or an exception.

int main() {
    std::cout << "C++20 future examples:\n";

    // 1. jthread manages the worker lifetime while promise transports the result.
    std::promise<int> promise;
    std::future<int> result = promise.get_future();
    std::jthread worker([promise = std::move(promise)]() mutable {
        promise.set_value(20 + 22);
    });
    std::cout << "jthread future result = " << result.get() << "\n";

    // 2. wait_for checks readiness without blocking forever.
    auto delayed = std::async(std::launch::async, [] {
        return 7;
    });
    if (delayed.wait_for(std::chrono::seconds(1)) == std::future_status::ready) {
        std::cout << "delayed result = " << delayed.get() << "\n";
    }

    return 0;
}
