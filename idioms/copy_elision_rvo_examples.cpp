// Copy elision / RVO (Return Value Optimization) example
// Compile with: g++ -std=c++20 -O2 idioms/copy_elision_rvo_examples.cpp -o bin/copy_elision_rvo_example && ./bin/copy_elision_rvo_example
// (also try building with -fno-elide-constructors to see the mandatory cases
//  still elide, and -std=c++14 to see the non-mandatory cases likely still
//  elide too, just no longer GUARANTEED to by the standard)

#include <iostream>
#include <utility>

// Description:
// "Returning by value is expensive" was true in older C++, and is largely a
// myth in modern C++ — because of copy elision. Copy elision means the
// compiler constructs the return value DIRECTLY in the caller's storage,
// skipping the copy/move constructor call (and the temporary object) that a
// naive reading of the code would suggest happens. RVO (Return Value
// Optimization) is the specific case of copy elision applied to returning a
// local variable or a temporary by value.
//
// There are two flavors, with different levels of guarantee:
//
// - Mandatory copy elision (C++17 and later, guaranteed by the standard):
//   `return SomeType(args);` — returning a PRVALUE (a temporary constructed
//   right there, not a named variable) — is REQUIRED to elide the copy/move
//   entirely. There is no "as-if" here; the standard mandates that no
//   copy/move constructor is even called. This works even for types with NO
//   copy or move constructor at all — construction happens directly in the
//   caller's storage, full stop.
//
// - NRVO (Named Return Value Optimization, an OPTIMIZATION, not guaranteed):
//   `SomeType local; ... return local;` — returning a NAMED local variable —
//   compilers are ALLOWED, but not required, to elide the copy/move here.
//   Every mainstream compiler does this reliably at typical optimization
//   levels, but the standard doesn't mandate it, so a type must still have an
//   accessible copy or move constructor for this code to even compile,
//   because the "as-if elided" copy/move must be a valid call even when the
//   compiler chooses to skip actually performing it.
//
// This distinction matters concretely: mandatory elision (returning a prvalue
// directly) is the strongest, most portable way to guarantee "no copy, no
// move, ever" for a return — including for move-only or non-movable types.
// NRVO (returning a named local) is extremely reliable in practice but is,
// technically, still just an optimization a compiler is permitted to skip.
//
// Use direct prvalue returns (`return T(...)` or `return {...}`) when:
// - you want the STRONGEST possible guarantee against any copy/move —
//   especially relevant for expensive-to-move or move-only types.
//
// Understand that NRVO applies (but isn't guaranteed) when:
// - you build up a named local variable across multiple statements and
//   return it at the end — this is extremely common, idiomatic code, and
//   virtually always elided by real compilers, but the type still needs a
//   valid (even if never actually called) copy/move constructor to compile.
//
// Know that elision does NOT apply when:
// - you return a data member, a parameter, or a different variable depending
//   on a branch condition WITHOUT it being the exact same named local on
//   every path — multiple different named variables returned from different
//   branches typically prevents NRVO (though a single named variable
//   returned from every branch is still eligible).
// - you explicitly wrap the return in std::move: `return std::move(local);`
//   — this is a common, well-intentioned MISTAKE. It actively PREVENTS NRVO
//   (by turning a prvalue-eligible expression into an xvalue, forcing an
//   actual move instead of allowing elision) and in the mandatory-elision
//   case does nothing beneficial at all. Just `return local;` — let the
//   compiler decide.
//
// Tips:
// - Prefer returning by value over out-parameters (`void compute(T& out)`) in
//   modern C++ for exactly this reason — the old "avoid returning big objects
//   by value" advice predates guaranteed copy elision and is largely obsolete.
// - Never write `return std::move(localVariable);` — it defeats NRVO for no
//   benefit; plain `return localVariable;` is always at least as good, and
//   often strictly better.
// - Use a move-only type (like the one below) as a sanity check: if your
//   function compiles and returns by value, and the type genuinely has no
//   copy constructor, you have concrete proof elision is happening for the
//   prvalue-return case — the code couldn't compile otherwise.

