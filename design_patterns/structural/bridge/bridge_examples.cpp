// Bridge pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/bridge/bridge_examples.cpp -o bin/bridge_example && ./bin/bridge_example

#include <iostream>
#include <memory>
#include <string>

// Description:
// Bridge decouples an abstraction from its implementation so the two can vary
// independently. Instead of a single inheritance hierarchy that grows by the
// cross-product of every abstraction variant times every implementation variant,
// the abstraction holds a reference to a separate implementation hierarchy and
// delegates to it.
//
// Use Bridge when:
// - both an abstraction and its implementation need to vary independently, and
//   should each be extensible on their own without affecting the other;
// - a class hierarchy would otherwise explode combinatorially (N abstractions x
//   M implementations = N*M subclasses) if implemented with inheritance alone;
// - you want to switch an implementation at runtime without changing the
//   abstraction the client code depends on.
//
// Avoid Bridge when:
// - there's only one implementation and no plausible need for more — the extra
//   indirection buys nothing over a single concrete class;
// - the abstraction and implementation are tightly coupled by nature and were
//   never going to vary independently — splitting them adds ceremony for no gain;
// - a simple inheritance hierarchy already models the variation adequately because
//   only one axis of variation (not two) actually exists.
//
// Tips:
// - Keep the implementation interface narrow and low-level; let the abstraction's
//   higher-level operations be built out of calls to it.
// - Pass or inject the concrete implementation into the abstraction (composition),
//   rather than baking a specific one in via inheritance.
// - Add new abstractions and new implementations independently, each without
//   touching the other hierarchy.

// Implementation hierarchy: how a shape is actually rendered.
class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void renderCircle(double radius) const = 0;
    virtual void renderSquare(double side) const = 0;
};

class VectorRenderer final : public Renderer {
public:
    void renderCircle(double radius) const override {
        std::cout << "vector: drawing circle, radius " << radius << "\n";
    }
    void renderSquare(double side) const override {
        std::cout << "vector: drawing square, side " << side << "\n";
    }
};

class RasterRenderer final : public Renderer {
public:
    void renderCircle(double radius) const override {
        std::cout << "raster: rasterizing circle, radius " << radius << "\n";
    }
    void renderSquare(double side) const override {
        std::cout << "raster: rasterizing square, side " << side << "\n";
    }
};

// Abstraction hierarchy: what shape is being drawn. Holds a Renderer and
// delegates the actual drawing work to it.
class Shape {
public:
    explicit Shape(std::shared_ptr<Renderer> renderer) : renderer_(std::move(renderer)) {}
    virtual ~Shape() = default;
    virtual void draw() const = 0;

protected:
    std::shared_ptr<Renderer> renderer_;
};

class Circle final : public Shape {
public:
    Circle(std::shared_ptr<Renderer> renderer, double radius)
        : Shape(std::move(renderer)), radius_(radius) {}

    void draw() const override {
        renderer_->renderCircle(radius_);
    }

private:
    double radius_;
};

class Square final : public Shape {
public:
    Square(std::shared_ptr<Renderer> renderer, double side)
        : Shape(std::move(renderer)), side_(side) {}

    void draw() const override {
        renderer_->renderSquare(side_);
    }

private:
    double side_;
};

int main() {
    std::cout << "Bridge pattern example:\n";

    auto vectorRenderer = std::make_shared<VectorRenderer>();
    auto rasterRenderer = std::make_shared<RasterRenderer>();

    Circle vectorCircle(vectorRenderer, 5.0);
    Square rasterSquare(rasterRenderer, 3.0);
    Circle rasterCircle(rasterRenderer, 5.0);

    vectorCircle.draw();
    rasterSquare.draw();
    rasterCircle.draw();

    return 0;
}