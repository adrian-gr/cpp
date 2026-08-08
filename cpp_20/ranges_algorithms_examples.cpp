// C++20 ranges algorithms examples
// Reference: https://en.cppreference.com/w/cpp/algorithm/ranges
// Compile with: g++ -std=c++20 -O2 -Wall -Wextra cpp_20/ranges_algorithms_examples.cpp -o bin/cpp20_ranges_algorithms && ./bin/cpp20_ranges_algorithms

#include <algorithm>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

// Problem solved: applies algorithms directly to ranges without manually passing iterator pairs.
// Before C++20: code repeatedly wrote begin/end calls and adapted pipelines by hand.
// Tips for ranges algorithms:
// - Prefer range overloads when the complete container or view is available.
// - Use projections to compare a member without writing a separate transforming container.
// - Views are lazy and non-owning; keep their source range alive while using them.
// - Use an eager container when results must be stored independently of the source.

struct Record {
    std::string name;
    int score;
};

int main() {
    std::cout << "C++20 ranges algorithms examples:\n";

    // 1. ranges::sort accepts the container directly and uses a projection.
    std::vector<Record> records{{"Ada", 91}, {"Grace", 98}, {"Linus", 87}};
    std::ranges::sort(records, {}, &Record::score);
    std::cout << "lowest score = " << records.front().name << "\n";

    // 2. ranges::find returns an iterator that can be compared with end.
    auto found = std::ranges::find(records, 98, &Record::score);
    if (found != records.end()) {
        std::cout << "found score owner = " << found->name << "\n";
    }

    // 3. A view pipeline filters and transforms lazily.
    auto highScores = records
        | std::views::filter([](const Record& record) {
              return record.score >= 90;
          })
        | std::views::transform([](const Record& record) {
              return record.name + " passed";
          });

    std::cout << "high scores:";
    for (const std::string& result : highScores) {
        std::cout << ' ' << result;
    }
    std::cout << "\n";

    // 4. ranges algorithms can write into a separate eager result container.
    std::vector<int> scores;
    scores.reserve(records.size());
    std::ranges::transform(records, std::back_inserter(scores), &Record::score);
    std::cout << "score count = " << scores.size() << "\n";

    return 0;
}
