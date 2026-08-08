// Core CRTP (Curiously Recurring Template Pattern) examples
// Reference: https://en.cppreference.com/w/cpp/language/crtp
// Compile with: g++ -std=c++11 -O2 cpp_core/crtp_examples.cpp -o bin/core_crtp && ./bin/core_crtp

#include <iostream>

// Problem solved: provides reusable base behavior with static dispatch and no virtual-call overhead.
// Before CRTP: code used virtual functions for runtime polymorphism or duplicated helper functions.
// Tips for CRTP:
// - Use CRTP when the derived type is known at compile time and runtime substitution is unnecessary.
// - Keep the base class focused on behavior shared by its derived types.
// - Use virtual functions instead when objects must be handled through a common runtime interface.
// - CRTP can make types tightly coupled and error messages harder to read, so use it deliberately.

template <typename Derived>
class Printable {
public:
    void print() const {
        static_cast<const Derived&>(*this).printImplementation();
    }
};

class Number : public Printable<Number> {
public:
    explicit Number(int value) : value_(value) {}

    void printImplementation() const {
        std::cout << "Number = " << value_ << "\n";
    }

private:
    int value_;
};

class Text : public Printable<Text> {
public:
    explicit Text(const char* value) : value_(value) {}

    void printImplementation() const {
        std::cout << "Text = " << value_ << "\n";
    }

private:
    const char* value_;
};

template <typename Derived>
void printTwice(const Printable<Derived>& value) {
    const Derived& derived = static_cast<const Derived&>(value);
    derived.printImplementation();
    derived.printImplementation();
}

int main() {
    std::cout << "Core CRTP examples:\n";

    Number number(42);
    Text text("compile-time dispatch");
    number.print();
    text.print();
    printTwice(number);

    return 0;
}
