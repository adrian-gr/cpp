# Iterator

## Description

Iterator is a behavioral design pattern that provides a way to access the elements
of a collection sequentially, without exposing the collection's underlying
representation (array, linked list, tree, hash map, etc.). The collection exposes a
method to produce an iterator; client code then walks the collection entirely
through that iterator's interface, never touching the collection's internals
directly.

This decouples traversal logic from the collection's storage details, and lets
different collection types be traversed through the same uniform interface.

Typical participants:

- **Iterator** — declares the traversal interface, typically something like
  `hasNext()`/`next()` (e.g. `Iterator<T>`).
- **ConcreteIterator** — implements traversal for one specific collection, tracking
  its own position/state (e.g. `PlaylistIterator`).
- **Collection** (or "Aggregate") — declares a method to produce an iterator over
  itself (e.g. `Collection<T>`).
- **ConcreteCollection** — implements the actual storage and produces a matching
  concrete iterator (e.g. `Playlist`).

## When to Use

- Client code needs to traverse a collection without knowing or depending on its
  internal structure — the collection's storage can change (array to linked list,
  say) without breaking client traversal code.
- You want to support multiple simultaneous, or multiple different kinds of,
  traversal over the same collection (forward, reverse, filtered) without bloating
  the collection's own interface with every traversal variant.
- You want a uniform way to iterate over different collection types — client code
  that walks a `Collection<T>` via `Iterator<T>` works unchanged for any concrete
  collection implementing that interface.
- Multiple independent traversals of the same collection need to happen
  concurrently, each with its own position — since each iterator instance carries
  its own state.

## When to Avoid

- The language or standard library's built-in iteration mechanism (range-based
  `for`, STL iterators, generators) already covers the need — reimplementing your
  own `Iterator`/`Collection` interfaces on top of that adds ceremony with no
  benefit.
- The collection's structure is simple and its traversal needs are fixed and
  trivial — a plain accessor or direct member access is clearer than an iterator
  abstraction.
- There's no real encapsulation benefit to hiding the underlying structure — if
  exposing it directly is already fine for every client, the extra interface layer
  isn't earning its keep.

## How to Implement

1. **Define the Iterator interface.** A minimal traversal contract — usually
   `hasNext()` and `next()`.

   ```cpp
   template <typename T>
   class Iterator {
   public:
       virtual ~Iterator() = default;
       virtual bool hasNext() const = 0;
       virtual const T& next() = 0;
   };
   ```

2. **Define the Collection interface.** Declares a way to produce an iterator over
   the collection's contents.

   ```cpp
   template <typename T>
   class Collection {
   public:
       virtual ~Collection() = default;
       virtual std::unique_ptr<Iterator<T>> createIterator() const = 0;
   };
   ```

3. **Implement a Concrete Iterator.** Holds a reference to the underlying data and
   tracks its own traversal position.

   ```cpp
   class PlaylistIterator final : public Iterator<std::string> {
   public:
       explicit PlaylistIterator(const std::vector<std::string>& tracks) : tracks_(tracks) {}

       bool hasNext() const override { return index_ < tracks_.size(); }
       const std::string& next() override { return tracks_[index_++]; }

   private:
       const std::vector<std::string>& tracks_;
       size_t index_ = 0;
   };
   ```

4. **Implement a Concrete Collection.** Owns the actual storage and hands out a
   matching iterator on request.

   ```cpp
   class Playlist final : public Collection<std::string> {
   public:
       void addTrack(const std::string& track) { tracks_.push_back(track); }

       std::unique_ptr<Iterator<std::string>> createIterator() const override {
           return std::make_unique<PlaylistIterator>(tracks_);
       }

   private:
       std::vector<std::string> tracks_;
   };
   ```

5. **Write client code against the abstractions only.** No reference to `Playlist`'s
   internal `std::vector`, or even to `Playlist` itself, is required.

   ```cpp
   void printAll(const Collection<std::string>& collection) {
       auto it = collection.createIterator();
       while (it->hasNext()) {
           std::cout << "track: " << it->next() << "\n";
       }
   }
   ```

## Implementation Tips

- Keep the iterator's interface minimal — `hasNext()`/`next()` or `begin()`/`end()`
  style is usually enough; a bloated iterator interface undermines the pattern's
  simplicity.
- Let each iterator instance carry its own position/state rather than storing
  traversal state on the collection itself — this is what allows multiple
  independent iterations over the same collection to run concurrently without
  interfering with each other.
- Prefer read-only iteration unless mutating the collection during traversal is a
  genuine requirement; supporting safe mutation during iteration (insertion/removal)
  adds real complexity and invalidation concerns.
- In C++, consider whether STL-style iterators (supporting `begin()`/`end()` and
  range-based `for`) would serve better than a custom `hasNext()`/`next()`
  interface — the STL convention interops with the standard library and language
  syntax for free.
- If the collection type can vary widely (tree, graph, linked list), keep the
  iterator's traversal strategy (depth-first, breadth-first, in-order, etc.) as part
  of the concrete iterator, not the abstract interface, so different strategies can
  coexist behind the same `Iterator<T>` contract.