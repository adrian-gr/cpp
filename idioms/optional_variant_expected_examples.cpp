// std::optional, std::variant, std::expected (C++23) — alternatives/complements
// to exceptions for representing absence or error states
//
// std::expected requires C++23 and a sufficiently recent standard library
// (libstdc++ 13+, libc++ 16+, MSVC 19.35+). Compile with:
//   g++ -std=c++23 -O2 idioms/optional_variant_expected_examples.cpp -o bin/optional_variant_expected_example && ./bin/optional_variant_expected_example
// If your toolchain doesn't yet have <expected>, everything except section 3
// below still compiles fine under -std=c++20.

#include <expected>
#include <iostream>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// Description:
// Exceptions are one way to signal "this operation didn't produce a normal
// result" — but they're not the only way, and not always the best one. Three
// standard library types offer alternatives, each suited to a different shape
// of "not a normal value":
//
// - std::optional<T>: "there might not BE a value at all" — no error
//   information, just presence or absence. The right tool when absence is a
//   normal, expected outcome, not a failure (e.g. looking up a key that's
//   allowed not to exist).
// - std::variant<T1, T2, ...>: "the value IS one of these specific alternative
//   types" — a type-safe union. Useful for representing a closed set of
//   possible shapes a value can take, checked and enforced at compile time,
//   without a base class, virtual dispatch, or a raw union's lack of type
//   safety.
// - std::expected<T, E> (C++23): "either a successful value of type T, or an
//   error of type E explaining what went wrong" — this is the direct,
//   exception-free way to signal a genuine failure, when you want the caller
//   to see (and be statically encouraged to check) both possibilities.
//
// The common thread: all three make possible-non-value outcomes part of the
// TYPE SYSTEM, checked by the compiler, rather than an implicit possibility a
// caller has to remember (a nullable pointer, an out-of-band sentinel value
// like -1, or an exception a caller might forget to catch). Exceptions are
// still the right tool for genuinely exceptional, rare failures where
// unwinding the stack automatically is valuable; these types are usually
// preferred for outcomes that are a normal, anticipated part of a function's
// contract — "not found" is not exceptional, it's just one of two expected
// answers.
//
// Use std::optional when:
// - a function may legitimately have no result to return, and that's not an
//   error — a search that may find nothing, a parse that may have nothing to
//   parse, an optional configuration field.
//
// Use std::variant when:
// - a value is genuinely one of a small, closed set of alternative types known
//   at compile time, and you want type-safe access enforced by the compiler
//   (std::get, std::visit) rather than a raw union's unchecked reinterpretation.
//
// Use std::expected when:
// - an operation can genuinely fail, and you want the failure reason available
//   to the caller as a normal return value rather than through an exception —
//   particularly in code paths where exceptions are undesirable (performance-
//   sensitive code, embedded contexts, or simply a preference for explicit,
//   visible error handling at each call site) or where you want to force
//   callers to at least consider the failure case via the type system.
//
// Avoid / be careful when:
// - reaching for optional/variant/expected everywhere reflexively — genuinely
//   exceptional, rare, or unrecoverable failures (out of memory, a violated
//   precondition/programmer error) are often still better served by exceptions
//   or by aborting, not by threading an error type through every call site.
// - accessing a std::optional/variant/expected without checking it first —
//   .value() on an empty optional, std::get on the wrong variant alternative,
//   and .value() on an unexpected expected ALL throw — these types don't
//   eliminate the need to check, they just make the check visible and
//   enforceable at the type level instead of implicit.

// ---------------------------------------------------------------------------
// 1. std::optional<T>: absence is a normal outcome, not a failure.
// ---------------------------------------------------------------------------
std::optional<int> findUserAge(const std::string& name) {
    static const std::vector<std::pair<std::string, int>> users = {
        {"alice", 30}, {"bob", 25}
    };
    for (const auto& [userName, age] : users) {
        if (userName == name) return age;
    }
    return std::nullopt;   // "not found" — not an error, just absence.
}

void demonstrateOptional() {
    std::cout << "-- std::optional --\n";

    if (auto age = findUserAge("alice")) {
        std::cout << "alice's age: " << *age << "\n";
    }

    // value_or provides a fallback without needing an if/else at every call site.
    int bobAge = findUserAge("bob").value_or(-1);
    int carolAge = findUserAge("carol").value_or(-1);
    std::cout << "bob: " << bobAge << ", carol (not found): " << carolAge << "\n";

    // .value() throws std::bad_optional_access if empty — checking first (as
    // above) is the normal usage pattern, not an afterthought.
    try {
        std::cout << findUserAge("dave").value() << "\n";
    } catch (const std::bad_optional_access& e) {
        std::cout << "caught: " << e.what() << "\n";
    }
}

