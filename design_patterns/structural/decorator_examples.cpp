// Decorator pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/decorator_examples.cpp -o bin/decorator_example && ./bin/decorator_example

#include <iostream>
#include <memory>
#include <string>
#include <utility>

// Description:
// Decorator adds responsibilities to an object dynamically by wrapping it
// with another object that implements the same interface.
//
// Use Decorator when:
// - behavior should be added without changing the wrapped class;
// - features should be combined in different configurations;
// - subclassing would create too many combinations of optional behavior.
//
// Avoid Decorator when:
// - the behavior is always required and belongs in the component itself;
// - many layers would make execution order difficult to understand;
// - clients need to depend on concrete implementation details.
//
// Tips:
// - Keep the decorator interface identical to the component interface.
// - Prefer ownership through smart pointers when decorators form a chain.
// - Keep each decorator focused on one optional responsibility.

class Beverage {
public:
    virtual ~Beverage() = default;
    virtual double cost() const = 0;
    virtual std::string description() const = 0;
};

class Coffee final : public Beverage {
public:
    double cost() const override {
        return 2.00;
    }

    std::string description() const override {
        return "coffee";
    }
};

class BeverageDecorator : public Beverage {
public:
    explicit BeverageDecorator(std::unique_ptr<Beverage> beverage)
        : beverage_(std::move(beverage)) {}

protected:
    const Beverage& beverage() const {
        return *beverage_;
    }

private:
    std::unique_ptr<Beverage> beverage_;
};

class Milk final : public BeverageDecorator {
public:
    using BeverageDecorator::BeverageDecorator;

    double cost() const override {
        return beverage().cost() + 0.50;
    }

    std::string description() const override {
        return beverage().description() + ", milk";
    }
};

class Sugar final : public BeverageDecorator {
public:
    using BeverageDecorator::BeverageDecorator;

    double cost() const override {
        return beverage().cost() + 0.25;
    }

    std::string description() const override {
        return beverage().description() + ", sugar";
    }
};

int main() {
    std::cout << "Decorator pattern example:\n";

    std::unique_ptr<Beverage> beverage = std::make_unique<Sugar>(
        std::make_unique<Milk>(std::make_unique<Coffee>()));

    std::cout << beverage->description() << ": $" << beverage->cost() << "\n";

    return 0;
}
