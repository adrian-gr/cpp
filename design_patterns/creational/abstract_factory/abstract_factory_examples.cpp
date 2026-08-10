// Abstract Factory pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/creational/abstract_factory/abstract_factory_examples.cpp -o bin/abstract_factory_example && ./bin/abstract_factory_example

#include <iostream>
#include <memory>
#include <string>

// Description:
// Abstract Factory provides an interface for creating families of related objects
// without specifying their concrete classes. Where Factory Method produces one
// product via one overridable method, Abstract Factory produces several related
// products through one cohesive interface, guaranteeing the products it returns
// are mutually compatible.
//
// Use Abstract Factory when:
// - a system must stay independent of how its products are created and composed;
// - products come in families that must be used together (e.g. matching UI widgets
//   for a given theme, or matching drivers for a given platform);
// - you want to swap an entire family of related objects by swapping one factory.
//
// Avoid Abstract Factory when:
// - there's only one product, or the products in a "family" aren't actually related;
// - the set of product types changes often, forcing frequent edits to every factory
//   interface and every concrete factory (adding a new product is expensive here);
// - a simpler Factory Method or plain constructor already covers the variation.
//
// Tips:
// - Keep each concrete factory focused on producing one consistent family.
// - Client code should depend only on the abstract factory and abstract products.
// - Combine with Dependency Injection: pass in the concrete factory once at startup.

class Button {
public:
    virtual ~Button() = default;
    virtual void render() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual void render() const = 0;
};

class LightButton final : public Button {
public:
    void render() const override { std::cout << "light button\n"; }
};

class LightCheckbox final : public Checkbox {
public:
    void render() const override { std::cout << "light checkbox\n"; }
};

class DarkButton final : public Button {
public:
    void render() const override { std::cout << "dark button\n"; }
};

class DarkCheckbox final : public Checkbox {
public:
    void render() const override { std::cout << "dark checkbox\n"; }
};

class WidgetFactory {
public:
    virtual ~WidgetFactory() = default;
    virtual std::unique_ptr<Button> createButton() const = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() const = 0;
};

class LightThemeFactory final : public WidgetFactory {
public:
    std::unique_ptr<Button> createButton() const override {
        return std::make_unique<LightButton>();
    }
    std::unique_ptr<Checkbox> createCheckbox() const override {
        return std::make_unique<LightCheckbox>();
    }
};

class DarkThemeFactory final : public WidgetFactory {
public:
    std::unique_ptr<Button> createButton() const override {
        return std::make_unique<DarkButton>();
    }
    std::unique_ptr<Checkbox> createCheckbox() const override {
        return std::make_unique<DarkCheckbox>();
    }
};

void renderForm(const WidgetFactory& factory) {
    auto button = factory.createButton();
    auto checkbox = factory.createCheckbox();
    button->render();
    checkbox->render();
}

int main() {
    std::cout << "Abstract Factory pattern example:\n";

    LightThemeFactory lightFactory;
    DarkThemeFactory darkFactory;

    std::cout << "light theme:\n";
    renderForm(lightFactory);

    std::cout << "dark theme:\n";
    renderForm(darkFactory);

    return 0;
}