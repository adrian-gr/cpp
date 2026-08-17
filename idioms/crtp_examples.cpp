// CRTP (Curiously Recurring Template Pattern) example
// Compile with: g++ -std=c++20 -O2 idioms/crtp_examples.cpp -o bin/crtp_example && ./bin/crtp_example

#include <iostream>
#include <memory>
#include <string>

// Description:
// CRTP is a technique where a class Derived inherits from a base class template
// parameterized on Derived itself: `class Derived : public Base<Derived>`. The
// base class can then static_cast<Derived*>(this) to call methods Derived defines,
// achieving polymorphism-like dispatch WITHOUT virtual functions — resolved
// entirely at compile time, with no vtable, no virtual call overhead, and (unlike
// runtime polymorphism) the concrete type is known and can be fully inlined.
//
// This is "static polymorphism": Base<T>'s interface looks like it's dispatching
// to an overridden method, but there's no runtime indirection at all — the
// compiler generates a distinct Base<Derived> for each Derived, resolving every
// call to Derived's method directly at compile time.
//
// Two very common real-world uses of exactly this pattern:
// - std::enable_shared_from_this<T>: a class inherits from it to safely obtain a
//   std::shared_ptr<T> to itself from within a member function, without owning a
//   raw `this` that a caller could mismanage.
// - Mixins: attaching a reusable capability (comparison operators, cloning,
//   counting instances) to many unrelated classes, where the mixin's logic needs
//   to know the concrete derived type to do its job.
//
// Use CRTP when:
// - you want compile-time (static) polymorphism instead of virtual dispatch —
//   same "customization point" idea as an interface, but resolved at compile
//   time, avoiding vtable lookup overhead and enabling full inlining;
// - you're writing a mixin that needs to operate on or return the concrete
//   derived type, not just a base-class interface (e.g. returning Derived&, not
//   Base&, from a chainable method);
// - you're implementing something like enable_shared_from_this: a base class
//   that provides functionality genuinely dependent on knowing the exact derived
//   type at compile time.
//
// Avoid CRTP when:
// - you need genuine runtime polymorphism — e.g. storing a collection of
//   different derived types behind a common base pointer/reference and dispatching
//   based on the object's actual runtime type. CRTP can't do that; use virtual
//   functions instead.
// - a simpler alternative (an ordinary virtual function, a free function template,
//   a concept-constrained function) already solves the problem without the
//   template-heavy syntax CRTP requires.
// - the readability cost of the base-parameterized-on-derived idiom outweighs the
//   performance benefit for your actual use case (many programs never need to
//   avoid virtual-call overhead badly enough to justify this).
//
// Tips:
// - The base class accesses the derived class's members via
//   static_cast<Derived*>(this) (or a helper method that does this once); this
//   cast is always safe here because Derived, by construction, publicly derives
//   from Base<Derived>.
// - CRTP bases are typically NOT meant to be used polymorphically themselves
//   (no virtual destructor needed, no runtime dispatch) — they exist purely to be
//   inherited from, once, by exactly the type that parameterizes them.
// - Combine CRTP with private inheritance or a protected constructor on the base
//   if you want to prevent anyone but the intended Derived from inheriting it.

// ---------------------------------------------------------------------------
// 1. Static polymorphism: Base<Derived> calls into Derived's method directly,
// with zero virtual dispatch.
// ---------------------------------------------------------------------------
template <typename Derived>
class Shape {
public:
    double area() const {
        // static_cast<const Derived*>(this) is always safe: Derived publicly
        // inherits from Shape<Derived>, so `this` genuinely points at a Derived.
        return static_cast<const Derived*>(this)->areaImpl();
    }

    void describe() const {
        std::cout << "shape with area " << area() << "\n";
    }
};

class Circle final : public Shape<Circle> {
public:
    explicit Circle(double radius) : radius_(radius) {}
    double areaImpl() const { return 3.14159 * radius_ * radius_; }

private:
    double radius_;
};

class Square final : public Shape<Square> {
public:
    explicit Square(double side) : side_(side) {}
    double areaImpl() const { return side_ * side_; }

private:
    double side_;
};

// ---------------------------------------------------------------------------
// 2. Mixin: adds a chainable, self-returning capability to any derived class,
// without virtual functions and while returning the concrete derived type
// (not a base-class reference) so chaining preserves the derived interface.
// ---------------------------------------------------------------------------
template <typename Derived>
class Loggable {
public:
    Derived& logAction(const std::string& action) {
        std::cout << "[log] " << action << "\n";
        return static_cast<Derived&>(*this);   // returns Derived&, enabling
                                                 // chained calls to Derived's
                                                 // own methods afterward.
    }
};

class Robot final : public Loggable<Robot> {
public:
    Robot& move(int steps) {
        std::cout << "moving " << steps << " steps\n";
        return *this;
    }
};

// ---------------------------------------------------------------------------
// 3. std::enable_shared_from_this: the standard library's own CRTP use.
// A class inherits from enable_shared_from_this<Self> to safely produce a
// shared_ptr<Self> to itself from within a member function.
// ---------------------------------------------------------------------------
class Worker final : public std::enable_shared_from_this<Worker> {
public:
    std::shared_ptr<Worker> getSelf() {
        return shared_from_this();   // safe: ties into the SAME shared_ptr
                                       // control block as any existing owner.
    }
};

int main() {
    std::cout << "CRTP example:\n";

    std::cout << "-- static polymorphism --\n";
    Circle circle(5.0);
    Square square(4.0);
    circle.describe();   // Shape<Circle>::area() calls Circle::areaImpl() directly
    square.describe();   // Shape<Square>::area() calls Square::areaImpl() directly
    // No vtable, no virtual call: each call above is resolved and can be
    // inlined entirely at compile time, because Shape<Circle> and Shape<Square>
    // are distinct, fully-specialized classes.

    std::cout << "\n-- mixin --\n";
    Robot robot;
    robot.logAction("startup").move(3).logAction("moved");
    // logAction() returns Robot&, not Loggable<Robot>&, so .move(3) — a
    // Robot-only method — is callable right after it in the same chain.

    std::cout << "\n-- enable_shared_from_this --\n";
    auto worker = std::make_shared<Worker>();
    std::shared_ptr<Worker> selfRef = worker->getSelf();
    std::cout << "shared_ptr use_count: " << worker.use_count() << "\n";
    // use_count is 2: `worker` and `selfRef` share the same control block,
    // which is exactly what enable_shared_from_this guarantees — a plain
    // `std::shared_ptr<Worker>(this)` inside getSelf() would instead create a
    // SECOND, unrelated control block, leading to a double-free.

    return 0;
}