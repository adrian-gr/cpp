#include <iostream>

// Tips for C++11 constexpr usage:
// - Use constexpr for pure functions and values that can be computed at compile time.
// - C++11 constexpr function bodies must be a single return statement.
// - Recursive constexpr functions are useful for simple compile-time calculations.
// - Use static_assert to verify compile-time results and document expectations.
// - Avoid forcing complex, stateful, or I/O-bound logic into constexpr functions.

constexpr int square(int value) {
    return value * value;
}

constexpr int factorial(int value) {
    return value <= 1 ? 1 : value * factorial(value - 1);
}

struct Point {
    int x;
    int y;

    constexpr Point(int xValue, int yValue)
        : x(xValue), y(yValue) {}

    constexpr int squaredDistanceFromOrigin() const {
        return x * x + y * y;
    }
};

int main() {
    std::cout << "C++11 constexpr examples:\n";

    // 1. Compile-time constant and constexpr function.
    constexpr int five = 5;
    constexpr int squared = square(five);
    static_assert(squared == 25, "square should be evaluated at compile time");
    std::cout << "square(5) = " << squared << "\n";

    // 2. Recursive constexpr calculation.
    constexpr int factorialResult = factorial(5);
    static_assert(factorialResult == 120,
                  "factorial should be evaluated at compile time");
    std::cout << "factorial(5) = " << factorialResult << "\n";

    // 3. constexpr constructor and member function.
    constexpr Point point(3, 4);
    static_assert(point.squaredDistanceFromOrigin() == 25,
                  "Point calculation should be evaluated at compile time");
    std::cout << "point squared distance = "
              << point.squaredDistanceFromOrigin() << "\n";

    return 0;
}
