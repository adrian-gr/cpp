# Strategy

## Description

Strategy is a behavioral design pattern that defines a family of interchangeable
algorithms, encapsulates each one in its own class, and lets them be swapped
independently of the code that uses them. The **context** holds a reference to a
strategy object and delegates the algorithm-specific work to it — the context never
implements the algorithm itself, and never knows which concrete strategy it's
currently using.

This turns "pick one of several ways to do X" from a conditional embedded in the
context's own code into a set of interchangeable, independently testable classes
that share one interface.

Typical participants:

- **Strategy** — declares the interface common to all supported algorithm variants
  (e.g. `PricingStrategy`).
- **ConcreteStrategy** — implements one specific variant of the algorithm (e.g.
  `RegularPricing`, `BulkDiscountPricing`, `LoyaltyMemberPricing`).
- **Context** — holds a reference to the current strategy and delegates to it,
  exposing a way for the strategy to be set or changed from outside (e.g.
  `Checkout`).

## When to Use

- You have several variants of an algorithm and want to switch between them at
  runtime, or configure which variant a client uses without modifying the client's
  code.
- A class has a large conditional (`switch`/`if-else`) selecting between behavior
  variants, and that conditional would be cleaner expressed as separate, swappable
  classes implementing a shared interface.
- You want to isolate algorithm-specific logic and data from the code that consumes
  the result, so the algorithm and its caller can change independently — new
  strategies can be added without touching the context.
- You're modeling pluggable behavior: pricing rules, sorting/comparison logic,
  validation rules, compression algorithms, route-finding strategies — anywhere
  "how we do X" needs to vary independently of "that we do X."

## When to Avoid

- There's only one algorithm variant, or variants never change at runtime — a
  direct implementation inside the context is simpler than an interface plus one
  concrete class.
- The algorithm variants are trivial one-liners — the overhead of separate classes
  usually isn't worth it compared to a small conditional or even a plain function
  pointer/lambda.
- The "strategies" would need to share so much context or mutable state with each
  other that they aren't really independent, interchangeable algorithms anymore —
  that's a sign the abstraction doesn't fit the actual coupling between them.

## How to Implement

1. **Define the Strategy interface.** Ideally a single method describing the
   algorithm's contract.

   ```cpp
   class PricingStrategy {
   public:
       virtual ~PricingStrategy() = default;
       virtual double computeTotal(const std::vector<Order>& orders) const = 0;
   };
   ```

2. **Implement Concrete Strategies.** Each is one interchangeable variant of the
   algorithm.

   ```cpp
   class RegularPricing final : public PricingStrategy {
   public:
       double computeTotal(const std::vector<Order>& orders) const override {
           double total = 0.0;
           for (const auto& order : orders) total += order.price;
           return total;
       }
   };

   class BulkDiscountPricing final : public PricingStrategy {
   public:
       double computeTotal(const std::vector<Order>& orders) const override {
           double total = 0.0;
           for (const auto& order : orders) total += order.price;
           if (orders.size() >= 3) total *= 0.9;
           return total;
       }
   };
   ```

3. **Implement the Context.** Holds the current strategy and delegates to it;
   exposes a way to change strategy from outside.

   ```cpp
   class Checkout {
   public:
       explicit Checkout(std::unique_ptr<PricingStrategy> strategy)
           : strategy_(std::move(strategy)) {}

       void setStrategy(std::unique_ptr<PricingStrategy> strategy) {
           strategy_ = std::move(strategy);
       }

       double total(const std::vector<Order>& orders) const {
           return strategy_->computeTotal(orders);
       }

   private:
       std::unique_ptr<PricingStrategy> strategy_;
   };
   ```

4. **Choose and swap strategies from the client.** The context stays agnostic to
   which concrete strategy it's given.

   ```cpp
   Checkout checkout(std::make_unique<RegularPricing>());
   std::cout << checkout.total(orders) << "\n";

   checkout.setStrategy(std::make_unique<BulkDiscountPricing>());
   std::cout << checkout.total(orders) << "\n";
   ```

## Implementation Tips

- Keep the strategy interface narrow — ideally a single method describing the
  algorithm's contract. A bloated strategy interface makes every concrete strategy
  implement methods it doesn't actually need.
- Let the client (or a factory) choose the concrete strategy; the context should
  stay entirely agnostic to which one it's given, and should never branch on the
  strategy's concrete type.
- Prefer passing the data a strategy needs directly into its method (as with
  `computeTotal(orders)`) rather than giving every strategy a back-reference to the
  whole context — this keeps strategies focused and easier to test in isolation.
- If strategies are stateless, consider sharing single instances (or using free
  functions/lambdas instead of a full class hierarchy) rather than constructing a
  new strategy object on every swap.
- Don't confuse Strategy with State: they look structurally similar (a context
  delegating to an interchangeable object), but strategies are typically swapped in
  from outside and know nothing about each other, while states often transition to
  one another as part of their own logic.