// A type that LOGS every constructor/destructor call, so we can literally
// watch how many (if any) copies or moves happen for each returning style.
class Traced {
public:
    explicit Traced(std::string label) : label_(std::move(label)) {
        std::cout << "  constructed: " << label_ << "\n";
    }
    ~Traced() {
        std::cout << "  destroyed: " << label_ << "\n";
    }
    Traced(const Traced& other) : label_(other.label_ + " (copy)") {
        std::cout << "  COPY constructed: " << label_ << "\n";
    }
    Traced(Traced&& other) noexcept : label_(other.label_ + " (moved)") {
        std::cout << "  MOVE constructed: " << label_ << "\n";
    }

    const std::string& label() const { return label_; }

private:
    std::string label_;
};

// Mandatory elision: returning a prvalue directly. The standard REQUIRES this
// to construct the Traced object directly in the caller's storage — no copy
// or move constructor call happens, guaranteed, even at -O0.
Traced makeMandatory() {
    return Traced("mandatory-prvalue");
}

// NRVO: returning a named local variable. Compilers reliably elide this too,
// but it's an optimization, not a guarantee — Traced still needs an
// accessible move constructor for this to compile, even though it won't
// actually be called when NRVO applies.
Traced makeNrvo() {
    Traced local("nrvo-named-local");
    std::cout << "  (about to return local by name)\n";
    return local;
}

// The common MISTAKE: wrapping a named local in std::move defeats NRVO. This
// forces an actual move construction to happen, where plain `return local;`
// would very likely have elided it entirely.
Traced makeMistakenMove() {
    Traced local("mistaken-std-move");
    return std::move(local);   // DON'T do this — see the extra MOVE line below.
}

// A MOVE-ONLY type (copy constructor explicitly deleted). If this function
// compiles and runs at all, mandatory elision MUST be happening for the
// `return Traced(...)`-style prvalue case — there is no copy constructor to
// even theoretically fall back on.
class MoveOnly {
public:
    explicit MoveOnly(int id) : id_(id) {
        std::cout << "  MoveOnly constructed: " << id_ << "\n";
    }
    ~MoveOnly() { std::cout << "  MoveOnly destroyed: " << id_ << "\n"; }
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&&) noexcept = default;
    MoveOnly& operator=(MoveOnly&&) noexcept = default;

    int id() const { return id_; }

private:
    int id_;
};

MoveOnly makeMoveOnly(int id) {
    return MoveOnly(id);   // prvalue return: MUST be elided (mandatory in
                             // C++17+) — this would fail to COMPILE otherwise,
                             // since MoveOnly's copy constructor is deleted
                             // and a non-elided path would need one.
}

int main() {
    std::cout << "Copy elision / RVO example:\n";

    std::cout << "-- mandatory elision (return prvalue) --\n";
    Traced a = makeMandatory();
    std::cout << "  got: " << a.label() << "\n\n";
    // Expect exactly ONE "constructed" line and no "COPY"/"MOVE" line at all
    // for a's construction — the object was built directly in a's storage.

    std::cout << "-- NRVO (return named local) --\n";
    Traced b = makeNrvo();
    std::cout << "  got: " << b.label() << "\n\n";
    // In practice (any real compiler, any real optimization level): still
    // exactly ONE "constructed" line, no "MOVE" line — NRVO elides it too,
    // even though the standard only permits, not requires, this.

    std::cout << "-- the std::move mistake (defeats NRVO) --\n";
    Traced c = makeMistakenMove();
    std::cout << "  got: " << c.label() << "\n\n";
    // Notice the extra "MOVE constructed" line here that the previous two
    // examples did NOT have — std::move(local) forced an actual move that
    // plain `return local;` would very likely have avoided entirely.

    std::cout << "-- move-only type: proof mandatory elision is really happening --\n";
    MoveOnly m = makeMoveOnly(42);
    std::cout << "  got id: " << m.id() << "\n";
    // This function compiles and runs despite MoveOnly having NO copy
    // constructor — only possible because `return MoveOnly(id);` is
    // guaranteed to be elided, never actually invoking any copy or move.

    return 0;
}