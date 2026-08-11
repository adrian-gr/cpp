// State pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/state/state_examples.cpp -o bin/state_example && ./bin/state_example

#include <iostream>
#include <memory>
#include <string>

// Description:
// State lets an object alter its behavior when its internal state changes, so it
// appears to change class. Behavior that would otherwise be a large conditional
// keyed on a "current state" field is instead split across a family of state
// classes, each implementing the behavior for one state.
//
// Use State when:
// - an object's behavior depends heavily on its current state, and that behavior
//   must change at runtime as the state changes;
// - you have large conditionals (switch/if-else on a state enum) scattered across
//   many methods, all branching on the same underlying state;
// - transitions between states follow clear rules that are cleaner to express as
//   one state object handing off to the next than as flags and branches.
//
// Avoid State when:
// - there are only two or three simple states with trivial, rarely-changing
//   behavior — a boolean flag or enum with a small switch is easier to follow;
// - the "states" don't actually change behavior, only data — that's not a State
//   pattern case, just varying fields;
// - the state machine is trivial enough that the class-per-state overhead outweighs
//   the clarity gained over a straightforward conditional.
//
// Tips:
// - Let each state decide the next state, rather than centralizing transition logic
//   in the context — this keeps transition rules local to where they're triggered.
// - Keep the context's public interface stable; only its internal state object changes.
// - Consider making states stateless singletons if they hold no per-instance data.

class VendingMachine;

class VendingState {
public:
    virtual ~VendingState() = default;
    virtual void insertCoin(VendingMachine& machine) = 0;
    virtual void selectItem(VendingMachine& machine) = 0;
    virtual std::string name() const = 0;
};

// Context: delegates behavior to its current state, and holds the state pointer
// that state objects themselves are responsible for transitioning.
class VendingMachine {
public:
    explicit VendingMachine(std::unique_ptr<VendingState> initialState)
        : state_(std::move(initialState)) {}

    void insertCoin() { state_->insertCoin(*this); }
    void selectItem() { state_->selectItem(*this); }

    void setState(std::unique_ptr<VendingState> newState) {
        std::cout << "transition: " << state_->name() << " -> " << newState->name() << "\n";
        state_ = std::move(newState);
    }

private:
    std::unique_ptr<VendingState> state_;
};

class DispensingState final : public VendingState {
public:
    void insertCoin(VendingMachine&) override {
        std::cout << "already dispensing, please wait\n";
    }
    void selectItem(VendingMachine&) override {
        std::cout << "already dispensing, please wait\n";
    }
    std::string name() const override { return "Dispensing"; }
};

class HasCoinState final : public VendingState {
public:
    void insertCoin(VendingMachine&) override {
        std::cout << "coin already inserted\n";
    }
    void selectItem(VendingMachine& machine) override {
        std::cout << "dispensing item\n";
        machine.setState(std::make_unique<DispensingState>());
    }
    std::string name() const override { return "HasCoin"; }
};

class IdleState final : public VendingState {
public:
    void insertCoin(VendingMachine& machine) override {
        std::cout << "coin accepted\n";
        machine.setState(std::make_unique<HasCoinState>());
    }
    void selectItem(VendingMachine&) override {
        std::cout << "insert a coin first\n";
    }
    std::string name() const override { return "Idle"; }
};

int main() {
    std::cout << "State pattern example:\n";

    VendingMachine machine(std::make_unique<IdleState>());

    machine.selectItem();   // insert a coin first
    machine.insertCoin();   // coin accepted -> HasCoin
    machine.insertCoin();   // coin already inserted
    machine.selectItem();   // dispensing item -> Dispensing

    return 0;
}