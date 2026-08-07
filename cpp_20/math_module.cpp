// C++20 module interface example
// Build with GCC 13:
//   g++ -std=c++20 -fmodules-ts -c cpp_20/math_module.cpp -o bin/math_module.o
//   g++ -std=c++20 -fmodules-ts cpp_20/modules_examples.cpp bin/math_module.o -o bin/cpp20_modules
//   ./bin/cpp20_modules

module;

// The global module fragment is useful for legacy headers that should not be exported.
#include <concepts>

export module math_module;

// Tips for C++20 modules:
// - Export only the declarations that form the module's public interface.
// - Keep implementation details unexported to reduce coupling between consumers.
// - Modules are compiled separately, so build systems must track module dependencies.
// - Do not mix textual inclusion and imports casually; use headers for legacy interfaces.
// - Compiler and build-system support is still important when adopting modules.

export constexpr int square(int value) {
    return value * value;
}

export template <std::integral T>
constexpr T add(T left, T right) {
    return left + right;
}

namespace {
int implementationDetail() {
    return 42;
}
}
