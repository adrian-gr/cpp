// Core containers and algorithms examples
// Compile with: g++ -std=c++11 -O2 cpp_core/containers_algorithms_examples.cpp -o bin/core_containers_algorithms && ./bin/core_containers_algorithms

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

// Tips:
// - Prefer standard algorithms over handwritten loops when the intent matches an algorithm.
// - Keep iterator ranges valid for the entire algorithm call.
// - Use projections or predicates to make the operation explicit.
// - Check whether an algorithm reorders, removes, or invalidates elements.

int main() {
    std::cout << "Core containers and algorithms examples:\n";

    std::vector<int> values{5, 1, 4, 2, 3};
    std::sort(values.begin(), values.end());

    int total = std::accumulate(values.begin(), values.end(), 0);
    auto found = std::find(values.begin(), values.end(), 4);

    std::cout << "sorted values:";
    for (int value : values) {
        std::cout << ' ' << value;
    }
    std::cout << "\n";
    std::cout << "sum = " << total << "\n";
    std::cout << "contains 4 = " << std::boolalpha
              << (found != values.end()) << "\n";

    return 0;
}
