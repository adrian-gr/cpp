// C++20 coroutine examples
// Reference: https://en.cppreference.com/w/cpp/language/coroutines
// Compile with: g++ -std=c++20 -O2 -Wall -Wextra cpp_20/coroutines_examples.cpp -o bin/cpp20_coroutines && ./bin/cpp20_coroutines

#include <coroutine>
#include <exception>
#include <iostream>
#include <optional>
#include <utility>

// Problem solved: suspends and resumes functions without hand-written state machines or callbacks.
// Before C++20: generators and async flows required callbacks, futures, or manually stored state.
// Tips for C++20 coroutine usage:
// - Coroutines suspend and resume execution; they are not automatically threads.
// - Use co_yield for generators, co_await for asynchronous operations, and co_return for results.
// - The promise type defines how a coroutine starts, suspends, produces values, and finishes.
// - Ensure coroutine frames are destroyed and that referenced objects outlive suspended work.
// - Use an established coroutine library for production async I/O and cancellation policies.

template <typename T>
class Generator {
public:
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;

    explicit Generator(Handle handle) : handle_(handle) {}
    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept
        : handle_(std::exchange(other.handle_, {})) {}

    ~Generator() {
        if (handle_) {
            handle_.destroy();
        }
    }

    class iterator {
    public:
        explicit iterator(Handle handle) : handle_(handle) {}

        iterator& operator++() {
            handle_.resume();
            return *this;
        }

        const T& operator*() const {
            return *handle_.promise().current;
        }

        bool operator==(std::default_sentinel_t) const {
            return !handle_ || handle_.done();
        }

    private:
        Handle handle_;
    };

    iterator begin() {
        if (handle_) {
            handle_.resume();
        }
        return iterator(handle_);
    }

    std::default_sentinel_t end() const {
        return {};
    }

    struct promise_type {
        std::optional<T> current;

        Generator get_return_object() {
            return Generator(Handle::from_promise(*this));
        }

        std::suspend_always initial_suspend() const noexcept {
            return {};
        }

        std::suspend_always final_suspend() const noexcept {
            return {};
        }

        std::suspend_always yield_value(T value) noexcept {
            current = std::move(value);
            return {};
        }

        void return_void() const noexcept {}

        void unhandled_exception() const noexcept {
            std::terminate();
        }
    };

private:
    Handle handle_;
};

Generator<int> countTo(int limit) {
    for (int value = 1; value <= limit; ++value) {
        co_yield value;
    }
    co_return;
}

int main() {
    std::cout << "C++20 coroutine examples:\n";

    // A generator suspends at each co_yield and resumes on the next iteration.
    std::cout << "generated values:";
    for (int value : countTo(5)) {
        std::cout << ' ' << value;
    }
    std::cout << "\n";

    return 0;
}
