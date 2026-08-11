// Visitor pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/visitor/visitor_examples.cpp -o bin/visitor_example && ./bin/visitor_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Visitor lets you define a new operation over a set of related element classes
// without modifying those classes. Each element accepts a visitor and calls back
// into the visitor's method for its own concrete type (double dispatch), so the
// operation's logic lives entirely in the visitor, not scattered across elements.
//
// Use Visitor when:
// - you need to perform several unrelated operations over a stable set of element
//   classes, and don't want to pollute each element with every operation;
// - the operation logic changes or grows more often than the element hierarchy
//   itself, so it's cheaper to add a new visitor than to touch every element;
// - you need double dispatch — behavior that depends on both the concrete element
//   type and the concrete operation being performed.
//
// Avoid Visitor when:
// - the element hierarchy changes frequently — adding a new element type means
//   adding a new visit method to every existing visitor, which is expensive here;
// - there's only one or two operations, and they're unlikely to grow — a virtual
//   method directly on each element is simpler than the visitor indirection;
// - elements would need to expose significant internal state just so visitors can
//   act on it, breaking encapsulation more than the pattern is worth.
//
// Tips:
// - Keep the visitor interface's visit() overloads one per concrete element type.
// - Elements only need one method (accept()); all operation logic lives in visitors.
// - Adding a new operation is just adding a new Visitor subclass — no element changes.

class Circle;
class Square;

class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;
    virtual void visit(const Circle& circle) = 0;
    virtual void visit(const Square& square) = 0;
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(ShapeVisitor& visitor) const = 0;
};

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

class Square final : public Shape {
public:
    explicit Square(double side) : side_(side) {}
    double side() const { return side_; }

    void accept(ShapeVisitor& visitor) const override {
        visitor.visit(*this);
    }

private:
    double side_;
};

// Concrete visitor: one new operation (area calculation) over all shape types,
// with no changes needed to Circle or Square.
class AreaVisitor final : public ShapeVisitor {
public:
    void visit(const Circle& circle) override {
        double area = 3.14159 * circle.radius() * circle.radius();
        std::cout << "circle area: " << area << "\n";
    }

    void visit(const Square& square) override {
        double area = square.side() * square.side();
        std::cout << "square area: " << area << "\n";
    }
};

// A second, unrelated operation — added without touching Shape, Circle, or Square.
class DescribeVisitor final : public ShapeVisitor {
public:
    void visit(const Circle& circle) override {
        std::cout << "a circle with radius " << circle.radius() << "\n";
    }

    void visit(const Square& square) override {
        std::cout << "a square with side " << square.side() << "\n";
    }
};

int main() {
    std::cout << "Visitor pattern example:\n";

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(3.0));
    shapes.push_back(std::make_unique<Square>(4.0));

    AreaVisitor areaVisitor;
    for (const auto& shape : shapes) {
        shape->accept(areaVisitor);
    }

    DescribeVisitor describeVisitor;
    for (const auto& shape : shapes) {
        shape->accept(describeVisitor);
    }

    return 0;
}