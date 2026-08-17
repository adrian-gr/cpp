// Composite pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/composite/composite_examples.cpp -o bin/composite_example && ./bin/composite_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Composite lets you compose objects into tree structures and then work with
// individual objects and compositions of objects uniformly through a common
// interface. A client calling an operation on a leaf or on a whole subtree uses
// exactly the same interface either way.
//
// Use Composite when:
// - you need to represent part-whole hierarchies of objects, like a tree;
// - client code should treat individual objects and compositions of objects
//   uniformly, without special-casing "is this a single item or a group?";
// - operations need to apply recursively across an entire structure, and pushing
//   that recursion into the objects themselves is simpler than the client managing it.
//
// Avoid Composite when:
// - the structure is not naturally hierarchical — forcing a tree onto flat data
//   just to use this pattern adds complexity without benefit;
// - leaves and composites need such different interfaces that a single common
//   interface would end up with awkward or meaningless methods on one side;
// - the tree is small, fixed, and simple enough that direct handling of each case
//   is clearer than the uniform-interface abstraction.
//
// Tips:
// - Keep the component interface focused on operations that make sense for both
//   leaves and composites; avoid leaking container-only operations into leaves.
// - Let composites delegate operations to their children recursively.
// - Consider whether child-management methods (add/remove) belong on the shared
//   interface or only on the composite, depending on how uniform you need traversal to be.

class FileSystemComponent {
public:
    virtual ~FileSystemComponent() = default;
    virtual void display(int indent = 0) const = 0;
    virtual long size() const = 0;
};

class File final : public FileSystemComponent {
public:
    File(std::string name, long sizeInBytes) : name_(std::move(name)), sizeInBytes_(sizeInBytes) {}

    void display(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "- " << name_ << " (" << sizeInBytes_ << " bytes)\n";
    }

    long size() const override {
        return sizeInBytes_;
    }

private:
    std::string name_;
    long sizeInBytes_;
};

class Directory final : public FileSystemComponent {
public:
    explicit Directory(std::string name) : name_(std::move(name)) {}

    void add(std::shared_ptr<FileSystemComponent> component) {
        children_.push_back(std::move(component));
    }

    void display(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "+ " << name_ << "/\n";
        for (const auto& child : children_) {
            child->display(indent + 2);
        }
    }

    long size() const override {
        long total = 0;
        for (const auto& child : children_) {
            total += child->size();
        }
        return total;
    }

private:
    std::string name_;
    std::vector<std::shared_ptr<FileSystemComponent>> children_;
};

int main() {
    std::cout << "Composite pattern example:\n";

    auto root = std::make_shared<Directory>("project");
    auto src = std::make_shared<Directory>("src");
    src->add(std::make_shared<File>("main.cpp", 1200));
    src->add(std::make_shared<File>("utils.cpp", 800));

    auto docs = std::make_shared<Directory>("docs");
    docs->add(std::make_shared<File>("README.md", 300));

    root->add(src);
    root->add(docs);
    root->add(std::make_shared<File>("LICENSE", 150));

    root->display();
    std::cout << "total size: " << root->size() << " bytes\n";

    return 0;
}