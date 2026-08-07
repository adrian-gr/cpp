// C++17 thread examples
// Compile with: g++ -std=c++17 -O2 -pthread cpp_17/thread_examples.cpp -o bin/cpp17_thread && ./bin/cpp17_thread

#include <algorithm>
#include <execution>
#include <iostream>
#include <numeric>
#include <vector>

// Tips for C++17 thread usage:
// - Execution policies let standard algorithms express whether work may run in parallel.
// - Use std::execution::par only when the callable is safe for concurrent invocation.
// - Do not rely on a parallel policy to guarantee that multiple threads are used.
// - Avoid unsynchronized output or shared mutation inside a parallel algorithm.

int main() {
    std::cout << "C++17 thread examples:\n";

    // 1. A parallel execution policy permits concurrent algorithm execution.
    std::vector<int> values(8, 1);
    std::for_each(std::execution::par, values.begin(), values.end(), [](int& value) {
        value *= 2;
    });
    std::cout << "parallel values sum = "
              << std::accumulate(values.begin(), values.end(), 0) << "\n";

    // 2. A sequential policy is useful when the same algorithm must be predictable.
    std::for_each(std::execution::seq, values.begin(), values.end(), [](int& value) {
        value += 1;
    });
    std::cout << "sequential follow-up sum = "
              << std::accumulate(values.begin(), values.end(), 0) << "\n";

    return 0;
}
