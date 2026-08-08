// C++11 smart pointer examples
// Reference: https://en.cppreference.com/w/cpp/memory
// Compile with: g++ -std=c++11 -O2 cpp_11/smart_pointers_examples.cpp -o bin/cpp11_smart_pointers && ./bin/cpp11_smart_pointers

#include <iostream>
#include <memory>
#include <string>

// Problem solved: gives dynamically allocated objects explicit ownership and automatic cleanup.
// Before C++11: code used raw new/delete, which made leaks and double deletion easy.
// Tips for C++11 smart pointer usage:
// - Prefer unique_ptr for one clear owner and shared_ptr only for shared ownership.
// - Use make_shared where practical and avoid owning raw pointers.
// - Use weak_ptr to observe a shared object without extending its lifetime.
// - Do not use shared_ptr merely as a universal replacement for unique_ptr.

struct Resource {
    explicit Resource(std::string name) : name(std::move(name)) {
        std::cout << "created " << this->name << "\n";
    }

    ~Resource() {
        std::cout << "destroyed " << name << "\n";
    }

    std::string name;
};

int main() {
    std::cout << "C++11 smart pointer examples:\n";

    // 1. unique_ptr expresses exclusive ownership.
    std::unique_ptr<Resource> exclusive(new Resource("exclusive"));
    std::unique_ptr<Resource> moved = std::move(exclusive);
    std::cout << "exclusive pointer is empty = " << std::boolalpha
              << !exclusive << "\n";

    // 2. shared_ptr supports shared ownership through a reference count.
    std::shared_ptr<Resource> shared = std::make_shared<Resource>("shared");
    std::shared_ptr<Resource> copy = shared;
    std::cout << "shared owners = " << shared.use_count() << "\n";

    // 3. weak_ptr observes shared ownership without keeping the object alive.
    std::weak_ptr<Resource> observer = shared;
    if (auto locked = observer.lock()) {
        std::cout << "observed resource = " << locked->name << "\n";
    }

    return 0;
}
