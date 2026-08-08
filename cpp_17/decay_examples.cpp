// C++17 std::decay examples
// Reference: https://en.cppreference.com/w/cpp/types/decay
// Compile with: g++ -std=c++17 -O2 cpp_17/decay_examples.cpp -o bin/cpp17_decay && ./bin/cpp17_decay

#include <iostream>
#include <type_traits>

// Problem solved: makes common decay transformations concise with the _t alias.
// Before C++17: code used the longer typename std::decay<T>::type spelling.
// Tips for std::decay:
// - Prefer decay_t when the transformed type is used directly.
// - Decay is useful for storing callable objects and arguments by value.
// - Be aware that decay changes arrays and functions into pointers.

template <typename Callable>
std::decay_t<Callable> storeCallable(Callable&& callable) {
    return std::forward<Callable>(callable);
}

int main() {
    std::cout << "C++17 std::decay examples:\n";

    auto lambda = [](int value) {
        return value + 1;
    };
    auto stored = storeCallable(lambda);
    static_assert(std::is_same_v<decltype(stored), decltype(lambda)>);
    std::cout << "stored callable result = " << stored(4) << "\n";

    const int value = 10;
    static_assert(std::is_same_v<std::decay_t<decltype(value)>, int>);
    std::cout << "decayed const value = " << std::decay_t<decltype(value)>(value) << "\n";

    return 0;
}
