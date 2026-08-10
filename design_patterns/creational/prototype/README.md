# Prototype

## Description

Prototype is a creational design pattern that lets you create new objects by copying
an existing, fully-configured instance rather than building one from scratch. Each
participating class exposes a `clone()` operation on a common interface; client code
calls `clone()` on whatever prototype it holds, without ever knowing — or needing to
know — the object's concrete type.

This is useful whenever "copy an existing thing" is cheaper, safer, or more
convenient than "construct a new thing," and it lets you spawn objects whose exact
runtime type is decided by which prototype was registered, not by a hardcoded
constructor call.

Typical participants:

- **Prototype** — the common interface declaring `clone()` (e.g. `Enemy`).
- **ConcretePrototype** — implements `clone()`, typically by copy-constructing itself
  (e.g. `Goblin`, `Dragon`).
- **Client / Registry** — holds one or more pre-configured prototype instances and
  produces new objects by cloning them, often looked up by key (e.g. `EnemySpawner`).

## When to Use

- Constructing an object is expensive — heavy setup, I/O, computed/derived state —
  and copying an already-built instance is cheaper than rebuilding it from
  parameters each time.
- The concrete type to instantiate isn't known until runtime, and you want to avoid a
  large switch/if-chain over type names to pick a constructor.
- You want to spawn many variations of a pre-configured "template" object — set up a
  handful of prototypes once, then clone them repeatedly (e.g. game enemy waves, UI
  component templates, document boilerplates).
- You want to add new object variants without modifying the code that creates
  them — register a new prototype instead of adding a new `case` or `if` branch.

## When to Avoid

- Construction is already cheap and straightforward — cloning adds an interface and
  indirection for no real benefit over a plain constructor.
- The object holds resources that don't copy safely by default — open file handles,
  network connections, objects with unique identity — unless you're prepared to write
  careful, correct deep-copy logic for them.
- There's only a small, fixed, well-known set of types — an ordinary constructor or a
  simple factory function is clearer than a clone-based registry.
- Shallow copies would silently share mutable state between "independent" clones,
  and getting deep-copy semantics right isn't worth the complexity for the use case.

## How to Implement

1. **Define the Prototype interface.** Declare `clone()` returning an owned pointer
   to the base type, alongside whatever other behavior the objects need.

   ```cpp
   class Enemy {
   public:
       virtual ~Enemy() = default;
       virtual std::unique_ptr<Enemy> clone() const = 0;
       virtual void describe() const = 0;
   };
   ```

2. **Implement Concrete Prototypes.** Each `clone()` implementation copy-constructs a
   new instance of itself, capturing its own current state.

   ```cpp
   class Goblin final : public Enemy {
   public:
       Goblin(int health, std::string weapon) : health_(health), weapon_(std::move(weapon)) {}

       std::unique_ptr<Enemy> clone() const override {
           return std::make_unique<Goblin>(*this);
       }

       void describe() const override { /* ... */ }

   private:
       int health_;
       std::string weapon_;
   };
   ```

3. **Set up prototype instances.** Construct one fully-configured instance per
   variant you want to be able to spawn.

   ```cpp
   auto goblinPrototype = std::make_unique<Goblin>(20, "rusty dagger");
   auto dragonPrototype = std::make_unique<Dragon>(500, "fire");
   ```

4. **(Optional) Build a registry for lookup by key.** Useful when the client picks a
   prototype at runtime by name rather than holding a direct reference.

   ```cpp
   class EnemySpawner {
   public:
       void registerPrototype(const std::string& key, std::unique_ptr<Enemy> prototype) {
           prototypes_[key] = std::move(prototype);
       }

       std::unique_ptr<Enemy> spawn(const std::string& key) const {
           return prototypes_.at(key)->clone();
       }

   private:
       std::unordered_map<std::string, std::unique_ptr<Enemy>> prototypes_;
   };
   ```

5. **Write client code against the abstraction.** The client clones through the
   `Enemy` interface (directly or via the registry) and never names a concrete class.

   ```cpp
   EnemySpawner spawner;
   spawner.registerPrototype("goblin_grunt", std::make_unique<Goblin>(20, "rusty dagger"));
   spawner.registerPrototype("ancient_dragon", std::make_unique<Dragon>(500, "fire"));

   auto wave1 = spawner.spawn("goblin_grunt");
   auto boss = spawner.spawn("ancient_dragon");
   ```

## Implementation Tips

- Implement `clone()` on top of a real copy constructor/assignment operator, not
  field-by-field ad hoc copying — this keeps clone semantics consistent with normal
  copy semantics for the class.
- Return an owning smart pointer (`std::unique_ptr`) from `clone()` so ownership of
  the new object is unambiguous.
- Decide deliberately between shallow and deep copy for any member that's itself a
  pointer or owns a resource — the default copy constructor may not be correct as-is.
- When clients select a template by name at runtime, keep a small registry
  (`std::unordered_map<std::string, std::unique_ptr<Prototype>>`) mapping keys to
  prototype instances, as with `EnemySpawner` above.