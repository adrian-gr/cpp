// ADL (Argument-Dependent Lookup) example
// Compile with: g++ -std=c++20 -O2 idioms/adl_examples.cpp -o bin/adl_example && ./bin/adl_example

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// Description:
// Argument-Dependent Lookup (ADL, also called "Koenig lookup") is the rule that,
// when the compiler resolves an unqualified function call like foo(x), it looks
// not only in the calling scope's usual visible names, but ALSO in the
// namespace(s) associated with x's TYPE — even if that namespace was never
// `using`-imported or otherwise brought into scope. This is why
// `std::cout << myVector` works for types with an operator<< defined alongside
// them, and why plain `swap(a, b)` (unqualified) can find a type's own
// namespace-level swap even without `using std::swap;` or a fully-qualified call.
//
// ADL exists because operator overloads and closely-related free functions are
// conventionally defined in the SAME namespace as the type they operate on
// (rather than as members, which isn't always possible — e.g. for operators
// where the left operand isn't your type, like std::ostream& operator<<). ADL
// is what lets `myObject + other` or `os << myObject` work without every caller
// needing `using namespace mylib;` just to find the operator.
//
// The idiom this most concretely affects is CUSTOM SWAP. The canonical pattern
// for calling a possibly-customized swap is:
//
//     using std::swap;
//     swap(a, b);   // unqualified call
//
// NOT `std::swap(a, b);` (fully qualified — this ALWAYS calls the generic
// std::swap, even if a's type defines a more efficient specific one) and NOT
// bare `swap(a, b);` without the `using std::swap;` first (which would fail to
// compile for built-in types or types with no ADL-findable swap, since there'd
// be no std::swap visible in scope as a fallback). The `using std::swap;` line
// brings std::swap into consideration as a FALLBACK candidate, while the
// unqualified call `swap(a, b)` still lets ADL find and prefer a more specific
// swap defined in a's own namespace, if one exists. Overload resolution then
// picks the best match between the two candidates — which is the type's own
// swap when one exists, because it's a more specific match.
//
// Use (rely on) ADL when:
// - calling operators or closely-related free functions (swap, to_string-style
//   helpers, begin/end for range-based for) on a user-defined type — this is
//   largely automatic; you benefit from ADL just by calling things unqualified.
//
// Write functions FOR ADL to find when:
// - you're defining a customization point for your own type — put the function
//   in the SAME namespace as the type (not the global namespace, not a
//   "utils" namespace elsewhere), so ADL can find it from unqualified calls.
//
// Be careful with ADL when:
// - a fully-qualified call (std::swap(a, b)) silently bypasses ADL entirely,
//   which can accidentally use a generic, less efficient implementation instead
//   of a type's customized one — this is the classic swap-idiom pitfall.
// - ADL can occasionally pull in an unexpected overload from a type's
//   namespace that shadows what you intended to call — rare in practice, but
//   worth knowing about when overload resolution behaves surprisingly.
//
// Tips:
// - For calling a possibly-customized swap generically, always use the
//   two-line idiom: `using std::swap; swap(a, b);` — never call std::swap(a, b)
//   directly in generic code that might be handed a type with its own swap.
// - Define a type's own operator overloads and swap in the SAME namespace as
//   the type itself, so ADL can find them.
// - Range-based for loops rely on ADL too: `for (auto& x : container)` looks
//   for begin(container)/end(container) partly via ADL, which is how
//   third-party container types can support range-based for without any
//   change to the language or std::begin/std::end themselves.

namespace mylib {

// A type with an expensive-to-copy resource (like the Buffer example from the
// Rule of Five section), where a generic std::swap (which does three moves:
// temp = a; a = b; b = temp) would be much more expensive than swapping the
// internal pointers directly.
class LargeBuffer {
public:
    explicit LargeBuffer(std::string label, size_t size)
        : label_(std::move(label)), data_(new int[size]), size_(size) {}

    ~LargeBuffer() { delete[] data_; }

    LargeBuffer(const LargeBuffer&) = delete;
    LargeBuffer& operator=(const LargeBuffer&) = delete;

    LargeBuffer(LargeBuffer&& other) noexcept
        : label_(std::move(other.label_)), data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    LargeBuffer& operator=(LargeBuffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            label_ = std::move(other.label_);
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    const std::string& label() const { return label_; }
    size_t size() const { return size_; }

private:
    friend void swap(LargeBuffer& a, LargeBuffer& b) noexcept;

    std::string label_;
    int* data_;
    size_t size_;
};

// A free function swap, defined in the SAME namespace as LargeBuffer (mylib),
// not in the global namespace and not in std. ADL is what allows an
// unqualified `swap(x, y)` call to find THIS function when x and y are
// mylib::LargeBuffer, even from code that never wrote `using namespace mylib;`.
void swap(LargeBuffer& a, LargeBuffer& b) noexcept {
    std::cout << "  mylib::swap called (ADL-found, pointer swap, O(1))\n";
    using std::swap;   // enables swapping the individual members below,
                         // falling back to std::swap for std::string/size_t.
    swap(a.label_, b.label_);
    swap(a.data_, b.data_);
    swap(a.size_, b.size_);
}

} // namespace mylib

// Generic function, written the way library/algorithm code should be written:
// it doesn't know in advance whether T has a customized swap. The two-line
// idiom lets ADL find one if it exists, falling back to std::swap otherwise.
template <typename T>
void genericSwap(T& a, T& b) {
    using std::swap;   // brings std::swap into the candidate set as a fallback
    swap(a, b);         // UNQUALIFIED call: ADL searches T's own namespace
                         // first; overload resolution prefers a more specific
                         // match there over the std::swap fallback.
}

int main() {
    std::cout << "ADL example:\n";

    std::cout << "-- calling swap on a type with an ADL-findable swap --\n";
    mylib::LargeBuffer bufferA("A", 1000);
    mylib::LargeBuffer bufferB("B", 2000);
    std::cout << "before: " << bufferA.label() << "(" << bufferA.size()
              << "), " << bufferB.label() << "(" << bufferB.size() << ")\n";

    genericSwap(bufferA, bufferB);   // finds mylib::swap via ADL, NOT std::swap

    std::cout << "after:  " << bufferA.label() << "(" << bufferA.size()
              << "), " << bufferB.label() << "(" << bufferB.size() << ")\n";

    std::cout << "\n-- calling swap on a type with NO custom swap --\n";
    int x = 1, y = 2;
    genericSwap(x, y);   // mylib::swap isn't a candidate at all (int isn't in
                          // mylib); falls back cleanly to std::swap, found
                          // via the `using std::swap;` line, not ADL.
    std::cout << "x = " << x << ", y = " << y << "\n";

    std::cout << "\n-- the pitfall: calling std::swap directly bypasses ADL --\n";
    mylib::LargeBuffer bufferC("C", 500);
    mylib::LargeBuffer bufferD("D", 700);
    std::cout << "calling std::swap(bufferC, bufferD) directly:\n";
    std::swap(bufferC, bufferD);   // fully qualified: ADL never runs, always
                                     // uses the generic std::swap (three moves)
                                     // even though mylib::swap exists and is
                                     // strictly more efficient for this type.
    std::cout << "(no \"mylib::swap called\" message printed above — the\n"
              << " qualified call bypassed ADL and used std::swap instead)\n";

    return 0;
}