// Core standard container examples
// Compile with: g++ -std=c++11 -O2 cpp_core/containers_examples.cpp -o bin/core_containers && ./bin/core_containers

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

// Tips:
// - Choose a container based on access, ordering, lookup, and ownership needs.
// - Prefer vector by default when contiguous storage and iteration are useful.
// - Use map or set for ordered keys and unordered containers for hash-based lookup.
// - Check iterator invalidation rules when modifying a container.

int main() {
    std::cout << "Core standard container examples:\n";
    std::vector<int> ordered{3, 1, 2};
    std::set<int> unique{3, 1, 2, 2};
    std::map<std::string, int> scores{{"Ada", 10}};
    std::unordered_map<int, std::string> names{{1, "one"}};
    std::cout << "vector size = " << ordered.size() << "\n";
    std::cout << "set size = " << unique.size() << "\n";
    std::cout << "map score = " << scores.at("Ada") << "\n";
    std::cout << "hash lookup = " << names.at(1) << "\n";
    return 0;
}
