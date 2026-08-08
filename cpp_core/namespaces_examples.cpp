// Core namespaces and translation units examples
// Compile with: g++ -std=c++11 -O2 cpp_core/namespaces_examples.cpp -o bin/core_namespaces && ./bin/core_namespaces

#include <iostream>
#include "namespaces_examples.h"

// Tips:
// - Put related declarations in a namespace to avoid global-name collisions.
// - Keep declarations in headers and definitions in source files.
// - Avoid using-directives in headers because they leak names to every includer.
// - Use include guards or pragma once for headers.

namespace core_examples {
int meaningOfLife() {
    return 42;
}
}

int main() {
    std::cout << "Core namespaces examples:\n";
    std::cout << "namespaced value = " << core_examples::meaningOfLife() << "\n";
    return 0;
}
