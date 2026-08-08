// Bridge pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/bridge_examples.cpp -o bin/bridge_example && ./bin/bridge_example

#include <iostream>
#include <memory>
#include <utility>

// Description:
// Bridge separates an abstraction from its implementation so both can vary independently.
// The abstraction delegates platform or backend work to an implementation interface.
//
// Use Bridge when:
// - an abstraction has multiple independent dimensions of variation;
// - platform-specific implementations should stay isolated from business-facing code;
// - inheritance would otherwise create a class for every abstraction/implementation combination.
//
// Avoid Bridge when:
// - there is only one stable implementation;
// - the extra abstraction and indirection add more complexity than flexibility;
// - a simple composition or direct function call expresses the relationship clearly.
//
// Tips:
// - Keep the abstraction focused on the client-facing operations.
// - Keep implementation classes focused on backend-specific behavior.
// - Prefer composition through a reference or smart pointer to the implementation.
// - Add a new abstraction or implementation independently without changing the other hierarchy.

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual void drawCircle(int radius) = 0;
};

class VectorRenderer final : public Renderer {
public:
    void drawCircle(int radius) override {
        std::cout << "vector circle with radius " << radius << "\n";
    }
};

class RasterRenderer final : public Renderer {
public:
    void drawCircle(int radius) override {
        std::cout << "raster circle with radius " << radius << "\n";
    }
};

class Shape {
public:
    explicit Shape(std::unique_ptr<Renderer> renderer)
        : renderer_(std::move(renderer)) {}

    virtual ~Shape() = default;
    virtual void draw() const = 0;

protected:
    Renderer& renderer() const {
        return *renderer_;
    }

private:
    std::unique_ptr<Renderer> renderer_;
};

class Circle final : public Shape {
public:
    Circle(int radius, std::unique_ptr<Renderer> renderer)
        : Shape(std::move(renderer)), radius_(radius) {}

    void draw() const override {
        renderer().drawCircle(radius_);
    }

private:
    int radius_;
};

int main() {
    std::cout << "Bridge pattern example:\n";

    Circle vectorCircle(5, std::make_unique<VectorRenderer>());
    Circle rasterCircle(5, std::make_unique<RasterRenderer>());
    vectorCircle.draw();
    rasterCircle.draw();

    return 0;
}
