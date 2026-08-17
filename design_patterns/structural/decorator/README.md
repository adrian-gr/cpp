# Decorator

## Description

Decorator is a structural design pattern that attaches additional responsibilities
to an object dynamically, by wrapping it in another object that implements the same
interface. Decorators can be stacked — each one adds behavior before and/or after
delegating to the object it wraps — without altering the original object's class,
and without affecting any other instance of that class.

Because a decorator implements the exact same interface it wraps, wrapped and
unwrapped objects are interchangeable everywhere that interface is expected. This
is what allows decorators to be composed in a chain: a decorator wrapping an object
is itself a valid instance of that same interface, so another decorator can wrap
*it* in turn.

Typical participants:

- **Component** — the shared interface for both plain objects and decorated ones
  (e.g. `Beverage`).
- **ConcreteComponent** — the original, undecorated object (e.g. `Espresso`).
- **Decorator** — implements the Component interface and holds a reference to a
  wrapped Component, forming the base for concrete decorators (e.g.
  `BeverageDecorator`).
- **ConcreteDecorator** — adds one specific responsibility, delegating to the
  wrapped object and layering its own behavior on top (e.g. `MilkDecorator`,
  `CaramelDecorator`).

## When to Use

- You need to add responsibilities to *individual objects* at runtime, not to an
  entire class — and you want to avoid a combinatorial explosion of subclasses for
  every possible combination of added behavior (e.g. avoiding
  `EspressoWithMilkAndCaramel`, `EspressoWithCaramelOnly`, and so on).
- You want to add and remove responsibilities dynamically, or compose them in
  different orders depending on the situation, rather than fixing every combination
  at compile time via inheritance.
- Extending via subclassing would be impractical — too many possible combinations
  — or impossible, because the class you'd want to extend is closed for extension
  (e.g. from a library you can't modify).

## When to Avoid

- The set of extensions is small, fixed, and unlikely to grow — a couple of
  subclasses, or optional constructor parameters/flags, may be clearer than
  introducing the decorator machinery.
- Stacking many decorators would make behavior hard to trace or reason about —
  deeply nested decorator chains can obscure what actually happens when a call is
  made, and that opacity can cost more than the flexibility it buys.
- The added behavior actually needs to change object identity or type in ways
  client code needs to detect (e.g. `dynamic_cast` checks on the decorated type) —
  decorators are meant to be transparent wrappers, and relying on identity checks
  works against that.

## How to Implement

1. **Define the Component interface.** Shared by both plain and decorated objects.

   ```cpp
   class Beverage {
   public:
       virtual ~Beverage() = default;
       virtual std::string description() const = 0;
       virtual double cost() const = 0;
   };
   ```

2. **Implement the Concrete Component.** The original, undecorated object.

   ```cpp
   class Espresso final : public Beverage {
   public:
       std::string description() const override { return "Espresso"; }
       double cost() const override { return 2.00; }
   };
   ```

3. **Define the base Decorator.** Implements the same interface and holds a
   wrapped Component.

   ```cpp
   class BeverageDecorator : public Beverage {
   public:
       explicit BeverageDecorator(std::unique_ptr<Beverage> beverage)
           : beverage_(std::move(beverage)) {}

   protected:
       std::unique_ptr<Beverage> beverage_;
   };
   ```

4. **Implement Concrete Decorators.** Each delegates to the wrapped object, then
   adds its own contribution.

   ```cpp
   class MilkDecorator final : public BeverageDecorator {
   public:
       using BeverageDecorator::BeverageDecorator;

       std::string description() const override { return beverage_->description() + " + Milk"; }
       double cost() const override { return beverage_->cost() + 0.50; }
   };
   ```

5. **Stack decorators at the point of use.** No `EspressoWithMilkAndCaramel`
   subclass is ever needed.

   ```cpp
   std::unique_ptr<Beverage> order = std::make_unique<Espresso>();
   order = std::make_unique<MilkDecorator>(std::move(order));
   order = std::make_unique<CaramelDecorator>(std::move(order));

   std::cout << order->description() << ": $" << order->cost() << "\n";
   // "Espresso + Milk + Caramel: $3.25"
   ```

## Implementation Tips

- Keep decorators implementing the exact same interface as the object they wrap,
  so wrapped and unwrapped objects remain interchangeable to any client code —
  this is what makes the whole pattern transparent.
- Have each decorator hold a reference to the wrapped object and delegate to it,
  adding its own behavior before and/or after the delegated call — most decorators
  should call through to the wrapped object rather than reimplementing its
  behavior.
- Order matters when stacking decorators (e.g. a logging decorator around a
  caching decorator behaves differently than the reverse) — document or make
  explicit what each layer assumes about the layers beneath it.
- If many combinations of decorators are common and predictable, consider
  providing convenience factory functions that build the typical stacks, so
  callers don't need to hand-assemble the same chains repeatedly.
- Don't confuse Decorator with Proxy: both wrap an object behind the same
  interface, but Decorator's purpose is to *add* behavior/responsibilities,
  usually stacked in varying combinations, while Proxy's purpose is to *control
  access* to the wrapped object (lazy loading, access control, remote calls) and
  is typically a single, purpose-specific wrapper rather than a composable stack.