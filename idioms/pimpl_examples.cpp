// Pimpl idiom (Pointer to Implementation) example
//
// This idiom's entire benefit is about HEADER separation, so it's shown as three
// files rather than one. Compile with:
//   g++ -std=c++20 -O2 idioms/pimpl_examples.cpp -o bin/pimpl_example && ./bin/pimpl_example
// (all three sections below are concatenated into one file here for convenience;
//  in a real project they'd be widget.h, widget.cpp, and main.cpp)

// ===========================================================================
// widget.h — the PUBLIC header. Everyone who #includes this only sees a
// forward-declared Impl type and a pointer to it. None of Widget's actual
// data members, and none of the headers ITS implementation needs, are visible
// here at all.
// ===========================================================================
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Pimpl ("pointer to implementation") moves a class's private data members and
// implementation details out of its header and into a separate, forward-declared
// type defined only in the .cpp file. The header exposes just a pointer to that
// incomplete type (usually std::unique_ptr<Impl>) plus the class's public
// interface. Callers who #include the header never see the implementation's
// private members or the headers those members depend on.
//
// This solves two real costs of ordinary C++ classes:
// - Compile-time coupling: if a class's private members use, say, <vector>,
//   <map>, and a third-party library's types, every file that #includes the
//   class's header transitively depends on those headers too — and recompiles
//   whenever any of them change, even if nothing about the class's PUBLIC
//   interface changed.
// - ABI/binary stability: changing a private member's type or adding one
//   normally changes the class's size/layout, forcing every dependent
//   translation unit to recompile. With Pimpl, the header's visible size is
//   just "one pointer," so private implementation changes don't ripple outward.
//
// Use Pimpl when:
// - a class's header is included by many other files, and its private members
//   pull in heavy or frequently-changing dependencies you don't want to expose;
// - you're maintaining binary/ABI compatibility across a library boundary, and
//   want to change private implementation details without breaking every
//   consumer's need to recompile (or, for a shared library, relink);
// - you want to genuinely hide implementation details — not just by convention
//   (private:) but so they're not even visible in the header text at all.
//
// Avoid Pimpl when:
// - the class is small, internal to one module, and not a widely-included
//   header — the extra indirection (heap allocation, one pointer dereference
//   per access) and boilerplate (forwarding every method) isn't worth it;
// - build times and ABI stability genuinely aren't a concern for this class —
//   Pimpl is a cost paid for a specific benefit, not a default style choice;
// - performance-critical code where the extra pointer indirection and
//   heap-allocated Impl object matter — Pimpl adds a small but real runtime cost.
//
// Tips:
// - Use std::unique_ptr<Impl> for the pointer; it needs the class's destructor,
//   move constructor, and move assignment to be defined in the .cpp file (where
//   Impl is a complete type) rather than defaulted in the header — defaulting
//   them in the header would try to instantiate std::unique_ptr's destructor
//   for an incomplete type, which fails to compile.
// - The public class's interface should stay in the header as normal; only the
//   PRIVATE DATA and any purely-internal helper logic move into Impl.
// - Every public method typically just forwards to the same-named method on
//   impl_ — this thin forwarding layer is the cost of the pattern.

class Widget {
public:
    explicit Widget(std::string name);

    // Special members are DECLARED here but defined in the .cpp file, where
    // Impl is a complete type. This is required, not optional, for
    // unique_ptr<Impl> to work with an incomplete Impl in the header.
    ~Widget();
    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;
    Widget(const Widget& other);
    Widget& operator=(const Widget& other);

    void addTag(const std::string& tag);
    void render() const;

private:
    class Impl;                    // forward declaration only — an incomplete
                                    // type as far as this header is concerned.
    std::unique_ptr<Impl> impl_;   // the ENTIRE private state of Widget, as
                                    // far as this header reveals, is "a pointer."
};

// ===========================================================================
// widget.cpp — the IMPLEMENTATION file. Impl is defined here, completely
// hidden from anyone who only sees widget.h. This file can #include whatever
// heavy dependencies it needs (here just <vector>/<string> for illustration,
// but imagine a third-party image library, a heavy template library, etc.)
// without any of that leaking into widget.h or its includers.
// ===========================================================================

class Widget::Impl {
public:
    explicit Impl(std::string name) : name_(std::move(name)) {}

    std::string name_;             // private data, invisible outside this file
    std::vector<std::string> tags_;   // ditto — <vector> never needs to be
                                        // included by widget.h at all

    void render() const {
        std::cout << name_ << " [";
        for (size_t i = 0; i < tags_.size(); ++i) {
            std::cout << tags_[i] << (i + 1 < tags_.size() ? ", " : "");
        }
        std::cout << "]\n";
    }
};

Widget::Widget(std::string name) : impl_(std::make_unique<Impl>(std::move(name))) {}

// Defined here, not defaulted in the header: at this point Impl is a complete
// type, so unique_ptr<Impl>'s destructor can actually be instantiated.
Widget::~Widget() = default;
Widget::Widget(Widget&& other) noexcept = default;
Widget& Widget::operator=(Widget&& other) noexcept = default;

// Copy operations need to be written explicitly: unique_ptr itself isn't
// copyable, so copying a Widget means deep-copying its Impl.
Widget::Widget(const Widget& other) : impl_(std::make_unique<Impl>(*other.impl_)) {}
Widget& Widget::operator=(const Widget& other) {
    if (this != &other) {
        impl_ = std::make_unique<Impl>(*other.impl_);
    }
    return *this;
}

void Widget::addTag(const std::string& tag) {
    impl_->tags_.push_back(tag);
}

void Widget::render() const {
    impl_->render();
}

// ===========================================================================
// main.cpp — a CONSUMER of widget.h. This file only ever sees the interface
// above: it has no idea Widget internally uses a std::vector, and recompiling
// it doesn't depend on anything inside widget.cpp changing.
// ===========================================================================
int main() {
    std::cout << "Pimpl idiom example:\n";

    Widget w("Button");
    w.addTag("primary");
    w.addTag("large");
    w.render();

    Widget w2 = w;   // uses the explicit copy constructor: deep-copies Impl
    w2.addTag("copy");
    w2.render();
    w.render();       // unaffected by the tag added to w2 — genuinely independent

    return 0;
}