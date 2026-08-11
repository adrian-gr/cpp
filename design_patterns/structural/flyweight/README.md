# Flyweight

## Description

Flyweight is a structural design pattern that minimizes memory use by sharing as
much data as possible between many similar objects. It works by splitting an
object's state into two categories:

- **Intrinsic state** — data that's shared, immutable, and identical across many
  instances (e.g. a font and size). This is stored once, inside the flyweight
  object itself.
- **Extrinsic state** — data that's unique per use (e.g. a character and its
  position on a page). This is *not* stored in the flyweight; it's passed in as
  parameters at the point of use.

A flyweight factory centralizes creation, caching flyweights by their intrinsic
state so identical values are never allocated twice — no matter how many objects
end up referencing them.

Typical participants:

- **Flyweight** — holds only intrinsic (shared, immutable) state, and exposes
  operations that accept extrinsic state as parameters (e.g. `GlyphStyle`).
- **FlyweightFactory** — creates and caches flyweights, returning an existing
  instance when one with matching intrinsic state already exists (e.g.
  `GlyphStyleFactory`).
- **Client** — holds extrinsic state itself, and a reference to the shared
  flyweight, combining the two when it needs to act (e.g. `Character`).

## When to Use

- An application needs to create a huge number of similar objects, and the
  per-object memory cost becomes a real, measurable problem — a text editor
  rendering thousands of glyphs, a game world with thousands of similar particles
  or tiles, a map renderer with thousands of similar icons.
- Most of an object's state can be made extrinsic (passed in from outside at the
  point of use), leaving a smaller amount of intrinsic state that's genuinely
  shared and reused across many instances.
- Object identity doesn't matter for the shared part — clients are fine working
  with a shared instance rather than needing a distinct object each time, since
  none of them will ever need to give that shared part unique, private state.

## When to Avoid

- The number of objects involved is small enough that memory sharing wouldn't
  meaningfully matter — the factory/cache machinery is pure overhead in that case.
- Most of an object's state is unique per instance, leaving little that's actually
  shareable as intrinsic state — if almost everything varies, there's little for
  Flyweight to share, and the pattern won't pay for its own complexity.
- Separating intrinsic from extrinsic state would make the code substantially
  harder to follow for a marginal memory benefit — readability and correctness
  should win over a memory optimization that isn't actually needed yet.

## How to Implement

1. **Identify intrinsic vs. extrinsic state.** Intrinsic: shared, immutable,
   identical across many objects. Extrinsic: unique per use, supplied by the
   caller.

2. **Define the Flyweight, holding only intrinsic state.** Operations take
   extrinsic state as parameters rather than reading it from stored fields.

   ```cpp
   class GlyphStyle {
   public:
       GlyphStyle(std::string font, int size) : font_(std::move(font)), size_(size) {}

       void render(char character, int x, int y) const {
           // character, x, y are extrinsic — passed in, never stored here.
           std::cout << "'" << character << "' at (" << x << "," << y << ") in "
                     << font_ << " " << size_ << "pt\n";
       }

   private:
       std::string font_;
       int size_;
   };
   ```

3. **Implement the Flyweight Factory.** Caches flyweights by their intrinsic state
   key, returning an existing instance whenever one already matches.

   ```cpp
   class GlyphStyleFactory {
   public:
       std::shared_ptr<GlyphStyle> getStyle(const std::string& font, int size) {
           std::string key = font + "_" + std::to_string(size);
           auto it = cache_.find(key);
           if (it != cache_.end()) return it->second;

           auto style = std::make_shared<GlyphStyle>(font, size);
           cache_[key] = style;
           return style;
       }

   private:
       std::unordered_map<std::string, std::shared_ptr<GlyphStyle>> cache_;
   };
   ```

4. **Have clients hold extrinsic state plus a shared flyweight reference.**

   ```cpp
   struct Character {
       char symbol;
       int x;
       int y;
       std::shared_ptr<GlyphStyle> style;   // shared, not owned exclusively

       void render() const { style->render(symbol, x, y); }
   };
   ```

5. **Always request flyweights through the factory.** Never construct a flyweight
   directly, or the caching guarantee is lost.

   ```cpp
   GlyphStyleFactory factory;
   auto headingStyle = factory.getStyle("Georgia", 24);

   Character h{'H', 0, 0, headingStyle};
   Character i{'i', 20, 0, headingStyle};   // same GlyphStyle instance, no new allocation
   ```

## Implementation Tips

- Keep flyweight objects immutable. Shared mutable state is a correctness hazard —
  if one client could mutate a shared flyweight, every other client referencing it
  would see that change unexpectedly.
- Centralize creation in a factory that caches and reuses flyweights by key; never
  let client code construct flyweight instances directly, or duplicates will slip
  in and defeat the memory-sharing goal.
- Pass extrinsic state as method parameters rather than storing it on the
  flyweight — the moment extrinsic data gets stored inside a "flyweight," it's no
  longer safely shareable across different uses.
- Use `std::shared_ptr` (or an equivalent reference-counted/pooled handle) for
  flyweight references from clients, so the factory's cache and the clients using
  a flyweight can coexist safely without one owning it exclusively.
- Measure before optimizing: Flyweight adds real complexity (factory, caching,
  intrinsic/extrinsic split), so confirm the number of objects and their memory
  footprint actually justify it before introducing the pattern.