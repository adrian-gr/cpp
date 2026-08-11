# Mediator

## Description

Mediator is a behavioral design pattern that defines an object encapsulating how a
set of other objects interact. Instead of "colleague" objects referencing and
calling each other directly — which tends to produce a tangled, many-to-many web of
dependencies — each colleague talks only to the mediator, and the mediator
coordinates the interaction between them.

This centralizes interaction logic that would otherwise be smeared across every
participant, and lets colleagues stay decoupled from one another: each only needs
to know the mediator's interface, not the full set of other colleagues it might
need to talk to.

Typical participants:

- **Mediator** — declares the interface colleagues use to communicate through it
  (e.g. `ChatMediator`).
- **ConcreteMediator** — implements the coordination logic, typically holding
  references to all the colleagues it manages and routing interactions between them
  (e.g. `ChatRoom`).
- **Colleague** — an object that communicates with other colleagues only through
  the mediator, never directly (e.g. `User`).

## When to Use

- A set of objects communicate in complex, tangled ways, and you want to centralize
  that interaction logic in one place rather than scattering references and calls
  across every participant.
- Reusing a colleague class elsewhere is difficult because it's tightly coupled to
  many other specific classes — Mediator breaks that coupling by routing everything
  through one shared coordinator.
- You want to change *how* a set of objects interact — the rules of coordination —
  without modifying the objects themselves; changing the mediator's logic is enough.
- You're modeling something naturally centralized: a chat room routing messages, an
  air traffic control tower coordinating planes, a dialog box coordinating widget
  interactions (enabling/disabling fields based on other fields' state).

## When to Avoid

- Only two objects interact, or the interactions are simple — a direct call between
  them is clearer than routing everything through an intermediary.
- The mediator itself risks becoming a bloated "god object" that knows too much
  about every colleague's internal behavior, rather than just their coordination —
  that's a sign responsibilities are leaking into the wrong place.
- The communication pattern between objects is stable, well-understood, and
  unlikely to ever need central control or reconfiguration — the indirection isn't
  earning its keep.
- Centralizing coordination would create a single point of failure or a
  performance bottleneck for interactions that are naturally independent of each
  other.

## How to Implement

1. **Define the Mediator interface.** Declares how colleagues communicate through
   it.

   ```cpp
   class ChatMediator {
   public:
       virtual ~ChatMediator() = default;
       virtual void sendMessage(const std::string& message, class User* sender) = 0;
   };
   ```

2. **Define the Colleague class.** Holds a reference to the mediator, and routes
   all outgoing communication through it — never directly to another colleague.

   ```cpp
   class User {
   public:
       User(ChatMediator& mediator, std::string name)
           : mediator_(mediator), name_(std::move(name)) {}

       void send(const std::string& message) {
           mediator_.sendMessage(message, this);
       }

       virtual void receive(const std::string& sender, const std::string& message) {
           std::cout << name_ << " received from " << sender << ": " << message << "\n";
       }

   private:
       ChatMediator& mediator_;
       std::string name_;
   };
   ```

3. **Implement the Concrete Mediator.** Holds references to all colleagues it
   coordinates, and implements the actual routing/coordination logic.

   ```cpp
   class ChatRoom final : public ChatMediator {
   public:
       void addUser(User* user) { users_.push_back(user); }

       void sendMessage(const std::string& message, User* sender) override {
           for (User* user : users_) {
               if (user != sender) {
                   user->receive(sender->name(), message);
               }
           }
       }

   private:
       std::vector<User*> users_;
   };
   ```

4. **Wire colleagues to the mediator, and let them communicate through it.**

   ```cpp
   ChatRoom chatRoom;

   User alice(chatRoom, "Alice");
   User bob(chatRoom, "Bob");

   chatRoom.addUser(&alice);
   chatRoom.addUser(&bob);

   alice.send("Hey everyone!");   // routed through ChatRoom, not called on bob directly
   ```

## Implementation Tips

- Keep colleagues genuinely unaware of each other — they should only hold a
  reference to the mediator interface, never to specific sibling colleagues. This
  is what makes colleague classes reusable outside this particular mediator.
- Give the mediator a narrow, coordination-only responsibility. If it starts
  implementing business logic that really belongs inside a colleague, that logic
  should move back out.
- Consider an event/notify-style interface (colleagues call a generic
  `notify(event, sender)` rather than many specific methods) so new colleague types
  or interactions can be added without repeatedly changing the mediator's public
  surface.
- If the mediator's coordination logic grows complex enough to need its own
  sub-strategies (e.g. different routing rules per message type), consider
  combining Mediator with Strategy rather than letting the mediator's single method
  balloon into a large conditional.
- A mediator can itself be an Observer subject, or notify observers, if colleagues
  need loosely-coupled outside listeners as well as tightly-coordinated
  colleague-to-colleague routing.