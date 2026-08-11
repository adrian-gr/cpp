# Visitor

## Description

Visitor is a behavioral design pattern that lets you define a new operation over a
set of related element classes without modifying those classes. Each element
exposes a single `accept(visitor)` method; calling it dispatches — via **double
dispatch** — to the matching `visit()` overload on the visitor for that element's
concrete type. All operation-specific logic lives in the visitor, not scattered
across every element class.

"Double dispatch" is the mechanism that makes this work in a single-dispatch
language like C++: `accept()` is a virtual call that resolves to the correct
element's override first (dispatch #1, on the element's type), and inside that
override, calling `visitor.visit(*this)` resolves to the correct overload based on
the now-concrete `*this` type (dispatch #2, on the operation for that type).

Typical participants:

- **Element** — declares `accept(Visitor&)`, the sole hook elements expose to
  visitors (e.g. `Shape`).
- **ConcreteElement** — implements `accept()` by calling back into the visitor with
  its own concrete type (e.g. `Circle`, `Square`).
- **Visitor** — declares one `visit()` overload per concrete element type (e.g.
  `ShapeVisitor`).
- **ConcreteVisitor** — implements one operation across all element types (e.g.
  `AreaVisitor`, `DescribeVisitor`).

## When to Use

- You need to perform several unrelated operations over a stable set of element
  classes, and don't want to pollute each element with a method for every
  operation — Visitor keeps operation logic out of the elements entirely.
- The operation logic changes or grows more often than the element hierarchy
  itself — adding a new operation is just adding a new `ConcreteVisitor`, with zero
  changes to any element class.
- You need genuine double dispatch — behavior that depends on both the concrete
  element type *and* the concrete operation being performed, which a single virtual
  method on the element can't express cleanly once there are several operations.
- You're processing a fixed, well-known set of node/element types with several
  different traversal-style operations: compilers walking an AST (type-checking,
  code generation, pretty-printing), document object models (rendering, exporting,
  validating), or a geometry library computing area, perimeter, and bounding box
  across shape types.

## When to Avoid

- The element hierarchy changes frequently. Adding a new element type means adding
  a new `visit()` overload to the `Visitor` interface *and* to every existing
  concrete visitor — this is Visitor's biggest maintenance cost, so it's a poor fit
  when new element types show up often.
- There are only one or two operations, and they're unlikely to grow — a virtual
  method directly on each element (a simple polymorphic call) is simpler than the
  visitor indirection.
- Elements would need to expose significant internal state just so visitors can act
  on it — if that breaks encapsulation more than the pattern's benefits are worth,
  a method on the element itself may be the better trade-off.

## How to Implement

1. **Declare the Visitor interface with forward-declared element types.** One
   `visit()` overload per concrete element.

   ```cpp
   class Circle;
   class Square;

   class ShapeVisitor {
   public:
       virtual ~ShapeVisitor() = default;
       virtual void visit(const Circle& circle) = 0;
       virtual void visit(const Square& square) = 0;
   };
   ```

2. **Declare the Element interface.** A single `accept()` method.

   ```cpp
   class Shape {
   public:
       virtual ~Shape() = default;
       virtual void accept(ShapeVisitor& visitor) const = 0;
   };
   ```

3. **Implement Concrete Elements.** Each `accept()` calls back into the visitor
   with its own concrete type — this is the second half of double dispatch.

   ```cpp
   class Circle final : public Shape {
   public:
       explicit Circle(double radius) : radius_(radius) {}
       double radius() const { return radius_; }

       void accept(ShapeVisitor& visitor) const override {
           visitor.visit(*this);
       }

   private:
       double radius_;
   };
   ```

4. **Implement Concrete Visitors.** Each is one complete operation, spanning every
   element type.

   ```cpp
   class AreaVisitor final : public ShapeVisitor {
   public:
       void visit(const Circle& circle) override {
           std::cout << "circle area: " << 3.14159 * circle.radius() * circle.radius() << "\n";
       }
       void visit(const Square& square) override {
           std::cout << "square area: " << square.side() * square.side() << "\n";
       }
   };
   ```

5. **Apply operations by visiting each element.** Adding `DescribeVisitor` as a new
   operation requires no changes to `Shape`, `Circle`, or `Square`.

   ```cpp
   std::vector<std::unique_ptr<Shape>> shapes;
   shapes.push_back(std::make_unique<Circle>(3.0));
   shapes.push_back(std::make_unique<Square>(4.0));

   AreaVisitor areaVisitor;
   for (const auto& shape : shapes) {
       shape->accept(areaVisitor);
   }
   ```

## Implementation Tips

- Keep the visitor interface's `visit()` overloads at exactly one per concrete
  element type — resist adding generic or catch-all overloads that undermine the
  type-specific dispatch the pattern relies on.
- Elements should only ever need one method, `accept()` — resist the temptation to
  add operation-specific methods back onto elements once a visitor exists; that
  defeats the separation the pattern provides.
- Before committing to Visitor, weigh how often you expect to add new element types
  versus new operations. If element types are the more volatile axis, Visitor will
  fight you; if operations are more volatile, Visitor is a strong fit.
- Const-correctness matters here: if visitors only read element state, take
  elements by `const&` in both `accept()` and `visit()` as shown, to make the
  read-only contract explicit and enforceable.
- Consider a variant-based (`std::variant` + `std::visit`) approach instead of
  classic inheritance-based Visitor in modern C++ when the element set is closed
  and known at compile time — it avoids virtual dispatch overhead and gives
  compile-time exhaustiveness checking, at the cost of needing to modify the
  variant type when adding a new element.