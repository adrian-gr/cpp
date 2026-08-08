// Core undefined behavior avoidance examples
// Compile with: g++ -std=c++11 -O2 -Wall -Wextra cpp_core/undefined_behavior_examples.cpp -o bin/core_undefined_behavior && ./bin/core_undefined_behavior

#include <array>
#include <iostream>
#include <limits>

// Tips:
// - Compile with warnings and run sanitizers while learning and debugging.
// - Check bounds, pointer validity, arithmetic limits, and object lifetimes explicitly.
// - Treat undefined behavior as a correctness failure, not as an optimization detail.
// - Prefer checked standard-library operations when invalid input is possible.

int main() {
    std::cout << "Core undefined behavior avoidance examples:\n";

    // Bounds-checked access avoids reading outside the array.
    std::array<int, 3> values{{1, 2, 3}};
    std::size_t index = 2;
    if (index < values.size()) {
        std::cout << "checked value = " << values[index] << "\n";
    }

    // Check before signed arithmetic that could overflow.
    int left = std::numeric_limits<int>::max();
    int right = 1;
    bool safeToAdd = right <= 0 || left <= std::numeric_limits<int>::max() - right;
    std::cout << "addition is safe = " << std::boolalpha << safeToAdd << "\n";

    // Avoid dereferencing null and dangling pointers; use nullptr and clear ownership.
    int* pointer = nullptr;
    std::cout << "null pointer handled = " << (pointer == nullptr) << "\n";
    return 0;
}
