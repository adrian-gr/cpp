// SFINAE / if constexpr / Concepts (C++20) example
// Compile with: g++ -std=c++20 -O2 idioms/sfinae_constexpr_concepts_examples.cpp -o bin/sfinae_constexpr_concepts_example && ./bin/sfinae_constexpr_concepts_example

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// Description:
// All three mechanisms below answer the same question — "how do I write a
// template that behaves differently, or is only available at all, depending on
// properties of its type parameter?" — at three different points in C++'s
// history, with sharply different readability.
//
// SFINAE ("Substitution Failure Is Not An Error"): if substituting a template
// parameter into a declaration would produce an invalid type/expression, the
// compiler silently removes that overload from consideration instead of erroring
// — it just looks for another viable overload. Pre-C++20 code (ab)uses this via
// std::enable_if to conditionally enable or disable whole function overloads
// based on a compile-time trait. It works, but the constraint is expressed
// indirectly, buried in the return type or a defaulted template parameter, and
// error messages when nothing matches are notoriously unreadable.
//
// if constexpr (C++17): lets you branch on a compile-time condition INSIDE a
// single function body, discarding the untaken branch entirely during
// compilation (it isn't even required to compile for types where it wouldn't).
// This replaces a common SFINAE use case — dispatching to different logic for
// different types — with an ordinary-looking if statement, dramatically
// improving readability for that specific case. It does not, by itself, remove
// or add an overload from a candidate set; it selects a branch inside one.
//
// Concepts (C++20): give constraints a name and a real place in the language.
// A concept is a compile-time predicate over a type, and `requires` lets you
// attach one directly to a template parameter. Concepts subsume most SFINAE
// uses: instead of hiding a constraint inside enable_if's return-type trick,
// you write it where a human reads it — right next to the template parameter —
// and the compiler produces a direct, readable error naming the concept that
// wasn't satisfied, rather than a wall of substitution-failure noise.
//
// Use SFINAE (std::enable_if) when:
// - you're targeting a codebase or standard version before C++20 and concepts
//   aren't available.
//
// Use if constexpr when:
// - a single function needs genuinely different logic for different types, and
//   the difference is a branch inside the function rather than "should this
//   overload be considered at all."
//
// Use Concepts when:
// - you're on C++20 or later — concepts are strictly more readable than SFINAE
//   for constraining template parameters, with clearer errors and clearer intent.
//
// Tips:
// - Prefer concepts over std::enable_if in any new C++20 code; there is very
//   little reason to reach for raw SFINAE today outside legacy-standard constraints.
// - if constexpr and concepts are complementary, not competing: use a concept to
//   constrain which types a function accepts, and if constexpr inside it to branch
//   between implementations for different (still-constrained) types.
// - A concept can be a simple type trait check, or a `requires` expression
//   listing actual operations the type must support — prefer the latter when you
//   care about "can I call these operations" more than "is this exact type."

// ---------------------------------------------------------------------------
// 1. SFINAE via std::enable_if (pre-C++20 style)
// Two overloads of `describe`, each enabled only for one category of type.
// The constraint lives in the return type, not next to the parameter — you
// have to read the whole enable_if expression to know what's required.
// ---------------------------------------------------------------------------
template <typename T>
typename std::enable_if<std::is_integral<T>::value, std::string>::type
describeSfinae(T value) {
    return "integral: " + std::to_string(value);
}

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, std::string>::type
describeSfinae(T value) {
    return "floating-point: " + std::to_string(value);
}

// ---------------------------------------------------------------------------
// 2. if constexpr (C++17): one function, branching internally.
// No overload resolution trickery — just a normal-looking if, except the
// untaken branch is discarded at compile time and never has to compile for
// types where it wouldn't make sense.
// ---------------------------------------------------------------------------
template <typename T>
std::string describeIfConstexpr(T value) {
    if constexpr (std::is_integral_v<T>) {
        return "integral: " + std::to_string(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        return "floating-point: " + std::to_string(value);
    } else {
        return "some other type";
    }
}

// ---------------------------------------------------------------------------
// 3. Concepts (C++20): the constraint is declared where a human reads it —
// directly on the template parameter — and gives a direct compiler error
// naming exactly which concept wasn't satisfied.
// ---------------------------------------------------------------------------
template <typename T>
concept Numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <Numeric T>
std::string describeConcept(T value) {
    if constexpr (std::is_integral_v<T>) {
        return "integral: " + std::to_string(value);
    } else {
        return "floating-point: " + std::to_string(value);
    }
}

// A `requires` expression concept: constrains by capability (has this
// operation) rather than by category (is this kind of type).
template <typename T>
concept Printable = requires(T value) {
    { std::cout << value } -> std::same_as<std::ostream&>;
};

template <Printable T>
void printValue(const T& value) {
    std::cout << "printable value: " << value << "\n";
}

int main() {
    std::cout << "SFINAE / if constexpr / Concepts example:\n";

    std::cout << "-- SFINAE (std::enable_if) --\n";
    std::cout << describeSfinae(42) << "\n";
    std::cout << describeSfinae(3.14) << "\n";
    // describeSfinae(std::string("x"));  // would NOT compile: no enabled overload
    //   matches std::string, and the error message would be a wall of
    //   substitution-failure noise rather than a clear statement of the problem.

    std::cout << "\n-- if constexpr --\n";
    std::cout << describeIfConstexpr(42) << "\n";
    std::cout << describeIfConstexpr(3.14) << "\n";
    std::cout << describeIfConstexpr(std::string("x")) << "\n";
    // Compiles fine for std::string too, since the "some other type" branch
    // is the one selected, and the other branches are discarded before ever
    // needing to compile for std::string.

    std::cout << "\n-- Concepts --\n";
    std::cout << describeConcept(42) << "\n";
    std::cout << describeConcept(3.14) << "\n";
    // std::cout << describeConcept(std::string("x"));  // would NOT compile,
    //   but with a direct, readable error: "constraints not satisfied... the
    //   required concept 'Numeric<std::string>' evaluated to false" — naming
    //   the exact concept, unlike SFINAE's substitution-failure noise.

    printValue(42);
    printValue(std::string("hello"));
    // printValue(std::vector<int>{1, 2, 3});  // would NOT compile: no
    //   operator<< for std::vector<int>, so Printable<std::vector<int>> is
    //   false, and the compiler says so directly.

    return 0;
}