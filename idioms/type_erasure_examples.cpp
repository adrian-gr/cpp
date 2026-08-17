// Type erasure example — hiding a concrete type behind a common interface at
// runtime, WITHOUT requiring that type to inherit from anything
// Compile with: g++ -std=c++20 -O2 idioms/type_erasure_examples.cpp -o bin/type_erasure_example && ./bin/type_erasure_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <functional>

// Description:
// Ordinary runtime polymorphism (see the earlier virtual-functions example)
// requires the concrete type to INHERIT from a common base class — Circle and
// Square both have to derive from Shape to be stored polymorphically. Type
// erasure achieves the same effect — storing and calling unrelated concrete
// types through one common interface — WITHOUT requiring the concrete types
// to know about, or inherit from, anything at all. A Duck class and a Robot
// class, written independently, with no shared base, no shared header, and no
// awareness of each other, can both be stored in the same type-erased
// container as long as each happens to provide the right method/operation.
//
// The mechanism (often called the "Concept/Model" idiom) has three parts:
// - Concept: a private, internal abstract interface (the ONLY place virtual
//   functions appear) declaring what operations are needed.
// - Model<T>: a private, internal TEMPLATE that implements Concept by holding
//   a T and forwarding calls to whatever operation T actually provides
//   (T::draw(), operator<<(T), etc.) — one Model<T> instantiation exists per
//   concrete type ever stored, generated automatically by the compiler.
// - The PUBLIC-FACING type (e.g. Drawable) is an ordinary, NON-template
//   class. It holds a std::unique_ptr<Concept> and forwards its own public
//   methods to it. Nothing about Concept, Model<T>, or virtual functions is
//   visible to a user of Drawable — the polymorphism, and the fact that
//   virtual dispatch is happening at all internally, is entirely hidden
//   ("erased") behind Drawable's ordinary-looking, non-template interface.
//
// This is exactly how std::function<Signature> and std::any are implemented
// internally: std::function can hold a free function, a lambda, a functor
// object, or a member-function pointer wrapper — none of which share a common
// base class or even know std::function exists — because internally it uses
// this same Concept/Model split, generating one hidden Model<T> per distinct
// callable type it's ever constructed from.
//
// Use type erasure when:
// - you want to store/call heterogeneous types through one common interface,
//   but the types either can't be modified to inherit from a common base
//   (third-party types, built-in types, lambdas) or genuinely shouldn't be
//   coupled to your interface via inheritance just to be usable with it;
// - you're designing a library-facing type (like a callback parameter, or a
//   generic container of "anything with capability X") where forcing every
//   caller's type to inherit from your base class would be an unreasonable
//   imposition on their code.
//
// Avoid type erasure when:
// - the types you need to store are already yours to modify, and a
//   conventional base class + inheritance (ordinary virtual functions)
//   already solves the problem more simply — type erasure adds real
//   implementation complexity (the Concept/Model machinery) that isn't worth
//   paying for when plain inheritance would do.
// - you need compile-time type information back frequently — type erasure
//   deliberately discards the concrete type; getting it back (like
//   std::any_cast) is more awkward and error-prone than never having erased
//   it in the first place.
//
// Tips:
// - Concept and Model<T> should be private nested classes/details, invisible
//   to users of the public-facing type — the entire point is that a caller
//   never needs to know virtual dispatch is happening underneath.
// - Model<T> is typically small: hold a T (or a T&/shared_ptr<T> for
//   non-owning/shared variants), and forward each Concept method to whatever
//   operation T actually supports.
// - The public-facing type's constructor is a template (accepting any T),
//   even though the class itself is not — this is what lets it accept
//   completely unrelated concrete types while remaining an ordinary type
//   itself, storable in an ordinary (non-template) std::vector.