// ---------------------------------------------------------------------------
// 2. std::variant<T1, T2, ...>: a type-safe union over a closed set of types.
// ---------------------------------------------------------------------------
using JsonValue = std::variant<std::monostate, bool, double, std::string>;
// monostate represents "null" — variant needs a default-constructible first
// alternative if you want a variant to be default-constructed as "empty".

std::string describeJsonValue(const JsonValue& value) {
    // std::visit dispatches to the correct overload based on the alternative
    // ACTUALLY held, checked and resolved at compile time — no manual
    // "which type is this" branching, and the compiler enforces that every
    // alternative is handled (no missing-case bugs, unlike a raw union +
    // manual tag check).
    return std::visit([](const auto& held) -> std::string {
        using T = std::decay_t<decltype(held)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            return "null";
        } else if constexpr (std::is_same_v<T, bool>) {
            return held ? "true" : "false";
        } else if constexpr (std::is_same_v<T, double>) {
            return "number: " + std::to_string(held);
        } else {   // std::string
            return "string: \"" + held + "\"";
        }
    }, value);
}

void demonstrateVariant() {
    std::cout << "\n-- std::variant --\n";

    std::vector<JsonValue> values = {std::monostate{}, true, 3.14, std::string("hello")};
    for (const auto& value : values) {
        std::cout << describeJsonValue(value) << "\n";
    }

    // std::get<T> throws std::bad_variant_access if the variant doesn't
    // currently hold that alternative — holds_alternative checks first.
    JsonValue v = 3.14;
    if (std::holds_alternative<double>(v)) {
        std::cout << "v holds a double: " << std::get<double>(v) << "\n";
    }
}

// ---------------------------------------------------------------------------
// 3. std::expected<T, E> (C++23): success value OR an explicit error, as a
// normal return type instead of an exception.
// ---------------------------------------------------------------------------
enum class ParseError { EmptyInput, NotANumber, OutOfRange };

std::string describeError(ParseError error) {
    switch (error) {
        case ParseError::EmptyInput: return "input was empty";
        case ParseError::NotANumber: return "input was not a valid number";
        case ParseError::OutOfRange: return "number out of representable range";
    }
    return "unknown error";
}

std::expected<int, ParseError> parsePositiveInt(const std::string& text) {
    if (text.empty()) {
        return std::unexpected(ParseError::EmptyInput);
    }
    try {
        size_t consumed = 0;
        int value = std::stoi(text, &consumed);
        if (consumed != text.size()) {
            return std::unexpected(ParseError::NotANumber);
        }
        return value;   // implicitly wraps into std::expected<int, ParseError>
    } catch (const std::out_of_range&) {
        return std::unexpected(ParseError::OutOfRange);
    } catch (const std::invalid_argument&) {
        return std::unexpected(ParseError::NotANumber);
    }
}

void demonstrateExpected() {
    std::cout << "\n-- std::expected (C++23) --\n";

    for (const std::string& input : {"42", "", "not-a-number", "99999999999999999999"}) {
        std::expected<int, ParseError> result = parsePositiveInt(input);

        // has_value()/operator bool checks, like optional; .error() is only
        // valid to call when !result.has_value(), the same way .value() on
        // optional is only valid when has_value() is true.
        if (result.has_value()) {
            std::cout << "\"" << input << "\" -> parsed: " << *result << "\n";
        } else {
            std::cout << "\"" << input << "\" -> error: " << describeError(result.error()) << "\n";
        }
    }

    // transform/and_then let you chain operations on the success path without
    // manually unwrapping at each step — conceptually similar to optional's
    // monadic operations (also added in C++23).
    auto doubled = parsePositiveInt("21").transform([](int n) { return n * 2; });
    std::cout << "21 parsed and doubled: "
              << (doubled ? std::to_string(*doubled) : "error") << "\n";
}

int main() {
    std::cout << "std::optional / std::variant / std::expected example:\n";

    demonstrateOptional();
    demonstrateVariant();
    demonstrateExpected();

    return 0;
}