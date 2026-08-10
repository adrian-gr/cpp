// Builder pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/creational/builder_examples.cpp -o bin/builder_example && ./bin/builder_example

#include <iostream>
#include <memory>
#include <string>

// Description:
// Builder separates the construction of a complex object from its representation,
// so the same construction process can produce different representations.
// A director drives the build steps; a builder accumulates state and produces the
// final product.
//
// Use Builder when:
// - an object requires many optional parameters or a specific construction sequence;
// - you want to avoid telescoping constructors (many overloads with default params);
// - the same step-by-step process should be able to yield different product variants.
//
// Avoid Builder when:
// - the object has few fields and a plain constructor or aggregate init is clear enough;
// - there's no meaningful construction sequence or validation between steps;
// - the extra Builder/Director classes add ceremony without simplifying call sites.
//
// Tips:
// - Keep the builder's interface expressed in domain terms (setBun, addPatty), not raw fields.
// - Return *this by reference from setter-style methods to support fluent chaining.
// - Have the builder validate/assemble in build(), so an incomplete product can't leak out.

class Burger {
public:
    void setBun(const std::string& bun) { bun_ = bun; }
    void setPatty(const std::string& patty) { patty_ = patty; }
    void addTopping(const std::string& topping) { toppings_ += (toppings_.empty() ? "" : ", ") + topping; }

    void describe() const {
        std::cout << bun_ << " bun, " << patty_ << " patty, toppings: "
                  << (toppings_.empty() ? "none" : toppings_) << "\n";
    }

private:
    std::string bun_;
    std::string patty_;
    std::string toppings_;
};

class BurgerBuilder {
public:
    virtual ~BurgerBuilder() = default;
    virtual void buildBun() = 0;
    virtual void buildPatty() = 0;
    virtual void buildToppings() = 0;
    virtual std::unique_ptr<Burger> release() = 0;
};

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

class VeggieBurgerBuilder final : public BurgerBuilder {
public:
    VeggieBurgerBuilder() : burger_(std::make_unique<Burger>()) {}

    void buildBun() override { burger_->setBun("whole wheat"); }
    void buildPatty() override { burger_->setPatty("black bean"); }
    void buildToppings() override {
        burger_->addTopping("avocado");
        burger_->addTopping("sprouts");
    }

    std::unique_ptr<Burger> release() override { return std::move(burger_); }

private:
    std::unique_ptr<Burger> burger_;
};

// Director drives the fixed build sequence; the builder decides what each step produces.
class Chef {
public:
    std::unique_ptr<Burger> make(BurgerBuilder& builder) {
        builder.buildBun();
        builder.buildPatty();
        builder.buildToppings();
        return builder.release();
    }
};

int main() {
    std::cout << "Builder pattern example:\n";

    Chef chef;

    ClassicBurgerBuilder classicBuilder;
    auto classic = chef.make(classicBuilder);
    classic->describe();

    VeggieBurgerBuilder veggieBuilder;
    auto veggie = chef.make(veggieBuilder);
    veggie->describe();

    return 0;
}
