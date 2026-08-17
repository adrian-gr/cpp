// Non-Virtual Interface (NVI) idiom example
// Compile with: g++ -std=c++20 -O2 idioms/nvi_examples.cpp -o bin/nvi_example && ./bin/nvi_example

#include <iostream>
#include <stdexcept>
#include <string>

// Description:
// NVI separates a class's PUBLIC INTERFACE from its CUSTOMIZATION POINTS. The
// public methods client code calls are non-virtual and never overridden; they
// implement fixed pre-condition checks, logging, invariant enforcement, or
// post-condition checks, and then delegate to a PRIVATE virtual method that
// subclasses override to supply the actual variable behavior.
//
// This is Template Method (see the earlier GoF example) applied specifically to
// a class's public API surface: instead of exposing raw virtual methods that
// derived classes can override AND that clients call directly, the base class
// keeps full control of what happens before and after the customizable step,
// because a derived class can only override the private virtual — it can never
// bypass the public method's surrounding logic, since it doesn't even have
// access to call the private virtual directly from outside.
//
// Plain virtual functions let a derived class override BOTH what happens and
// HOW it's invoked — a subclass could technically skip logging, validation, or
// bookkeeping a base class assumed would always run, simply by being called a
// different way, or by a caller invoking the virtual method directly instead of
// through whatever "proper" entry point the base class intended. NVI closes
// that gap: there is only ONE entry point (the public non-virtual method), and
// its surrounding logic runs unconditionally, every time, for every derived
// class, with no way to circumvent it.
//
// Use NVI when:
// - a base class needs to guarantee something ALWAYS happens around a
//   customizable step — validation, logging, locking, invariant checks — no
//   matter which derived class is involved or how the call arrives;
// - you want to separate "what clients are allowed to call" (the stable public
//   interface) from "what subclasses are allowed to customize" (the private
//   virtual extension points), so each can evolve somewhat independently;
// - derived classes should not be able to call each other's (or their own
//   base's) virtual step directly, bypassing the base class's orchestration.
//
// Avoid NVI when:
// - the class has no real pre/post-condition logic to enforce — if the public
//   method would just be a one-line forwarder to the virtual with nothing else
//   around it, NVI adds indirection without buying anything.
// - the derived classes genuinely need to call the base's default behavior AND
//   run additional logic AROUND it in a way NVI's fixed template doesn't allow
//   (in which case plain virtual dispatch, or an explicit protected helper the
//   derived class calls, may fit better).
//
// Tips:
// - Keep the customization point (the virtual method) private, not protected,
//   unless a derived class genuinely needs to call it directly (e.g. to invoke
//   its own base implementation from within its override) — private maximizes
//   the base class's control.
// - The public method's pre/post-condition logic is exactly the part that
//   should never be duplicated across every derived class; that's the whole
//   point of centralizing it here instead of in each override.
// - Give the virtual method a distinct name from the public method (e.g.
//   process() / doProcess()) so it's visually clear which one a derived class
//   is meant to touch.

class DataProcessor {
public:
    virtual ~DataProcessor() = default;

    // Public interface: non-virtual, single entry point, fixed pre/post logic.
    // No derived class can change what happens here, and no caller can skip it
    // by finding another way in — there IS no other way in.
    std::string process(const std::string& input) {
        if (input.empty()) {
            throw std::invalid_argument("input cannot be empty");
        }
        std::cout << "[nvi] validating input\n";

        std::string result = doProcess(input);   // customization point

        std::cout << "[nvi] logging result: " << result << "\n";
        if (result.empty()) {
            throw std::logic_error("doProcess() must not return an empty result");
        }
        return result;
    }

private:
    // Customization point: private, so it can ONLY be reached through
    // process(). A derived class overrides this to supply its own behavior,
    // but has no way to call it directly or skip process()'s surrounding checks.
    virtual std::string doProcess(const std::string& input) = 0;
};

class UppercaseProcessor final : public DataProcessor {
private:
    std::string doProcess(const std::string& input) override {
        std::string result = input;
        for (char& c : result) c = static_cast<char>(std::toupper(c));
        return result;
    }
};

class ReverseProcessor final : public DataProcessor {
private:
    std::string doProcess(const std::string& input) override {
        return std::string(input.rbegin(), input.rend());
    }
};

int main() {
    std::cout << "NVI idiom example:\n";

    UppercaseProcessor upper;
    std::cout << "result: " << upper.process("hello world") << "\n\n";
    // process()'s validation and logging ran automatically — UppercaseProcessor
    // never had to implement, remember, or even see that logic; it could not
    // have skipped it even if it wanted to.

    ReverseProcessor reverse;
    std::cout << "result: " << reverse.process("hello world") << "\n\n";
    // Same guarantee, for a completely different customization.

    try {
        upper.process("");   // fails the PUBLIC method's pre-condition check —
                              // doProcess() is never even called.
    } catch (const std::invalid_argument& e) {
        std::cout << "caught: " << e.what() << "\n";
    }

    // upper.doProcess("x");   // would NOT compile: doProcess() is private to
    //   DataProcessor, and even UppercaseProcessor's override doesn't make it
    //   any more accessible from outside the class hierarchy. process() is the
    //   only way in, by design.

    return 0;
}