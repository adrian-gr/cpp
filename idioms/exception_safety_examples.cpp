// Exceptions and exception safety guarantees — basic, strong, and no-throw
// Compile with: g++ -std=c++20 -O2 idioms/exception_safety_examples.cpp -o bin/exception_safety_example && ./bin/exception_safety_example

#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

// Description:
// Exception safety guarantees describe what a piece of code promises about its
// own state IF an exception is thrown partway through it. There are four
// recognized levels, from weakest to strongest:
//
// - No guarantee: an exception may leave the program in a broken, inconsistent
//   state — leaked resources, corrupted invariants. Never acceptable for
//   library-quality code.
// - Basic guarantee: if an exception is thrown, no resources are leaked and the
//   object remains in SOME valid state — invariants hold — but that state is
//   otherwise unspecified. The object is safe to use and safe to destroy, but
//   you can't assume it's unchanged.
// - Strong guarantee: if an exception is thrown, the operation has NO EFFECT at
//   all — it's transactional. The object is left exactly as it was before the
//   call, as if the call had never been attempted. Achieved via patterns like
//   "build the new state fully off to the side, then commit with a no-throw
//   swap/move at the very end."
// - No-throw guarantee (noexcept): the operation is guaranteed to never throw,
//   full stop. Destructors, swap, and move operations should aim for this —
//   and the standard library actively depends on move operations being
//   noexcept to safely use them during operations like std::vector reallocation.
//
// RAII (see the earlier RAII example) is what makes the basic guarantee nearly
// free: if every resource is owned by an RAII type, stack unwinding during an
// exception automatically releases everything correctly, with no manual cleanup
// code required at each call site. The strong guarantee, when needed, is a
// design technique layered on top of that: do all the risky/throwing work on a
// temporary copy first, and only touch the real object via an operation that
// cannot fail (typically a swap or a noexcept move).
//
// Use the basic guarantee (the default target) when:
// - RAII already gives it to you for free — this should be the normal state of
//   almost all your code, achieved simply by not managing resources manually.
//
// Use the strong guarantee when:
// - a caller genuinely needs "either this fully succeeds, or nothing changed"
//   — e.g. modifying a data structure where a half-applied change would be
//   worse than no change at all (transactional updates, multi-step mutations).
// - it's cheap enough to implement (usually via copy-then-swap) that the extra
//   cost of building a temporary copy is worth the stronger promise.
//
// Use the no-throw guarantee (mark noexcept) when:
// - writing a destructor (destructors should essentially always be noexcept;
//   throwing from one during stack unwinding calls std::terminate);
// - writing move constructors/move assignment — noexcept here isn't just
//   documentation, it changes what the standard library does: std::vector only
//   uses your move constructor during reallocation if it's noexcept; otherwise
//   it falls back to copying, silently, to preserve ITS OWN exception safety
//   guarantee (see the demonstration below);
// - writing swap — a swap that can't throw is what makes strong-guarantee
//   implementations (copy-then-swap) actually strong.
//
// Avoid:
// - catching exceptions you can't meaningfully handle just to log-and-rethrow
//   without adding value, or swallowing them silently — both hide real
//   failures instead of dealing with them.
// - claiming a stronger guarantee than the code actually provides — that's
//   worse than an honest weaker guarantee, because callers will rely on a
//   promise you don't keep.

// ---------------------------------------------------------------------------
// Basic guarantee, essentially for free via RAII: if push_back throws (e.g.
// out of memory) partway through, no resources leak — items_'s own RAII
// cleans itself up correctly during stack unwinding. addWithLogging's own
// caller sees a valid (if partially updated) Inventory, never a corrupted one.
// ---------------------------------------------------------------------------
class Inventory {
public:
    void addWithLogging(const std::string& item) {
        std::cout << "adding: " << item << "\n";
        items_.push_back(item);   // if this throws, items_ remains a fully
                                    // valid (if unchanged) std::vector — RAII
                                    // guarantees that, not code written here.
        std::cout << "added: " << item << "\n";
    }

    const std::vector<std::string>& items() const { return items_; }

private:
    std::vector<std::string> items_;
};

// ---------------------------------------------------------------------------
// Strong guarantee via copy-then-swap: replaceAll either fully succeeds or
// leaves the object completely untouched — never partially updated. The
// technique: do all the risky work (which might throw) on a local copy, and
// only touch the real object via a swap, which is noexcept and therefore
// cannot itself introduce a failure at the point of commit.
// ---------------------------------------------------------------------------
class Catalog {
public:
    // Weaker (basic-guarantee-only) version, for contrast: mutates items_
    // directly, element by element. If validateAndTransform throws partway
      // through, items_ is left PARTIALLY updated — some entries transformed,
    // some not. Still safe (no leak, no corruption) but not transactional.
    void replaceAllBasicOnly(const std::vector<std::string>& newItems) {
        items_.clear();
        for (const auto& item : newItems) {
            items_.push_back(validateAndTransform(item));   // may throw
            // If this throws on the 3rd of 5 items, items_ now contains 2
            // transformed items and is missing the rest — a valid but
            // incomplete state. Basic guarantee: yes. Strong: no.
        }
    }

