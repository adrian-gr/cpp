// Core const correctness examples
// Compile with: g++ -std=c++11 -O2 cpp_core/const_correctness_examples.cpp -o bin/core_const_correctness && ./bin/core_const_correctness

#include <iostream>
#include <string>

// Tips:
// - Mark methods const when they do not change observable object state.
// - Pass read-only objects by const reference when ownership is not needed.
// - Use const to make invalid mutation visible to the compiler.
// - Treat const correctness as part of an API's contract.

class Profile {
public:
    explicit Profile(std::string name) : name_(std::move(name)) {}
    const std::string& name() const { return name_; }
    void rename(std::string name) { name_ = std::move(name); }
private:
    std::string name_;
};

void printName(const Profile& profile) {
    std::cout << profile.name() << "\n";
}

int main() {
    std::cout << "Core const correctness examples:\n";
    const Profile readOnly("Ada");
    printName(readOnly);
    Profile editable("Grace");
    editable.rename("Grace Hopper");
    printName(editable);
    return 0;
}
