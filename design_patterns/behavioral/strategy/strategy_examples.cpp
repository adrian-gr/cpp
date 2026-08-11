// Strategy pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/strategy/strategy_examples.cpp -o bin/strategy_example && ./bin/strategy_example

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Strategy defines a family of interchangeable algorithms, encapsulates each one,
// and lets them be swapped independently of the code that uses them. The context
// holds a reference to a strategy and delegates the algorithm-specific work to it,
// without knowing which concrete strategy it's using.
//
// Use Strategy when:
// - you have several variants of an algorithm and want to switch between them at
//   runtime, or configure which one a client uses without touching the client code;
// - a class has a large conditional selecting between behavior variants, and that
//   conditional would be cleaner as separate, swappable classes;
// - you want to isolate algorithm-specific logic and data from the code that uses
//   the result, so each can change independently.
//
// Avoid Strategy when:
// - there's only one algorithm variant, or variants never change at runtime — a
//   direct implementation is simpler than an interface plus one concrete class;
// - the algorithms are trivial one-liners — the overhead of separate classes isn't
//   worth it compared to a small conditional or even a plain function pointer;
// - the strategies would need to share so much context/state with each other that
//   they're not really independent, interchangeable algorithms anymore.
//
// Tips:
// - Keep the strategy interface narrow — ideally a single method describing the
//   algorithm's contract.
// - Let the client (or a factory) choose the concrete strategy; the context should
//   stay agnostic to which one it's given.
// - Prefer passing required data into the strategy method rather than giving every
//   strategy a back-reference to the whole context.

struct Order {
    std::string item;
    double price;
};

class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double computeTotal(const std::vector<Order>& orders) const = 0;
};

class RegularPricing final : public PricingStrategy {
public:
    double computeTotal(const std::vector<Order>& orders) const override {
        double total = 0.0;
        for (const auto& order : orders) {
            total += order.price;
        }
        return total;
    }
};

class BulkDiscountPricing final : public PricingStrategy {
public:
    double computeTotal(const std::vector<Order>& orders) const override {
        double total = 0.0;
        for (const auto& order : orders) {
            total += order.price;
        }
        if (orders.size() >= 3) {
            total *= 0.9;   // 10% off for 3+ items
        }
        return total;
    }
};

class LoyaltyMemberPricing final : public PricingStrategy {
public:
    double computeTotal(const std::vector<Order>& orders) const override {
        double total = 0.0;
        for (const auto& order : orders) {
            total += order.price;
        }
        return total * 0.85;   // flat 15% off for loyalty members
    }
};

// Context: delegates the pricing algorithm to whichever strategy it's given.
class Checkout {
public:
    explicit Checkout(std::unique_ptr<PricingStrategy> strategy) : strategy_(std::move(strategy)) {}

    void setStrategy(std::unique_ptr<PricingStrategy> strategy) {
        strategy_ = std::move(strategy);
    }

    double total(const std::vector<Order>& orders) const {
        return strategy_->computeTotal(orders);
    }

private:
    std::unique_ptr<PricingStrategy> strategy_;
};

int main() {
    std::cout << "Strategy pattern example:\n";

    std::vector<Order> orders = {{"book", 12.0}, {"pen", 2.0}, {"notebook", 5.0}};

    Checkout checkout(std::make_unique<RegularPricing>());
    std::cout << "regular: " << checkout.total(orders) << "\n";

    checkout.setStrategy(std::make_unique<BulkDiscountPricing>());
    std::cout << "bulk discount: " << checkout.total(orders) << "\n";

    checkout.setStrategy(std::make_unique<LoyaltyMemberPricing>());
    std::cout << "loyalty member: " << checkout.total(orders) << "\n";

    return 0;
}