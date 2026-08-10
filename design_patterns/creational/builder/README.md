# Builder

## Description

Builder is a creational design pattern that separates the construction of a complex
object from its final representation, so the same step-by-step construction process
can produce different representations of that object.

Instead of a single constructor (or a growing list of overloaded constructors) that
tries to handle every combination of optional parameters, a **Builder** accumulates
state across a sequence of calls, and only assembles the final product when asked.
An optional **Director** encodes a fixed build sequence, driving any builder through
the same steps without knowing which concrete variant it's producing.

Typical participants:

- **Product** — the complex object being built (e.g. `Burger`).
- **Builder** — the abstract interface for construction steps (e.g. `buildBun()`,
  `buildPatty()`, `buildToppings()`) plus a way to retrieve the finished product.
- **ConcreteBuilder** — implements the steps to produce one specific variant, and
  holds the in-progress product instance.
- **Director** (optional) — calls the builder's steps in a fixed order; used when the
  construction sequence itself is reusable across builders.

## When to Use

- An object requires many optional parameters or fields, and constructor overloads
  would multiply out of control (the "telescoping constructor" problem).
- Construction involves a meaningful **sequence** — steps that must happen in order,
  or that depend on validating/accumulating state as you go.
- The same step-by-step process should be reusable to produce **different variants**
  of a product (e.g. a classic vs. a veggie burger, using the same `Chef` sequence).
- You want to keep an object's constructor(s) simple by moving assembly logic
  (defaults, derived fields, validation) into a dedicated builder.
- You want a fluent, readable construction API (`builder.setBun(...).setPatty(...)`)
  rather than a long positional argument list.

## When to Avoid

- The object has few fields and a plain constructor or aggregate initialization
  (`Point{x, y}`) is already clear — Builder adds ceremony with no payoff.
- There's no meaningful order or interdependency between the construction steps —
  a factory function or simple constructor covers it.
- You only ever need one representation of the product — the "different variants"
  benefit of Builder doesn't apply, so a Director/Builder split is overkill.
- The extra Builder (and possibly Director) classes make the code harder to follow
  than the constructor they're replacing.

## How to Implement

1. **Define the Product.** The complex object being assembled, with setters/mutators
   for its parts.

   ```cpp
   class Burger {
   public:
       void setBun(const std::string& bun) { bun_ = bun; }
       void setPatty(const std::string& patty) { patty_ = patty; }
       void addTopping(const std::string& topping) { toppings_ += topping; }
       void describe() const { /* ... */ }

   private:
       std::string bun_;
       std::string patty_;
       std::string toppings_;
   };
   ```

2. **Define the Builder interface.** One method per construction step, expressed in
   domain terms, plus a method to hand over the finished product.

   ```cpp
   class BurgerBuilder {
   public:
       virtual ~BurgerBuilder() = default;
       virtual void buildBun() = 0;
       virtual void buildPatty() = 0;
       virtual void buildToppings() = 0;
       virtual std::unique_ptr<Burger> release() = 0;
   };
   ```

3. **Implement Concrete Builders.** Each one owns an in-progress product instance and
   fills it in according to its variant.

   ```cpp
   class ClassicBurgerBuilder final : public BurgerBuilder {
   public:
       ClassicBurgerBuilder() : burger_(std::make_unique<Burger>()) {}

       void buildBun() override { burger_->setBun("sesame"); }
       void buildPatty() override { burger_->setPatty("beef"); }
       void buildToppings() override {
           burger_->addTopping("lettuce");
           burger_->addTopping("tomato");
       }

       std::unique_ptr<Burger> release() override { return std::move(burger_); }

   private:
       std::unique_ptr<Burger> burger_;
   };
   ```

4. **Implement the Director (optional).** Encodes the fixed sequence of steps,
   independent of which concrete builder it's given.

   ```cpp
   class Chef {
   public:
       std::unique_ptr<Burger> make(BurgerBuilder& builder) {
           builder.buildBun();
           builder.buildPatty();
           builder.buildToppings();
           return builder.release();
       }
   };
   ```

5. **Write client code against the abstractions.** The client picks a concrete
   builder, but the assembly sequence itself is shared.

   ```cpp
   int main() {
       Chef chef;

       ClassicBurgerBuilder classicBuilder;
       auto classic = chef.make(classicBuilder);
       classic->describe();

       VeggieBurgerBuilder veggieBuilder;
       auto veggie = chef.make(veggieBuilder);
       veggie->describe();
   }
   ```

## Implementation Tips

- Keep builder methods in domain language (`setBun`, `addTopping`), not raw field
  setters — this is what makes Builder read better than a constructor at the call
  site.
- For a fluent API, have step methods `return *this;` (by reference) so calls can be
  chained: `builder.setBun("sesame").setPatty("beef").build();`
- Do assembly/validation work in the "release"/`build()` step so an incomplete or
  invalid product can never escape the builder.
- If you don't need multiple product variants and there's no reusable step sequence,
  drop the Director — a fluent builder alone is often enough.