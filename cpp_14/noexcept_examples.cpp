// C++14 noexcept examples
// Reference: https://en.cppreference.com/w/cpp/language/noexcept_spec
// Compile with: g++ -std=c++14 -O2 cpp_14/noexcept_examples.cpp -o bin/cpp14_noexcept && ./bin/cpp14_noexcept

#include <iostream>
#include <string>
#include <utility>

// Problem solved: lets generic move operations propagate accurate exception guarantees.
// Before C++14: exception behavior was often duplicated manually or left undocumented.
// Tips for C++14 noexcept usage:
// - Use conditional noexcept to propagate a member or operation's exception guarantee.
// - noexcept is useful for generic code that should preserve the properties of its types.
// - Prefer noexcept over the deprecated dynamic exception specifications from older C++.
// - Test the condition with noexcept(expression) when selecting an implementation.

struct Message {
    std::string text;

    Message(const char* value) : text(value) {}
    Message(Message&& other) noexcept : text(std::move(other.text)) {}
    Message& operator=(Message&& other) noexcept {
        text = std::move(other.text);
        return *this;
    }
};

template <typename T>
void moveInto(T& target, T&& source) noexcept(noexcept(T(std::move(source)))) {
    target = T(std::move(source));
}

int main() {
    std::cout << "C++14 noexcept examples:\n";

    // 1. A move constructor can advertise that moving is non-throwing.
    static_assert(noexcept(Message(std::declval<Message&&>())),
                  "Message move construction should be noexcept");
    Message original("hello");
    Message moved(std::move(original));
    std::cout << "moved message = " << moved.text << "\n";

    // 2. Generic code can conditionally propagate a type's guarantee.
    Message destination("empty");
    static_assert(noexcept(moveInto(destination, std::move(moved))),
                  "moveInto should be noexcept for Message");
    moveInto(destination, std::move(moved));
    std::cout << "destination = " << destination.text << "\n";

    return 0;
}
