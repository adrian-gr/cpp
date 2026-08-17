// Decorator pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/decorator/decorator_examples.cpp -o bin/decorator_example && ./bin/decorator_example

#include <iostream>
#include <memory>
#include <string>

// Description:
// Decorator attaches additional responsibilities to an object dynamically by
// wrapping it in another object that implements the same interface. Decorators can
// be stacked, each adding behavior before and/or after delegating to the object it
// wraps, without altering the original object's class or affecting other instances.
//
// Use Decorator when:
// - you need to add responsibilities to individual objects at runtime, not to an
//   entire class, and want to avoid a combinatorial explosion of subclasses for
//   every combination of added behavior;
// - you want to add and remove responsibilities dynamically, or compose them in
//   different orders depending on the situation;
// - extending via subclassing would be impractical (too many combinations) or
//   impossible (the class is closed for extension).
//
// Avoid Decorator when:
// - the set of extensions is small, fixed, and unlikely to grow — a couple of
//   subclasses or optional constructor parameters may be clearer;
// - stacking many decorators would make behavior hard to trace or reason about,
//   and that opacity costs more than the flexibility gained;
// - the added behavior actually changes object identity or type in ways client
//   code needs to detect — decorators are meant to be transparent wrappers.
//
// Tips:
// - Keep decorators implementing the exact same interface as the object they wrap,
//   so wrapped and unwrapped objects are interchangeable to the client.
// - Have each decorator hold a reference to the wrapped object and delegate to it,
//   adding its own behavior before and/or after the delegated call.
// - Order matters when stacking decorators; document or make explicit what each
//   layer assumes about layers beneath it.

class Beverage {
public:
    virtual ~Beverage() = default;
    virtual std::string description() const = 0;
    virtual double cost() const = 0;
};

class Espresso final : public Beverage {
public:
    std::string description() const override { return "Espresso"; }
    double cost() const override { return 2.00; }
};

// Base decorator: implements the same interface and wraps a Beverage.
class BeverageDecorator : public Beverage {
public:
    explicit BeverageDecorator(std::unique_ptr<Beverage> beverage) : beverage_(std::move(beverage)) {}

protected:
    std::unique_ptr<Beverage> beverage_;
};

class MilkDecorator final : public BeverageDecorator {
public:
    using BeverageDecorator::BeverageDecorator;

    std::string description() const override {
        return beverage_->description() + " + Milk";
    }
    double cost() const override {
        return beverage_->cost() + 0.50;
    }
};

class CaramelDecorator final : public BeverageDecorator {
public:
    using BeverageDecorator::BeverageDecorator;

    std::string description() const override {
        return beverage_->description() + " + Caramel";
    }
    double cost() const override {
        return beverage_->cost() + 0.75;
    }
};

int main() {
    std::cout << "Decorator pattern example:\n";

    std::unique_ptr<Beverage> order = std::make_unique<Espresso>();
    std::cout << order->description() << ": $" << order->cost() << "\n";

    order = std::make_unique<MilkDecorator>(std::move(order));
    std::cout << order->description() << ": $" << order->cost() << "\n";

    order = std::make_unique<CaramelDecorator>(std::move(order));
    std::cout << order->description() << ": $" << order->cost() << "\n";

    return 0;
}