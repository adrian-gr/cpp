// Core C++ classes and RAII examples
// Compile with: g++ -std=c++11 -O2 cpp_core/classes_raii_examples.cpp -o bin/core_classes_raii && ./bin/core_classes_raii

#include <iostream>
#include <string>

// Tips:
// - Keep resources owned by objects so destruction is automatic at scope exit.
// - Make ownership explicit and disable copying when a resource has one owner.
// - Keep constructors responsible for establishing class invariants.

class Connection {
public:
    explicit Connection(std::string name) : name_(std::move(name)) {
        std::cout << "opened " << name_ << "\n";
    }

    ~Connection() {
        std::cout << "closed " << name_ << "\n";
    }

    Connection(const Connection&) = delete;
    Connection& operator=(const Connection&) = delete;

    void send(const std::string& message) const {
        std::cout << name_ << ": " << message << "\n";
    }

private:
    std::string name_;
};

void useConnection() {
    Connection connection("primary");
    connection.send("RAII controls the lifetime");
}

int main() {
    std::cout << "Core classes and RAII examples:\n";
    useConnection();
    std::cout << "scope finished\n";
    return 0;
}
