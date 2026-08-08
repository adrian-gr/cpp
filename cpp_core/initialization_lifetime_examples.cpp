// Core initialization order and lifetime examples
// Compile with: g++ -std=c++11 -O2 cpp_core/initialization_lifetime_examples.cpp -o bin/core_initialization_lifetime && ./bin/core_initialization_lifetime

#include <iostream>
#include <string>

// Tips:
// - Members initialize in declaration order, not the order written in the initializer list.
// - Initialize members directly instead of assigning to them in the constructor body.
// - Keep objects in the smallest scope that satisfies their lifetime needs.
// - Be careful when returning references or pointers to local objects.

class Service {
public:
    Service() : name_("service"), length_(name_.size()) {}
    void print() const { std::cout << name_ << " length = " << length_ << "\n"; }
private:
    std::string name_;
    std::size_t length_;
};

Service makeService() {
    Service service;
    return service;
}

int main() {
    std::cout << "Core initialization and lifetime examples:\n";
    Service service = makeService();
    service.print();
    {
        Service scoped = makeService();
        scoped.print();
    }
    std::cout << "scoped object destroyed\n";
    return 0;
}
