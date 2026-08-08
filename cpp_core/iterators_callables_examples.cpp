// Core iterators and callable objects examples
// Compile with: g++ -std=c++11 -O2 cpp_core/iterators_callables_examples.cpp -o bin/core_iterators_callables && ./bin/core_iterators_callables

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>

// Tips:
// - Treat iterators as ranges into a container and never dereference end().
// - Use const iterators when an algorithm should not mutate elements.
// - Prefer standard predicates and lambdas for local algorithm behavior.
// - Be aware that modifying a container can invalidate its iterators.

struct IsEven {
    bool operator()(int value) const {
        return value % 2 == 0;
    }
};

int main() {
    std::cout << "Core iterators and callable examples:\n";

    std::vector<int> values{1, 2, 3, 4, 5, 6};
    std::cout << "even values:";
    std::for_each(values.begin(), values.end(), [](int value) {
        if (value % 2 == 0) {
            std::cout << ' ' << value;
        }
    });
    std::cout << "\n";

    auto firstEven = std::find_if(values.begin(), values.end(), IsEven());
    if (firstEven != values.end()) {
        std::cout << "first even = " << *firstEven << "\n";
    }

    return 0;
}