    // Strong-guarantee version: builds the ENTIRE new state in a local
    // variable first. If anything throws during that build, `this->items_`
    // has not been touched at all — the exception propagates with the object
    // completely unchanged. Only the final swap touches the real object, and
    // std::vector::swap is noexcept, so that step cannot itself fail.
    void replaceAllStrong(const std::vector<std::string>& newItems) {
        std::vector<std::string> transformed;   // built entirely off to the side
        transformed.reserve(newItems.size());
        for (const auto& item : newItems) {
            transformed.push_back(validateAndTransform(item));   // may throw
            // If this throws, `transformed` (a local) is destroyed normally
            // during unwinding, and items_ was NEVER touched — commit hasn't
            // happened yet.
        }
        items_.swap(transformed);   // noexcept: this line cannot throw, so
                                      // once we reach it, success is guaranteed.
    }

    const std::vector<std::string>& items() const { return items_; }

private:
    std::string validateAndTransform(const std::string& item) {
        if (item.empty()) {
            throw std::invalid_argument("item cannot be empty");
        }
        std::string result = item;
        for (char& c : result) c = static_cast<char>(std::toupper(c));
        return result;
    }

    std::vector<std::string> items_;
};

// ---------------------------------------------------------------------------
// No-throw guarantee: the move constructor is marked noexcept, which is a
// promise the compiler and the standard library actually rely on — not just
// documentation for human readers.
// ---------------------------------------------------------------------------
class Buffer {
public:
    explicit Buffer(size_t size) : size_(size), data_(new int[size]) {}
    ~Buffer() { delete[] data_; }

    Buffer(const Buffer& other) : size_(other.size_), data_(new int[other.size_]) {
        std::copy(other.data_, other.data_ + size_, data_);
    }

    // noexcept here is a promise: this operation cannot throw. That promise
    // is exactly what std::vector<Buffer> needs to safely use THIS move
    // constructor during reallocation, rather than falling back to copying.
    Buffer(Buffer&& other) noexcept : size_(other.size_), data_(other.data_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    size_t size() const { return size_; }

private:
    size_t size_;
    int* data_;
};

// A version WITHOUT noexcept on the move constructor, to demonstrate the
// consequence: std::vector cannot assume this move is safe to use during
// reallocation (a throwing move mid-reallocation could leave the vector in
// a corrupted state, violating ITS OWN strong exception-safety guarantee for
// push_back), so it silently falls back to copying instead of moving.
class BufferNoexceptMissing {
public:
    explicit BufferNoexceptMissing(size_t size) : size_(size), data_(new int[size]) {}
    ~BufferNoexceptMissing() { delete[] data_; }

    BufferNoexceptMissing(const BufferNoexceptMissing& other)
        : size_(other.size_), data_(new int[other.size_]) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "  (copy constructor ran)\n";
    }

    // No noexcept: this move constructor COULD throw as far as the compiler
    // can prove, even though this particular implementation never actually
    // does. std::vector has to assume the worst.
    BufferNoexceptMissing(BufferNoexceptMissing&& other) : size_(other.size_), data_(other.data_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "  (move constructor ran)\n";
    }

    size_t size() const { return size_; }

private:
    size_t size_;
    int* data_;
};

int main() {
    std::cout << "Exception safety guarantees example:\n";

    std::cout << "-- basic guarantee (via RAII) --\n";
    Inventory inventory;
    inventory.addWithLogging("widget");
    inventory.addWithLogging("gadget");
    std::cout << "inventory has " << inventory.items().size() << " items\n";
    // If push_back had thrown above, items_ would remain untouched and fully
    // valid — no manual try/catch or cleanup code was needed to guarantee that.

    std::cout << "\n-- strong guarantee (copy-then-swap) --\n";
    Catalog catalog;
    try {
        catalog.replaceAllStrong({"apple", "banana", ""});   // "" will throw
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << "\n";
    }
    std::cout << "catalog items after failed replaceAllStrong: "
              << catalog.items().size() << " (untouched, exactly as before the call)\n";

    catalog.replaceAllStrong({"apple", "banana", "cherry"});   // succeeds fully
    std::cout << "catalog items after successful replaceAllStrong: ";
    for (const auto& item : catalog.items()) std::cout << item << " ";
    std::cout << "\n";

    std::cout << "\n-- no-throw guarantee and its effect on std::vector --\n";

    std::cout << "with noexcept move constructor:\n";
    std::vector<Buffer> buffers;
    buffers.emplace_back(10);
    buffers.emplace_back(20);   // triggers reallocation; uses the noexcept
                                 // move constructor, cheaply, since it's safe to.

    std::cout << "without noexcept move constructor:\n";
    std::vector<BufferNoexceptMissing> unsafeBuffers;
    unsafeBuffers.emplace_back(10);
    unsafeBuffers.emplace_back(20);   // triggers reallocation; std::vector
                                       // falls back to COPYING existing
                                       // elements instead of moving them,
                                       // because it cannot prove the move
                                       // won't throw partway through.

    return 0;
}