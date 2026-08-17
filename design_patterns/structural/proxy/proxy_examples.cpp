// Proxy pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/proxy/proxy_examples.cpp -o bin/proxy_example && ./bin/proxy_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Proxy provides a surrogate or placeholder for another object to control access
// to it. The proxy implements the same interface as the real object, so clients
// interact with the proxy exactly as they would with the real thing, while the
// proxy adds its own logic — lazy loading, access control, logging, caching,
// remote communication — around delegating to (or creating) the real object.
//
// Use Proxy when:
// - object creation or initialization is expensive, and you want to defer it until
//   the object is actually needed (a virtual/lazy-loading proxy);
// - you need to control access to an object — permission checks before allowing an
//   operation to proceed (a protection proxy);
// - the real object lives elsewhere (a different process or machine) and the proxy
//   stands in locally, handling the communication details (a remote proxy);
// - you want to add behavior like caching or logging around calls to an object
//   without modifying the object itself.
//
// Avoid Proxy when:
// - there's no real need to control, defer, or intercept access — direct use of
//   the real object is simpler and the indirection buys nothing;
// - the added logic is really about extending behavior with new responsibilities
//   rather than controlling access — that's a better fit for Decorator;
// - the overhead of an extra indirection layer isn't justified by the access
//   control, laziness, or remoteness concern it would address.
//
// Tips:
// - Keep the proxy implementing the exact same interface as the real subject, so
//   clients can't tell (and don't need to) whether they're talking to the real
//   object or the proxy.
// - Keep proxy-specific logic (lazy init, permission checks, logging) separate
//   from the real subject's own logic; the proxy should not duplicate business logic.
// - Distinguish Proxy's intent (controlling access) from Decorator's (adding
//   responsibilities) even though the structures look similar.

class Image {
public:
    virtual ~Image() = default;
    virtual void display() const = 0;
};

// Real subject: expensive to construct (simulated by the constructor's own work).
class HighResImage final : public Image {
public:
    explicit HighResImage(std::string filename) : filename_(std::move(filename)) {
        std::cout << "loading high-res image from disk: " << filename_ << "\n";
    }

    void display() const override {
        std::cout << "displaying " << filename_ << "\n";
    }

private:
    std::string filename_;
};

// Proxy: defers creating the expensive HighResImage until display() is first called.
class ImageProxy final : public Image {
public:
    explicit ImageProxy(std::string filename) : filename_(std::move(filename)) {}

    void display() const override {
        if (!realImage_) {
            realImage_ = std::make_unique<HighResImage>(filename_);
        }
        realImage_->display();
    }

private:
    std::string filename_;
    mutable std::unique_ptr<HighResImage> realImage_;
};

void showGallery(const std::vector<std::unique_ptr<Image>>& images) {
    std::cout << "gallery created (no images loaded yet)\n";
    for (const auto& image : images) {
        image->display();
    }
}

int main() {
    std::cout << "Proxy pattern example:\n";

    std::vector<std::unique_ptr<Image>> images;
    images.push_back(std::make_unique<ImageProxy>("vacation.jpg"));
    images.push_back(std::make_unique<ImageProxy>("family.jpg"));

    showGallery(images);

    return 0;
}