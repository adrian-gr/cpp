// Proxy pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/proxy_examples.cpp -o bin/proxy_example && ./bin/proxy_example

#include <iostream>
#include <memory>
#include <string>
#include <utility>

// Description:
// Proxy provides a substitute for another object and controls access to it.
// The proxy implements the same interface as the real subject.
//
// Use Proxy when:
// - access requires authorization or validation;
// - an object is expensive to create or access;
// - remote, virtual, or logging behavior should be transparent to clients.
//
// Avoid Proxy when:
// - access control or deferred work is not needed;
// - the extra indirection makes a simple object harder to use;
// - the proxy would duplicate substantial business logic.
//
// Tips:
// - Keep the proxy's interface identical to the subject's interface.
// - Keep access-control policy in the proxy and document behavior in the real subject.
// - Use dependency injection so the proxy can wrap alternate subject implementations.

class Document {
public:
    virtual ~Document() = default;
    virtual void display() const = 0;
};

class RealDocument final : public Document {
public:
    explicit RealDocument(std::string filename)
        : filename_(std::move(filename)) {
        std::cout << "loading document: " << filename_ << "\n";
    }

    void display() const override {
        std::cout << "displaying document: " << filename_ << "\n";
    }

private:
    std::string filename_;
};

class ProtectedDocumentProxy final : public Document {
public:
    ProtectedDocumentProxy(std::string filename, bool canView)
        : filename_(std::move(filename)), canView_(canView) {}

    void display() const override {
        if (!canView_) {
            std::cout << "access denied: " << filename_ << "\n";
            return;
        }

        if (!document_) {
            document_ = std::make_unique<RealDocument>(filename_);
        }
        document_->display();
    }

private:
    std::string filename_;
    bool canView_;
    mutable std::unique_ptr<RealDocument> document_;
};

void preview(const Document& document) {
    document.display();
}

int main() {
    std::cout << "Proxy pattern example:\n";

    ProtectedDocumentProxy deniedDocument("financial-report.pdf", false);
    ProtectedDocumentProxy allowedDocument("public-report.pdf", true);

    preview(deniedDocument);
    preview(allowedDocument);
    preview(allowedDocument);

    return 0;
}
