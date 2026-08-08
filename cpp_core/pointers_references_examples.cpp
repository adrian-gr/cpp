// Core pointers and references examples
// Compile with: g++ -std=c++11 -O2 cpp_core/pointers_references_examples.cpp -o bin/core_pointers_references && ./bin/core_pointers_references

#include <iostream>

// Tips:
// - Use references for required aliases and pointers when absence is meaningful.
// - Check pointers before dereferencing and prefer nullptr over null integer literals.
// - Document whether a pointer owns an object or only observes it.
// - Prefer standard smart pointers for ownership.

void increment(int& value) { ++value; }
void reset(int* value) { if (value) *value = 0; }

int main() {
    std::cout << "Core pointers and references examples:\n";
    int value = 10;
    increment(value);
    std::cout << "reference update = " << value << "\n";
    reset(&value);
    std::cout << "pointer update = " << value << "\n";
    reset(nullptr);
    return 0;
}
