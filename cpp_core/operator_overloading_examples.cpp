// Core operator overloading examples
// Compile with: g++ -std=c++11 -O2 cpp_core/operator_overloading_examples.cpp -o bin/core_operator_overloading && ./bin/core_operator_overloading

#include <iostream>

// Tips:
// - Overload operators only when the meaning is natural for the type.
// - Preserve expected laws such as equality symmetry and arithmetic consistency.
// - Prefer non-member operators when both operands should be treated symmetrically.
// - Keep operators small and unsurprising; use named functions for complex behavior.

class Point {
public:
    Point(int x, int y) : x_(x), y_(y) {}
    Point operator+(const Point& other) const { return Point(x_ + other.x_, y_ + other.y_); }
    bool operator==(const Point& other) const { return x_ == other.x_ && y_ == other.y_; }
    int x() const { return x_; }
    int y() const { return y_; }
private:
    int x_;
    int y_;
};

std::ostream& operator<<(std::ostream& output, const Point& point) {
    return output << '(' << point.x() << ", " << point.y() << ')';
}

int main() {
    std::cout << "Core operator overloading examples:\n";
    Point total = Point(1, 2) + Point(3, 4);
    std::cout << "total = " << total << "\n";
    std::cout << "equals expected = " << std::boolalpha
              << (total == Point(4, 6)) << "\n";
    return 0;
}
