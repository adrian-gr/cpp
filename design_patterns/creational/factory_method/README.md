# Factory Method

## Description

Factory Method is a creational design pattern that defines an interface (or virtual
function) for creating an object, but lets subclasses — or overridable creator
functions — decide which concrete class to instantiate. The client code depends only
on the abstract product and the abstract creator; it never names a concrete class
directly.

In short: the *what to create* decision is pushed down into overridable creation
logic, while the *how it's used* logic stays fixed in the base/creator class.

Typical participants:

- **Product** — the abstract interface all created objects share (e.g. `Notifier`).
- **ConcreteProduct** — a specific implementation (e.g. `EmailNotifier`, `SmsNotifier`).
- **Creator** — declares the factory method (`createNotifier()`) and usually contains
  other logic that uses the product (`notify()`).
- **ConcreteCreator** — overrides the factory method to return a specific
  `ConcreteProduct`.

## When to Use

- A class can't know ahead of time which concrete subclass of an object it needs to
  create — the decision belongs to a subclass, a config value, or a caller.
- You want to defer or centralize instantiation logic (setup steps, defaults,
  validation) instead of scattering `new`/constructor calls throughout the codebase.
- You're building a framework or library and want to give users an extension point:
  they subclass your creator and override the factory method to plug in their own
  types, without touching your framework code.
- You have parallel class hierarchies that must stay in lockstep (e.g. `Document` /
  `Application`), and each concrete creator should always produce the matching
  concrete product.
- You want to substitute a real object with a test double (mock/fake) by overriding
  the factory method in a test subclass, while all client code stays unchanged.

## When to Avoid

- There's only one concrete product and no realistic prospect of adding more — a
  constructor call is simpler and clearer.
- The variation is just a runtime parameter (a string, enum, or config flag), not a
  need for polymorphic, subclassable creation — a plain factory *function* with a
  switch/map is lighter weight than a whole creator class hierarchy.
- The "factory method" would end up containing substantial business logic rather than
  just picking and constructing a product — that's a sign the abstraction is being
  stretched to cover something else.
- The extra class hierarchy adds indirection without real benefit, making the code
  harder to navigate for no payoff.

## How to Implement

1. **Define the Product interface.** Declare the operations every concrete product
   must support.

   ```cpp
   class Notifier {
   public:
       virtual ~Notifier() = default;
       virtual void send(const std::string& message) = 0;
   };
   ```

2. **Implement Concrete Products.** One class per variant.

   ```cpp
   class EmailNotifier final : public Notifier {
   public:
       void send(const std::string& message) override {
           std::cout << "email: " << message << "\n";
       }
   };

   class SmsNotifier final : public Notifier {
   public:
       void send(const std::string& message) override {
           std::cout << "sms: " << message << "\n";
       }
   };
   ```

3. **Define the Creator with the factory method.** Put any shared logic that *uses*
   the product in a non-virtual (or non-overridden) method; the factory method itself
   stays abstract (or has a sensible default).

   ```cpp
   class NotifierCreator {
   public:
       virtual ~NotifierCreator() = default;

       void notify(const std::string& message) {
           std::unique_ptr<Notifier> notifier = createNotifier();
           notifier->send(message);
       }

   protected:
       virtual std::unique_ptr<Notifier> createNotifier() = 0;
   };
   ```

4. **Implement Concrete Creators.** Each overrides the factory method to return its
   matching product.

   ```cpp
   class EmailNotifierCreator final : public NotifierCreator {
   protected:
       std::unique_ptr<Notifier> createNotifier() override {
           return std::make_unique<EmailNotifier>();
       }
   };

   class SmsNotifierCreator final : public NotifierCreator {
   protected:
       std::unique_ptr<Notifier> createNotifier() override {
           return std::make_unique<SmsNotifier>();
       }
   };
   ```

5. **Write client code against the abstractions only.** It should never reference a
   concrete product or creator by name.

   ```cpp
   void alertUser(NotifierCreator& creator) {
       creator.notify("your order has shipped");
   }

   int main() {
       EmailNotifierCreator emailCreator;
       SmsNotifierCreator smsCreator;

       alertUser(emailCreator);
       alertUser(smsCreator);
   }
   ```

## Implementation Tips

- Keep the product interface minimal — only what clients actually need.
- Prefer returning `std::unique_ptr` (or your language's equivalent smart/owned
  pointer) from factory methods for clear, unambiguous ownership.
- If the set of products is data-driven or grows frequently, consider pairing Factory
  Method with a registry/map of creator callables instead of adding a new subclass
  for every variant.
- Don't let the factory method grow business logic — its only job is to pick and
  construct the right product.