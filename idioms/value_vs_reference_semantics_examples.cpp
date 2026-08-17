// Value semantics vs reference semantics example
// Compile with: g++ -std=c++20 -O2 idioms/value_vs_reference_semantics_examples.cpp -o bin/value_vs_reference_example && ./bin/value_vs_reference_example

#include <iostream>
#include <string>
#include <vector>

// Description:
// Value semantics: assigning or passing an object copies its data. Each variable
// owns an independent instance; mutating one never affects another. This is C++'s
// default for plain objects (int, std::string, std::vector, user-defined classes
// without pointer members) passed or assigned by value.
//
// Reference semantics: assigning or passing an object shares access to the same
// underlying data. Mutating through one variable is visible through every other
// variable that refers to the same object. In C++ this is opt-in, via references
// (&), pointers (*), or shared ownership types (std::shared_ptr).
//
// Neither is "better" — they answer different questions:
// - Value semantics: "does this variable own an independent copy of the data?"
// - Reference semantics: "do multiple variables observe the same shared data?"
//
// Cost implications:
// - Value semantics can mean real copying cost — deep-copying a large
//   std::vector or std::string is O(n), not free, every time it happens.
// - Move semantics (C++11+) recovers most of that cost for temporaries and
//   objects you're done with: a move steals the internal buffer instead of
//   copying it, in O(1) instead of O(n).
// - Reference semantics avoids copying entirely, but couples the referring
//   variables together — a mutation through one is visible through all of them,
//   which can be exactly what you want, or a hard-to-trace bug.
// - Passing by const reference (const T&) gets you the cost profile of reference
//   semantics (no copy) while keeping the safety of value semantics (the callee
//   can't mutate the caller's object) — this is the default choice for passing
//   non-trivial objects into functions that only need to read them.

struct Point {
    double x;
    double y;
};

void demonstrateValueSemantics() {
    std::cout << "-- value semantics --\n";

    Point a{1.0, 2.0};
    Point b = a;          // copies a's data into b; a and b are now independent
    b.x = 99.0;

    std::cout << "a.x = " << a.x << ", b.x = " << b.x << "\n";
    // a.x is unaffected by the change to b.x — each owns its own data.

    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = v1;   // deep copy: v2 gets its own buffer of elements
    v2.push_back(4);

    std::cout << "v1.size() = " << v1.size() << ", v2.size() = " << v2.size() << "\n";
    // v1 is untouched; the copy was fully independent, at the cost of an O(n) copy.
}

void demonstrateReferenceSemantics() {
    std::cout << "-- reference semantics --\n";

    Point a{1.0, 2.0};
    Point& refToA = a;    // refToA is another name for a, not a copy
    refToA.x = 99.0;

    std::cout << "a.x = " << a.x << ", refToA.x = " << refToA.x << "\n";
    // a.x changed too — refToA and a are the same object.

    Point* ptrToA = &a;
    ptrToA->y = 42.0;
    std::cout << "a.y = " << a.y << "\n";
    // Same story through a pointer: mutating *ptrToA mutates a directly.
}

// Passing by value: the callee gets its own copy. Safe, but costs a copy for
// anything non-trivial (here, a whole std::vector's worth of elements).
void modifyByValue(std::vector<int> data) {
    data.push_back(-1);
    std::cout << "inside modifyByValue: size = " << data.size() << "\n";
}

// Passing by reference: no copy, and the callee can mutate the caller's object.
void modifyByReference(std::vector<int>& data) {
    data.push_back(-1);
    std::cout << "inside modifyByReference: size = " << data.size() << "\n";
}

// Passing by const reference: no copy AND the callee can't mutate the caller's
// object. This is the usual default for read-only access to non-trivial types.
void readByConstReference(const std::vector<int>& data) {
    std::cout << "inside readByConstReference: size = " << data.size() << "\n";
    // data.push_back(-1);  // would not compile: data is const here
}

int main() {
    std::cout << "Value vs reference semantics example:\n";

    demonstrateValueSemantics();
    demonstrateReferenceSemantics();

    std::cout << "-- passing conventions and their cost --\n";
    std::vector<int> numbers = {1, 2, 3};

    modifyByValue(numbers);
    std::cout << "after modifyByValue, caller's size = " << numbers.size() << "\n";
    // Caller's vector is untouched — modifyByValue mutated its own copy.
    // Cost: one full O(n) copy of `numbers` was made to call this function.

    modifyByReference(numbers);
    std::cout << "after modifyByReference, caller's size = " << numbers.size() << "\n";
    // Caller's vector DID change — no copy was made; the callee shared the caller's data.

    readByConstReference(numbers);
    // No copy, and the compiler enforces that this function can't mutate `numbers`.

    return 0;
}