// C++11 noexcept examples
// Reference: https://en.cppreference.com/w/cpp/language/noexcept_spec
// Compile with: g++ -std=c++11 -O2 cpp_11/noexcept_examples.cpp -o bin/cpp11_noexcept && ./bin/cpp11_noexcept

#include <iostream>
#include <utility>

// Problem solved: declares non-throwing guarantees that generic code and the optimizer can inspect.
// Before C++11: code used deprecated dynamic exception specifications or undocumented assumptions.
// Tips for C++11 noexcept usage:
// - Mark a function noexcept when it is guaranteed not to throw exceptions.
// - Use noexcept on move constructors and move operations when they cannot throw.
// - A noexcept function that lets an exception escape calls std::terminate.
// - Use the conditional form noexcept(expression) when the guarantee depends on a type.
// - Do not mark a function noexcept merely to suppress compiler warnings.

void safeMessage() noexcept {
    std::cout << "safeMessage does not throw\n";
}

void mayThrow(bool shouldThrow) {
    if (shouldThrow) {
        throw 1;
    }
}

template <typename T>
void swapValues(T& left, T& right) noexcept(noexcept(std::swap(left, right))) {
    std::swap(left, right);
}

int main() {
    std::cout << "C++11 noexcept examples:\n";

    // 1. Query whether a function is declared noexcept.
    static_assert(noexcept(safeMessage()), "safeMessage should be noexcept");
    static_assert(!noexcept(mayThrow(false)), "mayThrow should not be noexcept");
    safeMessage();

    // 2. Conditional noexcept follows the exception guarantee of std::swap.
    int first = 1;
    int second = 2;
    static_assert(noexcept(swapValues(first, second)),
                  "swapping integers should be noexcept");
    swapValues(first, second);
    std::cout << "swapped values = " << first << ", " << second << "\n";

    // 3. A noexcept function must not allow an exception to escape.
    try {
        mayThrow(false);
        std::cout << "mayThrow completed without an exception\n";
    } catch (...) {
        std::cout << "caught exception\n";
    }

    return 0;
}
