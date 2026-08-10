# Abstract Factory

## Description

Abstract Factory is a creational design pattern that provides an interface for
creating **families of related objects** without specifying their concrete classes.
Client code depends only on the abstract factory and abstract products; it never
names a concrete product class, and it's guaranteed that everything one concrete
factory produces belongs to the same, mutually compatible family.

This differs from Factory Method in scope: Factory Method centers on one overridable
method producing one product; Abstract Factory bundles several related creation
methods behind one interface, so an entire family of products can be swapped by
swapping a single factory instance.

Typical participants:

- **AbstractProduct** — an interface for one kind of product in the family (e.g.
  `Button`, `Checkbox`).
- **ConcreteProduct** — a specific implementation, tied to a specific family (e.g.
  `LightButton`, `DarkCheckbox`).
- **AbstractFactory** — declares one creation method per product type in the family
  (e.g. `createButton()`, `createCheckbox()`).
- **ConcreteFactory** — implements all of those methods for one consistent family
  (e.g. `LightThemeFactory`, `DarkThemeFactory`).

## When to Use

- A system needs to stay independent of how its products are created, composed, and
  represented — it should work purely against interfaces.
- Products naturally come in **families that must be used together**, and mixing
  members from different families would be a bug (e.g. a light-theme button paired
  with a dark-theme checkbox, or a Windows-style widget paired with a Mac-style one).
- You want to be able to swap an entire family of related objects by swapping one
  factory — e.g. selecting a UI theme, a platform-specific driver set, or a database
  backend's family of related helper classes, all in one place.
- You want to enforce, structurally, that client code can't accidentally instantiate
  a mismatched combination of related objects.

## When to Avoid

- There's only one product, or the "family" doesn't actually contain multiple related
  products that need to travel together — Factory Method or a plain factory function
  is simpler.
- The set of product types changes frequently. Adding a new product type means adding
  a new method to the abstract factory interface *and* every concrete factory — this
  is Abstract Factory's biggest maintenance cost, so it's a poor fit for volatile
  product sets.
- The number of families is small and unlikely to grow, and a couple of `if`/config
  checks would be clearer than a full factory hierarchy.
- A simpler creational pattern (Factory Method, Builder, or even direct construction)
  already satisfies the actual variation you need.

## How to Implement

1. **Define the Abstract Products.** One interface per kind of product in the family.

   ```cpp
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
   ```

2. **Implement Concrete Products, grouped by family.** Each family gets its own
   implementation of every product type.

   ```cpp
   class LightButton final : public Button {
   public:
       void render() const override { std::cout << "light button\n"; }
   };

   class LightCheckbox final : public Checkbox {
   public:
       void render() const override { std::cout << "light checkbox\n"; }
   };

   // DarkButton, DarkCheckbox follow the same shape for the dark family.
   ```

3. **Define the Abstract Factory.** One creation method per product type.

   ```cpp
   class WidgetFactory {
   public:
       virtual ~WidgetFactory() = default;
       virtual std::unique_ptr<Button> createButton() const = 0;
       virtual std::unique_ptr<Checkbox> createCheckbox() const = 0;
   };
   ```

4. **Implement Concrete Factories, one per family.** Each concrete factory only ever
   returns products from its own family.

   ```cpp
   class LightThemeFactory final : public WidgetFactory {
   public:
       std::unique_ptr<Button> createButton() const override {
           return std::make_unique<LightButton>();
       }
       std::unique_ptr<Checkbox> createCheckbox() const override {
           return std::make_unique<LightCheckbox>();
       }
   };
   ```

5. **Write client code against the abstract factory and abstract products only.**

   ```cpp
   void renderForm(const WidgetFactory& factory) {
       auto button = factory.createButton();
       auto checkbox = factory.createCheckbox();
       button->render();
       checkbox->render();
   }

   int main() {
       LightThemeFactory lightFactory;
       DarkThemeFactory darkFactory;

       renderForm(lightFactory);
       renderForm(darkFactory);
   }
   ```

## Implementation Tips

- Keep each concrete factory strictly scoped to one consistent family — resist the
  temptation to let one factory method reach into another family "just this once."
- Client code should never construct a concrete product directly; it should always
  go through the factory interface, even for a single product.
- Pair Abstract Factory with dependency injection: choose and hand in the concrete
  factory once (at startup, or via configuration), and let the rest of the system run
  entirely against the abstract interfaces.
- If you find yourself adding a new product method to the interface often, that's a
  signal the product family is more volatile than Abstract Factory is suited for —
  consider Factory Method per product or a more data-driven approach instead.