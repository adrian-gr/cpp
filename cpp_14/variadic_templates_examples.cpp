// C++14 variadic template examples
// Compile with: g++ -std=c++14 -O2 cpp_14/variadic_templates_examples.cpp -o bin/cpp14_variadic && ./bin/cpp14_variadic

#include <iostream>
#include <utility>

// Tips for C++14 variadic template usage:
// - Forwarding references preserve whether each argument is an lvalue or rvalue.
// - Use std::forward when passing a parameter pack to another function.
// - decltype(auto) can preserve the exact return type of a variadic helper.
// - Keep ownership and lifetime clear when forwarding references through several calls.

void show(int& value) {
    std::cout << "lvalue: " << value << "\n";
}

void show(int&& value) {
    std::cout << "rvalue: " << value << "\n";
}

template <typename... Types>
void showAll(Types&&... values) {
    int unused[] = {0, (show(std::forward<Types>(values)), 0)...};
    (void)unused;
}

template <typename... Types>
std::size_t countArguments(Types&&...) {
    return sizeof...(Types);
}

int main() {
    std::cout << "C++14 variadic template examples:\n";
    int value = 10;
    showAll(value, 20);
    std::cout << "argument count = " << countArguments(value, 20, 3.5) << "\n";
    return 0;
}
