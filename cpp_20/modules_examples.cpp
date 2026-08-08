// C++20 module consumer example
// Reference: https://en.cppreference.com/w/cpp/language/modules
// Build with GCC 13:
//   g++ -std=c++20 -fmodules-ts -c cpp_20/math_module.cpp -o bin/math_module.o
//   g++ -std=c++20 -fmodules-ts cpp_20/modules_examples.cpp bin/math_module.o -o bin/cpp20_modules
//   ./bin/cpp20_modules

#include <iostream>

// Problem solved: imports a compiled interface without preprocessing and copying a header's contents.
// Before C++20: consumers included headers, increasing coupling and repeated compilation work.

import math_module;

int main() {
    std::cout << "C++20 modules examples:\n";

    // Imported declarations are available without textual inclusion.
    constexpr int squared = square(6);
    constexpr int sum = add(20, 22);
    static_assert(squared == 36);
    static_assert(sum == 42);

    std::cout << "square(6) = " << squared << "\n";
    std::cout << "add(20, 22) = " << sum << "\n";

    return 0;
}
