// C++17 smart pointer examples
// Reference: https://en.cppreference.com/w/cpp/memory
// Compile with: g++ -std=c++17 -O2 cpp_17/smart_pointers_examples.cpp -o bin/cpp17_smart_pointers && ./bin/cpp17_smart_pointers

#include <iostream>
#include <memory>
#include <string>

// Problem solved: supports polymorphic ownership and views into shared objects without extra allocation.
// Before C++17: code used raw pointer aliases or custom ownership wrappers for these cases.
// Tips for C++17 smart pointer usage:
// - Use unique_ptr to own polymorphic objects when shared ownership is unnecessary.
// - Use the shared_ptr aliasing constructor only when the owner must outlive a subobject view.
// - Always use virtual destructors for polymorphic deletion through a base pointer.
// - Prefer make_shared for ordinary shared objects and weak_ptr for non-owning links.

struct Shape {
    virtual ~Shape() = default;
    virtual int area() const = 0;
};

struct Square : Shape {
    explicit Square(int side) : side(side) {}
    int area() const override { return side * side; }
    int side;
};

struct Document {
    std::string title;
    int pages;
};

int main() {
    std::cout << "C++17 smart pointer examples:\n";

    // 1. unique_ptr supports safe polymorphic ownership.
    std::unique_ptr<Shape> shape = std::make_unique<Square>(5);
    std::cout << "square area = " << shape->area() << "\n";

    // 2. An aliasing shared_ptr owns the document but points at one member.
    auto document = std::make_shared<Document>(Document{"C++", 120});
    std::shared_ptr<int> pageView(document, &document->pages);
    std::cout << "page view = " << *pageView
              << ", owners = " << document.use_count() << "\n";

    // 3. weak_ptr access is conditional and does not create ownership.
    std::weak_ptr<Document> observer = document;
    if (auto locked = observer.lock()) {
        std::cout << "observed title = " << locked->title << "\n";
    }

    return 0;
}
