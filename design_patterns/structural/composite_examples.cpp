// Composite pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/composite_examples.cpp -o bin/composite_example && ./bin/composite_example

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Description:
// Composite composes objects into tree structures and lets clients treat
// individual objects and groups of objects through the same interface.
//
// Use Composite when:
// - objects naturally form part-whole hierarchies;
// - clients should work with leaves and groups uniformly;
// - operations can be expressed consistently for both individual and grouped objects.
//
// Avoid Composite when:
// - the hierarchy has no meaningful parent-child relationship;
// - leaf and composite operations are fundamentally different;
// - exposing child-management operations would make the component interface unclear.
//
// Tips:
// - Keep the common component interface small and meaningful for every node.
// - Let composites own their children so tree lifetime is explicit.
// - Use composition to build trees while keeping client code independent of concrete nodes.

class FileSystemEntry {
public:
    virtual ~FileSystemEntry() = default;
    virtual void print(int depth = 0) const = 0;
};

class File final : public FileSystemEntry {
public:
    explicit File(std::string name)
        : name_(std::move(name)) {}

    void print(int depth = 0) const override {
        std::cout << std::string(depth * 2, ' ') << "- " << name_ << "\n";
    }

private:
    std::string name_;
};

class Directory final : public FileSystemEntry {
public:
    explicit Directory(std::string name)
        : name_(std::move(name)) {}

    void add(std::unique_ptr<FileSystemEntry> entry) {
        entries_.push_back(std::move(entry));
    }

    void print(int depth = 0) const override {
        std::cout << std::string(depth * 2, ' ') << "+ " << name_ << "\n";
        for (const auto& entry : entries_) {
            entry->print(depth + 1);
        }
    }

private:
    std::string name_;
    std::vector<std::unique_ptr<FileSystemEntry>> entries_;
};

int main() {
    std::cout << "Composite pattern example:\n";

    Directory project("project");

    auto source = std::make_unique<Directory>("src");
    source->add(std::make_unique<File>("main.cpp"));
    source->add(std::make_unique<File>("composite.cpp"));

    auto tests = std::make_unique<Directory>("tests");
    tests->add(std::make_unique<File>("composite_test.cpp"));

    project.add(std::move(source));
    project.add(std::move(tests));
    project.add(std::make_unique<File>("README.md"));

    // The client calls the same operation on a leaf or a composite.
    project.print();

    return 0;
}
