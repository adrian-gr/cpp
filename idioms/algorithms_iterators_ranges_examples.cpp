// Algorithms + iterators over raw loops — <algorithm>, the STL iterator model,
// and C++20 ranges as the modern successor
// Compile with: g++ -std=c++20 -O2 idioms/algorithms_iterators_ranges_examples.cpp -o bin/algorithms_iterators_ranges_example && ./bin/algorithms_iterators_ranges_example

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <string>
#include <vector>

// Description:
// The STL separates CONTAINERS (vector, list, map...) from ALGORITHMS (sort,
// find, transform...) via a common abstraction: ITERATORS. An algorithm doesn't
// know or care what container it's traversing — it only knows how to advance an
// iterator, dereference it, and compare it to another. This is what lets one
// `std::sort` implementation work identically over a std::vector, a std::deque,
// or a raw array, and it's the reason writing a raw hand-rolled for-loop is
// usually a step DOWN in both expressiveness and safety compared to reaching
// for the matching named algorithm.
//
// A raw loop states HOW to do something (initialize an index, check a bound,
// increment, dereference) and leaves the reader to infer WHAT it's for. A named
// algorithm call (`std::find`, `std::count_if`, `std::accumulate`) states WHAT
// directly, and the "how" — the loop, the bounds-checking, the edge cases — is
// handled once, correctly, inside the standard library, instead of being
// re-derived (and potentially re-broken) at every call site.
//
// C++20 ranges take this one step further: instead of passing a
// begin/end ITERATOR PAIR to every algorithm (std::sort(v.begin(), v.end())),
// ranges algorithms accept the CONTAINER ITSELF (std::ranges::sort(v)) — no
// mismatched-iterator bugs (accidentally passing v1.begin() with v2.end()), and
// ranges support LAZY, COMPOSABLE views (filter, transform, take...) that chain
// together without allocating an intermediate container at each step.
//
// Use algorithms (over raw loops) when:
// - the operation you need already has a name in <algorithm>/<numeric> — find,
//   count, sort, transform, accumulate, any_of/all_of/none_of, remove, unique...
//   a named call is more readable and less bug-prone than reimplementing it;
// - you want a guarantee the operation is implemented correctly for edge cases
//   (empty ranges, single elements) without re-deriving them at every call site.
//
// Use iterators (over raw indices) when:
// - writing generic code that should work across container types — an
//   iterator-based function works for vector, list, and set alike; an
//   index-based one (v[i]) silently assumes random access and breaks for list.
//
// Use C++20 ranges when:
// - available (C++20+) — ranges algorithms are safer (no mismatched iterator
//   pairs), and views let you express a pipeline of operations (filter, then
//   transform, then take) without allocating an intermediate container for
//   each stage, evaluating lazily as the final result is consumed.
//
// Avoid reaching for an algorithm when:
// - the loop body has genuinely unique, one-off logic that doesn't correspond
//   to any standard algorithm and forcing one (e.g. abusing std::for_each with
//   a complex lambda) would be less clear than a plain loop — algorithms exist
//   to replace loops that already have a name, not to eliminate loops entirely.
//
// Tips:
// - Reach for <algorithm>/<numeric> before writing a for-loop; check if what
//   you're about to write already has a name.
// - Prefer std::ranges:: versions over the classic iterator-pair versions in
//   new C++20 code — same algorithms, safer call sites, and composable views.
// - A views pipeline (v | std::views::filter(...) | std::views::transform(...))
//   is lazy: nothing is actually computed until the final range is iterated or
//   materialized, so intermediate stages never allocate their own storage.

int main() {
    std::cout << "Algorithms + iterators + ranges example:\n";

    std::vector<int> numbers = {5, 3, 8, 1, 9, 2, 7, 4, 6};

    // ---------------------------------------------------------------------
    // 1. Raw loop vs. named algorithm: same result, different clarity/safety.
    // ---------------------------------------------------------------------
    std::cout << "-- raw loop vs. algorithm --\n";

    // Raw loop version: states HOW (index, bound check, increment, compare).
    bool foundRawLoop = false;
    for (size_t i = 0; i < numbers.size(); ++i) {
        if (numbers[i] == 9) {
            foundRawLoop = true;
            break;
        }
    }
    std::cout << "raw loop found 9: " << std::boolalpha << foundRawLoop << "\n";

    // Algorithm version: states WHAT — "does 9 exist in this range?"
    bool foundAlgorithm = std::find(numbers.begin(), numbers.end(), 9) != numbers.end();
    std::cout << "std::find found 9: " << foundAlgorithm << "\n";

    // ---------------------------------------------------------------------
    // 2. A few more named algorithms replacing what would otherwise be loops.
    // ---------------------------------------------------------------------
    std::cout << "\n-- more algorithms replacing loops --\n";

    std::sort(numbers.begin(), numbers.end());
    std::cout << "sorted: ";
    for (int n : numbers) std::cout << n << " ";
    std::cout << "\n";

    int evenCount = std::count_if(numbers.begin(), numbers.end(),
                                    [](int n) { return n % 2 == 0; });
    std::cout << "even count: " << evenCount << "\n";

    int sum = std::accumulate(numbers.begin(), numbers.end(), 0);
    std::cout << "sum: " << sum << "\n";

    bool allPositive = std::all_of(numbers.begin(), numbers.end(),
                                     [](int n) { return n > 0; });
    std::cout << "all positive: " << allPositive << "\n";

    // ---------------------------------------------------------------------
    // 3. Iterator model generality: the SAME algorithm call works whether
    // numbers is a vector, a list, or a set — because std::find only needs
    // ITERATORS (begin/end, ++, ==), not random-access indexing.
    // ---------------------------------------------------------------------
    std::cout << "\n-- iterator generality --\n";

    std::vector<std::string> words = {"pear", "apple", "kiwi", "banana"};
    auto it = std::find(words.begin(), words.end(), "kiwi");
    if (it != words.end()) {
        std::cout << "found: " << *it << " at position "
                  << std::distance(words.begin(), it) << "\n";
    }
    // The exact same std::find call, with the exact same signature, would
    // work unchanged on a std::list<std::string> or std::set<std::string> —
    // it never assumes random access, only ++it and it != end().

    // ---------------------------------------------------------------------
    // 4. C++20 ranges: pass the container directly (no iterator pair to get
    // wrong), and compose lazy views instead of materializing intermediate
    // containers at every step.
    // ---------------------------------------------------------------------
    std::cout << "\n-- C++20 ranges --\n";

    std::vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // std::ranges::sort(data) instead of std::sort(data.begin(), data.end())
    // — no chance of mismatching begin()/end() from two different containers.
    std::ranges::sort(data, std::greater{});
    std::cout << "sorted descending: ";
    for (int n : data) std::cout << n << " ";
    std::cout << "\n";

    // A views pipeline: filter even numbers, square them, take the first 3 —
    // entirely lazy. Nothing is computed until the range-for below actually
    // pulls values through the pipeline; no intermediate vector is allocated
    // for "the evens" or "the squares" at any stage.
    auto pipeline = data
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::transform([](int n) { return n * n; })
        | std::views::take(3);

    std::cout << "first 3 squared evens (lazy pipeline): ";
    for (int n : pipeline) std::cout << n << " ";
    std::cout << "\n";

    return 0;
}