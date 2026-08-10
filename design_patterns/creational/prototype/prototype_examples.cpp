// Prototype pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/creational/prototype/prototype_examples.cpp -o bin/prototype_example && ./bin/prototype_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

// Description:
// Prototype lets you copy existing objects without coupling your code to their
// concrete classes. A common interface exposes a clone() operation; each concrete
// class implements it to produce a fully-configured copy of itself.
//
// Use Prototype when:
// - constructing an object is expensive (setup, I/O, computed state) and copying an
//   already-configured instance is cheaper than building one from scratch;
// - you need to create objects whose exact concrete type isn't known until runtime,
//   without a big switch/if chain over types;
// - you want to spawn variations of a pre-configured "template" object at runtime.
//
// Avoid Prototype when:
// - construction is already cheap and simple — cloning adds indirection for no gain;
// - the object holds resources that don't copy safely (open handles, unique identity)
//   without careful deep-copy logic;
// - a small, fixed set of types makes ordinary constructors or a factory clearer.
//
// Tips:
// - Implement clone() with a real copy constructor/assignment underneath, not ad hoc field copying.
// - Return owning smart pointers (std::unique_ptr) from clone() for clear ownership.
// - Keep a small prototype registry when clients pick a template by name/key at runtime.

class Enemy {
public:
    virtual ~Enemy() = default;
    virtual std::unique_ptr<Enemy> clone() const = 0;
    virtual void describe() const = 0;
};

class Goblin final : public Enemy {
public:
    Goblin(int health, std::string weapon) : health_(health), weapon_(std::move(weapon)) {}

    std::unique_ptr<Enemy> clone() const override {
        return std::make_unique<Goblin>(*this);
    }

    void describe() const override {
        std::cout << "goblin: " << health_ << " hp, wields " << weapon_ << "\n";
    }

private:
    int health_;
    std::string weapon_;
};

class Dragon final : public Enemy {
public:
    Dragon(int health, std::string breath) : health_(health), breath_(std::move(breath)) {}

    std::unique_ptr<Enemy> clone() const override {
        return std::make_unique<Dragon>(*this);
    }

    void describe() const override {
        std::cout << "dragon: " << health_ << " hp, breathes " << breath_ << "\n";
    }

private:
    int health_;
    std::string breath_;
};

// Registry of pre-configured prototypes, looked up and cloned at spawn time.
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

int main() {
    std::cout << "Prototype pattern example:\n";

    EnemySpawner spawner;
    spawner.registerPrototype("goblin_grunt", std::make_unique<Goblin>(20, "rusty dagger"));
    spawner.registerPrototype("ancient_dragon", std::make_unique<Dragon>(500, "fire"));

    auto wave1 = spawner.spawn("goblin_grunt");
    auto wave2 = spawner.spawn("goblin_grunt");
    auto boss = spawner.spawn("ancient_dragon");

    wave1->describe();
    wave2->describe();
    boss->describe();

    return 0;
}