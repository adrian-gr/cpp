# State

## Description

State is a behavioral design pattern that lets an object alter its behavior when
its internal state changes, so that the object appears to change its class.
Behavior that would otherwise live in a large conditional — branching on a "current
state" field, repeated across many methods — is instead split across a family of
state classes, each implementing the behavior appropriate to exactly one state.

The object whose behavior varies (the **context**) holds a reference to its current
state object and simply delegates to it. Each concrete state implements the
behavior for that state, and typically also decides what the next state should be,
handing the context off to it.

Typical participants:

- **Context** — the object whose behavior varies by state; delegates calls to its
  current state object (e.g. `VendingMachine`).
- **State** — declares the interface common to all states (e.g. `VendingState`).
- **ConcreteState** — implements behavior for one specific state, and usually
  triggers the transition to the next state (e.g. `IdleState`, `HasCoinState`,
  `DispensingState`).

## When to Use

- An object's behavior depends heavily on its current state, and that behavior
  needs to change at runtime as the state changes — not just once at construction.
- You have large conditionals (`switch`/`if-else` on a state enum) scattered across
  many methods, all branching on the same underlying state field — a sign the
  state-specific logic wants to be pulled out into its own classes.
- Transitions between states follow clear rules that read more cleanly as one state
  object explicitly handing off to the next, rather than as scattered flags and
  branches checked all over the context class.
- You're modeling something naturally state-machine-like: a vending machine, a
  connection (connecting/connected/disconnected), an order lifecycle
  (placed/shipped/delivered/cancelled), a UI component with distinct interaction
  modes.

## When to Avoid

- There are only two or three simple states with trivial, rarely-changing behavior
  — a boolean flag or a small `switch` on an enum is easier to follow than a
  class-per-state hierarchy.
- The "states" don't actually change *behavior*, only *data* — that's not a case for
  State; it's just varying fields, and a plain struct/enum covers it.
- The state machine is trivial enough that the overhead of one class per state
  outweighs the clarity gained over a straightforward conditional — don't reach for
  State just because a `switch` exists.

## How to Implement

1. **Define the State interface.** One method per behavior that varies by state.

   ```cpp
   class VendingState {
   public:
       virtual ~VendingState() = default;
       virtual void insertCoin(VendingMachine& machine) = 0;
       virtual void selectItem(VendingMachine& machine) = 0;
   };
   ```

2. **Implement the Context.** Holds the current state and delegates every relevant
   call to it. Also exposes a way for states to trigger a transition.

   ```cpp
   class VendingMachine {
   public:
       explicit VendingMachine(std::unique_ptr<VendingState> initialState)
           : state_(std::move(initialState)) {}

       void insertCoin() { state_->insertCoin(*this); }
       void selectItem() { state_->selectItem(*this); }

       void setState(std::unique_ptr<VendingState> newState) {
           state_ = std::move(newState);
       }

   private:
       std::unique_ptr<VendingState> state_;
   };
   ```

3. **Implement Concrete States.** Each handles behavior for one state, and
   transitions the context to the next state when appropriate.

   ```cpp
   class IdleState final : public VendingState {
   public:
       void insertCoin(VendingMachine& machine) override {
           machine.setState(std::make_unique<HasCoinState>());
       }
       void selectItem(VendingMachine&) override {
           std::cout << "insert a coin first\n";
       }
   };

   class HasCoinState final : public VendingState {
   public:
       void insertCoin(VendingMachine&) override {
           std::cout << "coin already inserted\n";
       }
       void selectItem(VendingMachine& machine) override {
           machine.setState(std::make_unique<DispensingState>());
       }
   };
   ```

4. **Drive behavior through the context's stable public interface.** The caller
   never checks or branches on the current state itself.

   ```cpp
   VendingMachine machine(std::make_unique<IdleState>());

   machine.selectItem();   // "insert a coin first" (Idle handles this)
   machine.insertCoin();   // transitions Idle -> HasCoin
   machine.selectItem();   // transitions HasCoin -> Dispensing
   ```

## Implementation Tips

- Let each state decide the next state, rather than centralizing all transition
  logic inside the context — this keeps each transition rule local to the state
  where it's triggered, instead of one large transition table the context has to
  maintain.
- Keep the context's public interface stable; only its internal state object
  changes over time. Client code should never need to know which concrete state
  the context currently holds.
- Consider making stateless states singletons (shared instances with no
  per-instance data) if none of them hold state of their own — this avoids
  reallocating a new state object on every transition.
- If transitions can fail or need validation, decide clearly whether that
  validation lives in the state (rejecting invalid actions with a message, as
  above) or in the context (refusing to call `setState` under certain conditions)
  — and be consistent about it across all states.
- Don't confuse State with Strategy: they look structurally similar (a context
  delegating to an interchangeable object), but states typically know about and
  transition to other states, while strategies are usually swapped in from outside
  by the context or client, with no strategy aware of any other.