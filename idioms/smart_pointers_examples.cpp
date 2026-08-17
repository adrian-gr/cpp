// Smart pointers — unique_ptr, shared_ptr, weak_ptr replacing raw new/delete
// Compile with: g++ -std=c++20 -O2 idioms/smart_pointers_examples.cpp -o bin/smart_pointers_example && ./bin/smart_pointers_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Smart pointers are RAII wrappers (see the earlier RAII example) around raw
// pointers: the pointed-to object's lifetime is tied to the smart pointer's
// own lifetime, so it's destroyed automatically — correctly, exactly once —
// with no explicit `delete` required anywhere in user code. Three standard
// smart pointers cover almost every ownership situation:
//
// - std::unique_ptr<T>: EXCLUSIVE ownership. Exactly one unique_ptr owns the
//   object at a time. It cannot be copied (copying would imply two owners),
//   only moved (transferring, never duplicating, ownership). Zero overhead
//   compared to a raw pointer in the common case — no reference count, no
//   extra allocation; it's typically just a pointer with a destructor.
//
// - std::shared_ptr<T>: SHARED ownership. Multiple shared_ptrs can own the
//   same object simultaneously, via reference counting; the object is
//   destroyed when the LAST owning shared_ptr is destroyed or reset. This
//   costs a control block (allocated once, ideally via make_shared) tracking
//   the reference count, plus atomic increment/decrement on copy/destroy for
//   thread safety — real overhead, in exchange for genuinely shared lifetime.
//
// - std::weak_ptr<T>: a NON-OWNING observer of an object managed by
//   shared_ptr. It doesn't keep the object alive and doesn't contribute to
//   the reference count, but it CAN be checked/upgraded to a shared_ptr
//   (via .lock()) if the object still exists. Its main purpose is breaking
//   reference cycles: two shared_ptrs that (directly or indirectly) own each
//   other would never reach a zero reference count and would leak forever;
//   replacing one direction of that cycle with a weak_ptr fixes it.
//
// The rule of thumb: default to unique_ptr. Reach for shared_ptr only when
// genuinely shared, overlapping ownership is required — not just because
// "it's easier" or "I'm not sure who owns this." Use weak_ptr specifically to
// observe a shared_ptr-managed object without extending its lifetime, most
// commonly to break a cycle between two shared_ptr-holding objects.
//
// Use unique_ptr when:
// - exactly one owner should exist for the object's entire lifetime — this
//   should be the DEFAULT choice for any dynamically allocated object; reach
//   for something else only when unique_ptr's constraints genuinely don't fit.
// - you want a raw-pointer-equivalent-cost RAII wrapper around a resource
//   that needs custom cleanup (via a custom deleter) beyond just `delete`.
//
// Use shared_ptr when:
// - multiple independent parts of the program genuinely need to keep an
//   object alive, and none of them has a clear, sole claim to "owns it" —
//   e.g. a cache entry referenced by both the cache and several in-flight
//   consumers, or a shared resource in a graph-like structure.
//
// Use weak_ptr when:
// - you need to observe or occasionally access a shared_ptr-managed object
//   WITHOUT keeping it alive yourself — e.g. a cache that shouldn't prevent
//   its entries from being freed, an observer that shouldn't outlive what
//   it's observing, or a child object holding a reference back to its parent.
// - breaking a shared_ptr reference cycle (A holds shared_ptr<B>, B holds
//   shared_ptr<A> — neither will ever reach a zero refcount; make one
//   direction weak_ptr instead).
//
// Avoid / be careful:
// - reaching for shared_ptr by default "to be safe" — it's genuinely more
//   expensive (atomic refcounting, a control block) and, worse, can mask
//   unclear ownership design rather than expressing a real design decision.
// - creating a shared_ptr from a raw `this` pointer directly
//   (std::shared_ptr<T>(this)) inside a member function — this creates a
//   SECOND, unrelated control block, leading to a double-free; use
//   std::enable_shared_from_this (see the earlier CRTP example) instead.
// - calling raw `new`/`delete` directly in modern code at all — prefer
//   std::make_unique/std::make_shared, which are exception-safer (see the
//   note below) and, for shared_ptr, avoid a second allocation.

// A traced type so construction/destruction is visible in the output, the
// same technique the RVO example used.
struct Traced {
    explicit Traced(std::string name) : name_(std::move(name)) {
        std::cout << "  constructed: " << name_ << "\n";
    }
    ~Traced() {
        std::cout << "  destroyed: " << name_ << "\n";
    }
    std::string name_;
};