// ---------------------------------------------------------------------------
// Two completely unrelated types, deliberately: no shared base class, no
// shared header, no awareness of each other or of any "Drawable" concept.
// Duck happens to have a draw() method; Robot happens to have a differently
// named render() method — type erasure will adapt both to one interface.
// ---------------------------------------------------------------------------
class Duck {
public:
    explicit Duck(std::string name) : name_(std::move(name)) {}
    void draw() const { std::cout << name_ << " the duck: quack!\n"; }

private:
    std::string name_;
};

class Robot {
public:
    explicit Robot(int id) : id_(id) {}
    void render() const { std::cout << "Robot #" << id_ << ": beep boop\n"; }

private:
    int id_;
};

// ---------------------------------------------------------------------------
// The type-erased wrapper. Drawable itself is an ORDINARY, non-template
// class — it can be stored in a plain std::vector<Drawable>, unlike a
// template.
// ---------------------------------------------------------------------------
class Drawable {
private:
    // Concept: the ONLY place a virtual function appears in this whole
    // design. Private and nested — invisible to Drawable's own users.
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() const = 0;
    };

    // Model<T>: one instantiation generated per distinct T ever stored in a
    // Drawable. Adapts whatever operation T actually provides to Concept's
    // draw() method — this is the "adapter" layer that lets Duck::draw() and
    // Robot::render() both satisfy the same erased interface.
    template <typename T>
    struct Model final : Concept {
        explicit Model(T value) : value_(std::move(value)) {}

        void draw() const override {
            callDraw(value_);   // dispatches based on what T actually has
        }

        // Overload resolution (NOT virtual dispatch) picks the right one of
        // these at COMPILE time, per T — this is where "Duck has draw()" vs
        // "Robot has render()" gets adapted to one shape.
        static void callDraw(const Duck& duck) { duck.draw(); }
        static void callDraw(const Robot& robot) { robot.render(); }

        T value_;
    };

    std::unique_ptr<Concept> concept_;   // the ENTIRE state of Drawable, as
                                           // far as its own type declares —
                                           // no trace of Duck, Robot, or even
                                           // that virtual dispatch is used.

public:
    // Templated CONSTRUCTOR on a non-template CLASS: this is what lets
    // Drawable be built from any T (Duck, Robot, anything else with a
    // callDraw overload added later) while Drawable itself stays an
    // ordinary, storable-in-a-plain-vector type.
    template <typename T>
    Drawable(T value) : concept_(std::make_unique<Model<T>>(std::move(value))) {}

    // Public interface: ordinary, non-virtual, non-template. Forwards to the
    // erased Concept — this is the ONLY method a user of Drawable ever sees
    // or calls; everything above is a private implementation detail.
    void draw() const {
        concept_->draw();
    }
};

int main() {
    std::cout << "Type erasure example:\n";

    // A single, ORDINARY std::vector<Drawable> — not a template parameter
    // pack, not std::vector<std::variant<Duck, Robot>>, and Duck/Robot never
    // had to inherit from anything to end up in the same container.
    std::vector<Drawable> shapes;
    shapes.emplace_back(Duck("Donald"));
    shapes.emplace_back(Robot(7));
    shapes.emplace_back(Duck("Daffy"));

    for (const auto& shape : shapes) {
        shape.draw();   // one call site, dispatching to Duck::draw() or
                          // Robot::render() depending on what's actually
                          // erased inside each Drawable — resolved via the
                          // hidden Concept/Model virtual call underneath.
    }

    std::cout << "\n-- for comparison: std::function does the same thing --\n";
    // std::function<void()> is the standard library's own type-erasure
    // wrapper: a free function, a lambda, and a functor object below share
    // NO common base class, yet all three fit in the same
    // std::vector<std::function<void()>> — exactly the Drawable pattern
    // above, just already written for you in <functional>.
    struct Greeter {
        void operator()() const { std::cout << "Greeter functor called\n"; }
    };

    std::vector<std::function<void()>> callbacks;
    callbacks.push_back([] { std::cout << "lambda called\n"; });
    callbacks.push_back(Greeter{});
    for (const auto& callback : callbacks) {
        callback();
    }

    return 0;
}