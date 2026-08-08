// Core inheritance and polymorphism examples
// Compile with: g++ -std=c++11 -O2 cpp_core/inheritance_polymorphism_examples.cpp -o bin/core_polymorphism && ./bin/core_polymorphism

#include <iostream>
#include <memory>
#include <vector>

// Tips:
// - Give polymorphic base classes virtual destructors.
// - Use override so the compiler checks that a virtual function is actually overridden.
// - Prefer composition when inheritance does not represent a genuine substitutable relationship.
// - Use runtime polymorphism only when the concrete type must vary at runtime.

class Shape {
public:
    virtual ~Shape() {}
    virtual double area() const = 0;
};

class Rectangle : public Shape {
public:
    Rectangle(double width, double height) : width_(width), height_(height) {}

    double area() const override {
        return width_ * height_;
    }

private:
    double width_;
    double height_;
};

class Circle : public Shape {
public:
    explicit Circle(double radius) : radius_(radius) {}

    double area() const override {
        return 3.14159 * radius_ * radius_;
    }

private:
    double radius_;
};

int main() {
    std::cout << "Core inheritance and polymorphism examples:\n";

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.emplace_back(new Rectangle(4.0, 5.0));
    shapes.emplace_back(new Circle(2.0));

    for (const auto& shape : shapes) {
        std::cout << "area = " << shape->area() << "\n";
    }

    return 0;
}
