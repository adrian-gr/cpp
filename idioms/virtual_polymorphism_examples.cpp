// Virtual functions and polymorphism — with the composition-over-inheritance caveat
// Compile with: g++ -std=c++20 -O2 idioms/virtual_polymorphism_examples.cpp -o bin/virtual_polymorphism_example && ./bin/virtual_polymorphism_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Virtual functions give C++ runtime polymorphism: a base-class pointer or
// reference can invoke a derived class's override without the caller knowing
// the concrete type. The call is resolved through a vtable at runtime, which
// costs one indirection (and disables inlining across the virtual call) but
// buys genuine heterogeneous dispatch — a std::vector<std::unique_ptr<Base>>
// can hold many different derived types and call the right override on each,
// something CRTP's compile-time dispatch (see the previous example) cannot do.
//
// Inheritance is the classic way to model "is-a" relationships and share an
// interface across types. But inheritance is also one of the tightest forms of
// coupling in the language: a derived class exposes (or at least inherits) its
// base's entire interface, is broken by base-class changes, and can only ever
// have ONE base class defining its "is-a" identity (multiple inheritance exists
// but brings its own problems). Deep hierarchies built "just in case" tend to
// calcify into designs that are hard to change.
//
// The modern default — "prefer composition over inheritance" — says: reach for
// inheritance specifically when you need substitutability (a derived object must
// be usable wherever the base is expected, and behave consistently with the
// base's documented contract — the Liskov Substitution Principle). For code
// reuse alone, or for "this object needs to DO several unrelated things," HOLD
// references to other objects (composition) or accept them as constructor
// parameters (dependency injection) instead of inheriting from them.
//
// Use virtual functions / inheritance when:
// - you genuinely need runtime polymorphism: a collection of heterogeneous
//   objects, dispatched through a common interface, where the concrete type
//   isn't known until runtime;
// - the relationship is truly "is-a" AND every derived type can honor the base
//   class's full contract (Liskov substitutability) — not just "is-a" in name.
//
// Prefer composition when:
// - you're reusing code/behavior, not modeling a genuine is-a relationship — hold
//   an instance of the reused class as a member instead of inheriting from it;
// - an object needs several unrelated capabilities — compose it from several
//   single-purpose objects rather than trying to inherit all of them (C++ allows
//   multiple inheritance, but composition is usually more explicit and flexible);
// - you want to swap behavior at runtime without touching the class hierarchy —
//   composed dependencies can be swapped per-instance; inheritance is fixed at
//   compile time per concrete type.
//
// Avoid:
// - deep inheritance hierarchies built for hypothetical future reuse rather
//   than a real, current substitutability need;
// - inheriting just to "get" some methods for free when the derived type can't
//   honestly satisfy the base class's behavioral contract (classic violation:
//   a Square inheriting from a mutable Rectangle, then breaking width/height
//   independence).
//
// Tips:
// - Give base classes intended for polymorphic use a virtual destructor —
//   without one, deleting a derived object through a base pointer is undefined
//   behavior.
// - Mark overriding methods `override` (and the class `final` if no further
//   derivation is intended) so the compiler catches signature mismatches and
//   accidental hiding instead of silently creating a new, unrelated method.
// - When you catch yourself inheriting purely to reuse a handful of methods,
//   ask whether a member (composition) would express the relationship more
//   honestly — "has-a"/"uses-a" rather than "is-a".

// ---------------------------------------------------------------------------
// Genuine "is-a" + substitutability: every PaymentMethod can be charged, and
// every derived type honors that contract fully. This is exactly the case
// virtual dispatch is for.
// ---------------------------------------------------------------------------
class PaymentMethod {
public:
    virtual ~PaymentMethod() = default;   // virtual destructor: required for
                                            // safe deletion through a base pointer.
    virtual void charge(double amount) const = 0;
};

class CreditCard final : public PaymentMethod {
public:
    void charge(double amount) const override {
        std::cout << "charging $" << amount << " to credit card\n";
    }
};

class PayPalAccount final : public PaymentMethod {
public:
    void charge(double amount) const override {
        std::cout << "charging $" << amount << " via PayPal\n";
    }
};

// Genuinely needs runtime polymorphism: a heterogeneous collection, resolved
// per-element, unknown until runtime which concrete type each element is.
void chargeAll(const std::vector<std::unique_ptr<PaymentMethod>>& methods, double amount) {
    for (const auto& method : methods) {
        method->charge(amount);
    }
}

// ---------------------------------------------------------------------------
// Composition over inheritance: an Order needs LOGGING and NOTIFICATION
// capability. It is not "a" Logger or "a" Notifier — it USES a logger and a
// notifier. Composing these as members (dependencies) instead of inheriting
// from Logger/Notifier base classes keeps Order's own interface focused on
// what an Order actually is, and lets each dependency be swapped independently.
// ---------------------------------------------------------------------------
class Logger {
public:
    virtual ~Logger() = default;
    virtual void log(const std::string& message) const = 0;
};

class ConsoleLogger final : public Logger {
public:
    void log(const std::string& message) const override {
        std::cout << "[log] " << message << "\n";
    }
};

class Notifier {
public:
    virtual ~Notifier() = default;
    virtual void notify(const std::string& message) const = 0;
};

class EmailNotifier final : public Notifier {
public:
    void notify(const std::string& message) const override {
        std::cout << "[email] " << message << "\n";
    }
};

// Order is NOT a Logger and NOT a Notifier — it HAS one of each, injected via
// its constructor. This is composition: Order can be tested with fake
// loggers/notifiers, and either dependency can be swapped without touching
// Order's own class hierarchy (there isn't one).
class Order {
public:
    Order(std::string item, std::unique_ptr<Logger> logger, std::unique_ptr<Notifier> notifier)
        : item_(std::move(item)), logger_(std::move(logger)), notifier_(std::move(notifier)) {}

    void place() {
        logger_->log("order placed: " + item_);
        notifier_->notify("your order for " + item_ + " has been placed");
    }

private:
    std::string item_;
    std::unique_ptr<Logger> logger_;      // composed dependency, not a base class
    std::unique_ptr<Notifier> notifier_;  // composed dependency, not a base class
};

int main() {
    std::cout << "Virtual functions and polymorphism example:\n";

    std::cout << "-- runtime polymorphism (inheritance is the right tool) --\n";
    std::vector<std::unique_ptr<PaymentMethod>> methods;
    methods.push_back(std::make_unique<CreditCard>());
    methods.push_back(std::make_unique<PayPalAccount>());
    chargeAll(methods, 42.50);
    // Each ->charge() call resolves to the correct override at runtime, based
    // on the object's actual type — impossible to know at compile time which
    // charge() implementation runs for a given vector element.

    std::cout << "\n-- composition (dependency injection, not inheritance) --\n";
    Order order("wireless mouse",
                std::make_unique<ConsoleLogger>(),
                std::make_unique<EmailNotifier>());
    order.place();
    // Order's own class hierarchy is just "class Order" — flat, with no base
    // class at all. Its behavior is customized by WHAT IT HOLDS, not by WHAT
    // IT INHERITS FROM. A test could construct an Order with a mock Logger
    // and a mock Notifier without touching Order's definition at all.

    return 0;
}