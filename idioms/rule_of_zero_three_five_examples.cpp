// Rule of Zero / Three / Five example
// Compile with: g++ -std=c++20 -O2 idioms/rule_of_zero_three_five_examples.cpp -o bin/rule_of_zero_three_five_example && ./bin/rule_of_zero_three_five_example

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Description:
// When a class manages a resource directly (raw memory, a file handle, a raw
// pointer), the compiler-generated special member functions (copy constructor,
// copy assignment, destructor, move constructor, move assignment) are wrong by
// default — they only know how to copy/move/destroy each member individually,
// which corrupts or double-frees a manually-managed resource. The "Rule of Three"
// says: if you need to write ANY ONE of destructor / copy constructor / copy
// assignment, you almost certainly need to write all three, because needing one
// is evidence the class owns a resource the compiler can't handle automatically.
// The "Rule of Five" extends this to C++11's two new special members: move
// constructor and move assignment. Without them, a class you've made non-trivial
// falls back to copying even when a move was intended, silently losing the
// performance benefit (or, if you disabled copy, losing move availability
// entirely).
// The "Rule of Zero" is the preferred escape from all of this: design your class
// so it owns no resources DIRECTLY. Compose it out of members that are already
// RAII types (std::string, std::vector, std::unique_ptr, std::shared_ptr) — each
// of which already implements its own correct Rule-of-Five behavior. Then write
// NONE of the five special members yourself; the compiler-generated versions are
// correct by construction, because they just correctly copy/move/destroy each
// RAII member in turn.
//
// Use Rule of Three/Five when:
// - a class must manage a raw resource directly (a C API handle, raw new/delete,
//   a raw file descriptor) — usually only inside a small, focused wrapper class
//   whose entire purpose is to be that resource's RAII owner.
//
// Use Rule of Zero when:
// - a class's job is to model a domain concept, not manage a resource — in that
//   case, compose it from RAII-owning members and write no special members at all.
//
// Avoid:
// - writing a destructor or copy constructor "just in case" without an actual
//   resource-management reason — that alone forces you into Rule of Three/Five
//   territory for no benefit.
// - implementing only SOME of the five when a resource is genuinely owned —
//   partial implementations are a classic source of double-frees, leaks, and
//   silent copy-instead-of-move performance bugs.
//
// Tips:
// - Default to Rule of Zero. Reach for Rule of Five only in the rare class whose
//   entire job is owning one specific raw resource — everything else should
//   compose that class (or std::unique_ptr/vector/string) rather than duplicate it.
// = "= default" and "= delete" make intent explicit even for classes that don't
//   need custom logic, and document the design decision for future readers.

// ---------------------------------------------------------------------------
// Rule of Five: this class owns a raw resource (a heap-allocated buffer via new[])
// directly, so all five special members must be written correctly by hand.
// ---------------------------------------------------------------------------
class Buffer {
public:
    explicit Buffer(size_t size) : size_(size), data_(new int[size]) {
        std::cout << "Buffer: allocated " << size_ << " ints\n";
    }

    // 1. Destructor: releases the owned resource.
    ~Buffer() {
        delete[] data_;
        std::cout << "Buffer: freed\n";
    }

    // 2. Copy constructor: must deep-copy, not copy the pointer (or two Buffers
    //    would both try to delete[] the same memory).
    Buffer(const Buffer& other) : size_(other.size_), data_(new int[other.size_]) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "Buffer: deep-copied " << size_ << " ints\n";
    }

    // 3. Copy assignment: same deep-copy requirement, plus must release the
    //    left-hand side's existing resource first and handle self-assignment.
    Buffer& operator=(const Buffer& other) {
        if (this == &other) return *this;
        int* newData = new int[other.size_];
        std::copy(other.data_, other.data_ + other.size_, newData);
        delete[] data_;
        data_ = newData;
        size_ = other.size_;
        std::cout << "Buffer: copy-assigned " << size_ << " ints\n";
        return *this;
    }

    // 4. Move constructor: steals the other object's pointer instead of copying —
    //    O(1) instead of O(n) — and leaves the source in a safely destructible state.
    Buffer(Buffer&& other) noexcept : size_(other.size_), data_(other.data_) {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "Buffer: moved (stole pointer)\n";
    }

    // 5. Move assignment: same idea, but must also release the left-hand side's
    //    existing resource first.
    Buffer& operator=(Buffer&& other) noexcept {
        if (this == &other) return *this;
        delete[] data_;
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "Buffer: move-assigned\n";
        return *this;
    }

    size_t size() const { return size_; }

private:
    size_t size_;
    int* data_;   // the raw resource that forces all five special members
};

// ---------------------------------------------------------------------------
// Rule of Zero: this class owns NO raw resources directly. Every member is
// already an RAII type that correctly manages its own lifetime. No destructor,
// copy constructor, copy assignment, move constructor, or move assignment is
// written here — the compiler-generated versions are already correct.
// ---------------------------------------------------------------------------
class UserProfile {
public:
    UserProfile(std::string name, std::vector<std::string> tags)
        : name_(std::move(name)), tags_(std::move(tags)) {}

    const std::string& name() const { return name_; }
    const std::vector<std::string>& tags() const { return tags_; }

    // No destructor, no copy/move constructor, no copy/move assignment.
    // string_ and tags_ each already implement correct Rule-of-Five behavior,
    // so the compiler-generated versions of all five simply do the right thing
    // by copying or moving each member in turn.

private:
    std::string name_;                 // RAII: owns its own character buffer
    std::vector<std::string> tags_;    // RAII: owns its own dynamic array
};

int main() {
    std::cout << "Rule of Zero/Three/Five example:\n";

    std::cout << "-- Rule of Five: Buffer --\n";
    Buffer a(100);
    Buffer b = a;                 // copy constructor: deep copy, independent buffers
    Buffer c = std::move(a);      // move constructor: steals a's pointer, O(1)
    std::cout << "b.size() = " << b.size() << ", c.size() = " << c.size() << "\n";
    // a is now in a valid-but-unspecified (here: empty) state after being moved from.

    std::cout << "\n-- Rule of Zero: UserProfile --\n";
    UserProfile p1("Alice", {"admin", "beta-tester"});
    UserProfile p2 = p1;                    // compiler-generated copy: correct, because
                                             // string/vector each copy themselves correctly
    UserProfile p3 = std::move(p1);         // compiler-generated move: correct and O(1)
                                             // for the same reason

    std::cout << p2.name() << " has " << p2.tags().size() << " tags (copied)\n";
    std::cout << p3.name() << " has " << p3.tags().size() << " tags (moved)\n";

    return 0;
}