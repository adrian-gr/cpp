# Bridge

## Description

Bridge is a structural design pattern that decouples an abstraction from its
implementation so the two can vary independently. Instead of one class hierarchy
encoding both "what" (the abstraction) and "how" (the implementation) through
inheritance, the abstraction holds a reference to a separate implementation object
and delegates the actual work to it.

This avoids the combinatorial explosion that comes from trying to express two
independent dimensions of variation — say, shape type and rendering technique — as
a single inheritance hierarchy, where you'd otherwise end up needing a subclass for
every *combination* (`VectorCircle`, `RasterCircle`, `VectorSquare`,
`RasterSquare`, ...).

Typical participants:

- **Abstraction** — defines the high-level interface clients use, and holds a
  reference to an Implementor (e.g. `Shape`).
- **RefinedAbstraction** — extends the abstraction with additional behavior,
  without touching the implementation hierarchy at all (e.g. `ResizableCircle`).
- **Implementor** — declares the interface for the implementation-side operations
  the abstraction needs (e.g. `Renderer`).
- **ConcreteImplementor** — implements the Implementor interface with one specific
  technique (e.g. `VectorRenderer`, `RasterRenderer`).

## When to Use

- Both an abstraction and its implementation need to vary independently, and
  subclassing both together would produce a combinatorial explosion of classes —
  one class per (abstraction, implementation) pair.
- You want to switch implementations at runtime — swap which `Renderer` a `Shape`
  uses — without touching any abstraction code.
- Changes to the implementation shouldn't require recompiling or changing client
  code that depends only on the abstraction; the two sides should be free to evolve
  on their own schedules.
- You're bridging a high-level concept to multiple low-level platforms or
  back-ends: GUI toolkits across operating systems, persistence logic across
  different database drivers, shapes across different rendering engines.

## When to Avoid

- There's only one implementation and no plausible need for more — the extra
  indirection (an abstraction holding a reference to a separate implementation
  interface) adds complexity without a payoff.
- The abstraction and implementation are tightly, permanently coupled by nature,
  and splitting them wouldn't reflect any real independent variation — forcing a
  Bridge split here just adds ceremony.
- A simpler single hierarchy is enough to express the variation you actually need,
  or the "how" variation is narrow enough that Strategy (varying one algorithm)
  fits better than a full second hierarchy.

## How to Implement

1. **Define the Implementor interface.** The primitive operations the abstraction
   needs — kept minimal, not a mirror of the abstraction's own public methods.

   ```cpp
   class Renderer {
   public:
       virtual ~Renderer() = default;
       virtual void renderCircle(double x, double y, double radius) = 0;
   };
   ```

2. **Implement Concrete Implementors.** Each is one way of doing "how."

   ```cpp
   class VectorRenderer final : public Renderer {
   public:
       void renderCircle(double x, double y, double radius) override {
           std::cout << "vector: drawing circle at (" << x << "," << y << ") r=" << radius << "\n";
       }
   };
   ```

3. **Define the Abstraction, holding an Implementor by reference (composition).**

   ```cpp
   class Shape {
   public:
       explicit Shape(Renderer& renderer) : renderer_(renderer) {}
       virtual ~Shape() = default;
       virtual void draw() = 0;

   protected:
       Renderer& renderer_;
   };
   ```

4. **Implement Concrete Abstractions.** Each delegates its drawing work to whatever
   `Renderer` it was given.

   ```cpp
   class Circle final : public Shape {
   public:
       Circle(Renderer& renderer, double x, double y, double radius)
           : Shape(renderer), x_(x), y_(y), radius_(radius) {}

       void draw() override { renderer_.renderCircle(x_, y_, radius_); }

   private:
       double x_, y_, radius_;
   };
   ```

5. **(Optional) Add a Refined Abstraction.** Extends the abstraction without any
   changes to the implementation hierarchy.

   ```cpp
   class ResizableCircle final : public Shape {
   public:
       ResizableCircle(Renderer& renderer, double x, double y, double radius)
           : Shape(renderer), x_(x), y_(y), radius_(radius) {}

       void resize(double factor) { radius_ *= factor; }
       void draw() override { renderer_.renderCircle(x_, y_, radius_); }

   private:
       double x_, y_, radius_;
   };
   ```

6. **Mix and match at the call site.** Any shape works with any renderer.

   ```cpp
   VectorRenderer vector;
   RasterRenderer raster;

   Circle circle1(vector, 0, 0, 5.0);
   circle1.draw();

   Circle circle2(raster, 10, 10, 3.0);
   circle2.draw();
   ```

## Implementation Tips

- Keep the implementation interface minimal — just the primitive operations the
  abstraction actually needs, not a full mirror of the abstraction's own public
  API. A bloated Implementor interface undermines independent variation on both
  sides.
- Let the abstraction hold the implementation via a pointer or reference
  (composition), never by inheriting from it — inheritance would re-couple the two
  hierarchies the pattern is meant to keep apart.
- Higher-level abstractions (refined abstractions) can be layered on top of the
  base abstraction without touching the implementation hierarchy at all, and new
  implementations can be added without touching the abstraction hierarchy — that
  independence is the whole point.
- Bridge is often set up early, at design time, specifically because two axes of
  variation are anticipated — unlike Adapter, which is typically reached for after
  the fact to reconcile one pre-existing incompatible interface.
- If the implementation side only ever has one active implementation per
  abstraction instance and never needs runtime swapping, double-check whether
  you actually need Bridge, or whether a simpler direct dependency would do.