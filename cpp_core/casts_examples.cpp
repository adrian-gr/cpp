// Core cast examples
// Compile with: g++ -std=c++11 -O2 cpp_core/casts_examples.cpp -o bin/core_casts && ./bin/core_casts

#include <iostream>

// Tips:
// - Prefer implicit conversions or static_cast when the conversion is well understood.
// - Use dynamic_cast only for checked navigation in a polymorphic hierarchy.
// - Avoid reinterpret_cast unless interfacing with a low-level API requires it.
// - Never use const_cast to mutate an object that was originally declared const.

struct Base { virtual ~Base() {} };
struct Derived : Base { int value = 42; };

int main() {
    std::cout << "Core cast examples:\n";
    double decimal = 3.75;
    int number = static_cast<int>(decimal);
    Base* base = new Derived;
    Derived* derived = dynamic_cast<Derived*>(base);
    std::cout << "static cast = " << number << "\n";
    std::cout << "dynamic cast value = " << (derived ? derived->value : 0) << "\n";
    delete base;
    return 0;
}
