// Flyweight pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/flyweight/flyweight_examples.cpp -o bin/flyweight_example && ./bin/flyweight_example

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// Description:
// Flyweight minimizes memory use by sharing as much data as possible between many
// similar objects. Object state is split into intrinsic state (shared, immutable,
// stored once per distinct value) and extrinsic state (unique per instance, passed
// in at the point of use rather than stored in the shared object).
//
// Use Flyweight when:
// - an application needs to create a huge number of similar objects, and the
//   per-object memory cost becomes a real problem;
// - most of an object's state can be made extrinsic (passed in from outside) and
//   the remaining intrinsic state is shared across many instances;
// - object identity doesn't matter — clients can work with shared instances rather
//   than needing distinct objects.
//
// Avoid Flyweight when:
// - the number of objects is small enough that memory sharing wouldn't matter;
// - most of an object's state is unique per instance, leaving little that's
//   actually shareable — the factory/cache machinery adds complexity for no gain;
// - separating intrinsic from extrinsic state would make the code substantially
//   harder to follow for a marginal memory benefit.
//
// Tips:
// - Keep flyweight objects immutable; shared mutable state is a correctness hazard.
// - Centralize creation in a factory that caches and reuses flyweights by key.
// - Pass extrinsic state as method parameters rather than storing it on the flyweight.

// Intrinsic state: shared, immutable data reused across many glyph instances.
class GlyphStyle {
public:
    GlyphStyle(std::string font, int size) : font_(std::move(font)), size_(size) {}

    void render(char character, int x, int y) const {
        // x, y are extrinsic: passed in per use, never stored on GlyphStyle.
        std::cout << "'" << character << "' at (" << x << "," << y << ") in "
                  << font_ << " " << size_ << "pt\n";
    }

private:
    std::string font_;
    int size_;
};

// Flyweight factory: caches and reuses GlyphStyle instances by key, so identical
// styles are never allocated twice regardless of how many characters use them.
class GlyphStyleFactory {
public:
    std::shared_ptr<GlyphStyle> getStyle(const std::string& font, int size) {
        std::string key = font + "_" + std::to_string(size);
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            return it->second;
        }
        auto style = std::make_shared<GlyphStyle>(font, size);
        cache_[key] = style;
        std::cout << "created new style: " << key << "\n";
        return style;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<GlyphStyle>> cache_;
};

// Each Character holds only its own identity plus a shared style pointer, not a
// full copy of font/size data.
struct Character {
    char symbol;
    int x;
    int y;
    std::shared_ptr<GlyphStyle> style;

    void render() const {
        style->render(symbol, x, y);
    }
};

int main() {
    std::cout << "Flyweight pattern example:\n";

    GlyphStyleFactory factory;
    auto bodyStyle = factory.getStyle("Georgia", 12);
    auto headingStyle = factory.getStyle("Georgia", 24);

    std::vector<Character> document;
    document.push_back({'H', 0, 0, headingStyle});
    document.push_back({'i', 20, 0, headingStyle});
    document.push_back({'t', 0, 30, bodyStyle});
    document.push_back({'h', 10, 30, bodyStyle});
    document.push_back({'e', 20, 30, bodyStyle});
    document.push_back({'r', 30, 30, bodyStyle});
    document.push_back({'e', 40, 30, factory.getStyle("Georgia", 12)}); // reuses cached style

    for (const auto& character : document) {
        character.render();
    }

    return 0;
}