// unique_ptr as a function parameter/return: makes ownership TRANSFER
// explicit in the signature itself. Taking unique_ptr<T> by value means "this
// function now owns the object"; returning one means "the caller now owns it."
std::unique_ptr<Traced> createTraced(const std::string& name) {
    return std::make_unique<Traced>(name);   // prvalue return: elided per the
                                               // earlier RVO example, and
                                               // exception-safe (see below).
}

void takeOwnership(std::unique_ptr<Traced> owned) {
    std::cout << "  took ownership of: " << owned->name_ << "\n";
}   // owned's Traced is destroyed here, when the parameter goes out of scope.

// A Node structure that intentionally demonstrates the shared_ptr cycle
// problem, and its weak_ptr fix.
struct Node {
    std::string name;
    std::shared_ptr<Node> next;   // owning: keeps the next node alive
    std::weak_ptr<Node> prev;     // NON-owning: observes the previous node
                                    // without keeping it alive — this is what
                                    // breaks the cycle.

    explicit Node(std::string n) : name(std::move(n)) {
        std::cout << "  Node constructed: " << name << "\n";
    }
    ~Node() {
        std::cout << "  Node destroyed: " << name << "\n";
    }
};

int main() {
    std::cout << "Smart pointers example:\n";

    std::cout << "-- unique_ptr: exclusive ownership --\n";
    {
        std::unique_ptr<Traced> a = std::make_unique<Traced>("unique-A");
        // std::unique_ptr<Traced> b = a;   // would NOT compile: copy
        //   constructor is deleted — unique_ptr cannot be copied, only moved.
        std::unique_ptr<Traced> b = std::move(a);   // ownership TRANSFERS to b
        std::cout << "  a is now: " << (a ? "non-null" : "null") << "\n";
        std::cout << "  b owns: " << b->name_ << "\n";
    }   // b's Traced is destroyed here automatically, exactly once.

    std::cout << "\n-- unique_ptr signatures documenting ownership transfer --\n";
    std::unique_ptr<Traced> owned = createTraced("factory-made");
    takeOwnership(std::move(owned));   // std::move REQUIRED here: unique_ptr
                                         // truly cannot be copied, so passing
                                         // it by value to takeOwnership must
                                         // move — unlike the RVO example's
                                         // warning against std::move on a
                                         // RETURN, this is a genuine transfer.
    std::cout << "  owned is now: " << (owned ? "non-null" : "null") << "\n\n";

    std::cout << "-- shared_ptr: shared ownership, refcounted --\n";
    {
        std::shared_ptr<Traced> s1 = std::make_shared<Traced>("shared-X");
        std::cout << "  use_count after creation: " << s1.use_count() << "\n";
        {
            std::shared_ptr<Traced> s2 = s1;   // COPY is fine: refcount increments
            std::cout << "  use_count with s2 alive: " << s1.use_count() << "\n";
        }   // s2 destroyed; refcount decrements, but object survives (s1 remains)
        std::cout << "  use_count after s2 destroyed: " << s1.use_count() << "\n";
    }   // s1 destroyed; refcount reaches 0; Traced is destroyed HERE, not earlier.
    std::cout << "\n";

    std::cout << "-- weak_ptr breaking a shared_ptr reference cycle --\n";
    {
        auto first = std::make_shared<Node>("first");
        auto second = std::make_shared<Node>("second");

        first->next = second;    // first OWNS second (shared_ptr)
        second->prev = first;    // second OBSERVES first (weak_ptr) — does
                                   // NOT keep first alive; this is what
                                   // prevents the cycle from leaking.

        std::cout << "  first use_count: " << first.use_count() << "\n";   // 1
        std::cout << "  second use_count: " << second.use_count() << "\n"; // 2
        //   second's count is 2 because `second` (the local variable) AND
        //   first->next both own it; first's count is only 1 because
        //   second->prev is a weak_ptr and does NOT contribute to the count.

        // .lock() upgrades a weak_ptr to a shared_ptr IF the object still
        // exists, or returns an empty shared_ptr if it's already been
        // destroyed — this check-before-use is required precisely because
        // weak_ptr never guarantees the object is still alive.
        if (auto locked = second->prev.lock()) {
            std::cout << "  second->prev is still alive: " << locked->name << "\n";
        }
    }   // Both Nodes are correctly destroyed here. If `prev` had ALSO been a
        // shared_ptr, first and second would hold shared_ptrs to each other,
        // neither refcount would ever reach zero, and this block would leak
        // both Nodes silently, forever — the classic shared_ptr cycle leak.

    return 0;
}