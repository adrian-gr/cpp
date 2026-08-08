// Core copy and move semantics examples
// Compile with: g++ -std=c++11 -O2 cpp_core/copy_move_examples.cpp -o bin/core_copy_move && ./bin/core_copy_move

#include <iostream>
#include <string>
#include <utility>

// Tips:
// - Follow the Rule of Zero when members already manage their own resources.
// - Define copy operations for independent duplication and move operations for transfer.
// - Mark move operations noexcept when they cannot throw.
// - A moved-from object must remain valid, but its old value should not be assumed.

class Message {
public:
    explicit Message(std::string text) : text_(std::move(text)) {}
    Message(const Message& other) : text_(other.text_) { std::cout << "copy\n"; }
    Message(Message&& other) noexcept : text_(std::move(other.text_)) { std::cout << "move\n"; }
    Message& operator=(const Message& other) { text_ = other.text_; return *this; }
    Message& operator=(Message&& other) noexcept { text_ = std::move(other.text_); return *this; }
    const std::string& text() const { return text_; }
private:
    std::string text_;
};

int main() {
    std::cout << "Core copy and move examples:\n";
    Message original("original");
    Message copied = original;
    Message moved = std::move(original);
    std::cout << "copied = " << copied.text() << ", moved = " << moved.text() << "\n";
    return 0;
}
