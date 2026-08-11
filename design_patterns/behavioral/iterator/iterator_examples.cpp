// Iterator pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/iterator/iterator_examples.cpp -o bin/iterator_example && ./bin/iterator_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Iterator provides a way to access the elements of a collection sequentially
// without exposing its underlying representation (array, linked list, tree, etc.).
// The collection exposes a way to create an iterator; the client walks the
// collection through that iterator's interface alone.
//
// Use Iterator when:
// - clients need to traverse a collection without knowing its internal structure;
// - you want to support multiple simultaneous or multiple kinds of traversal over
//   the same collection without bloating the collection's own interface;
// - you want a uniform way to iterate over different collection types.
//
// Avoid Iterator when:
// - the language/library's built-in iteration (e.g. range-based for, STL iterators)
//   already covers the need — reinventing it adds ceremony with no benefit;
// - the collection's structure is simple and traversal needs are fixed and trivial;
// - exposing the underlying structure directly is fine and no encapsulation benefit
//   is gained by hiding it behind an iterator.
//
// Tips:
// - Keep the iterator's interface minimal: hasNext()/next() or begin()/end()-style.
// - Let each iterator instance carry its own traversal position and state, so
//   multiple independent iterations over the same collection can run concurrently.
// - Prefer read-only iteration unless mutation during traversal is a real requirement.

template <typename T>
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual bool hasNext() const = 0;
    virtual const T& next() = 0;
};

template <typename T>
class Collection {
public:
    virtual ~Collection() = default;
    virtual std::unique_ptr<Iterator<T>> createIterator() const = 0;
};

class PlaylistIterator final : public Iterator<std::string> {
public:
    explicit PlaylistIterator(const std::vector<std::string>& tracks) : tracks_(tracks) {}

    bool hasNext() const override {
        return index_ < tracks_.size();
    }

    const std::string& next() override {
        return tracks_[index_++];
    }

private:
    const std::vector<std::string>& tracks_;
    size_t index_ = 0;
};

class Playlist final : public Collection<std::string> {
public:
    void addTrack(const std::string& track) {
        tracks_.push_back(track);
    }

    std::unique_ptr<Iterator<std::string>> createIterator() const override {
        return std::make_unique<PlaylistIterator>(tracks_);
    }

private:
    std::vector<std::string> tracks_;
};

void printAll(const Collection<std::string>& collection) {
    auto it = collection.createIterator();
    while (it->hasNext()) {
        std::cout << "track: " << it->next() << "\n";
    }
}

int main() {
    std::cout << "Iterator pattern example:\n";

    Playlist playlist;
    playlist.addTrack("Sunset Drive");
    playlist.addTrack("Night Owl");
    playlist.addTrack("Morning Static");

    printAll(playlist);

    return 0;
}