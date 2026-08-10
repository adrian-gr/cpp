# Singleton

## Description

Singleton is a creational design pattern that ensures a class has only one instance
for the lifetime of the program, and provides a single, well-known global access
point to that instance. Unlike the other creational patterns, Singleton doesn't
abstract over *which* concrete type gets created — it controls *how many* instances
of one specific type can ever exist.

The class enforces this itself: constructors are made private (or otherwise
inaccessible), and a static accessor method is the only way to reach the instance,
creating it on first use if necessary.

Typical participants:

- **Singleton** — the class itself. It declares a private constructor, deletes copy
  construction/assignment, and exposes a static `instance()` (or equivalent) method
  that returns a reference/pointer to the single shared instance.

There's no separate "product" or "family" hierarchy — Singleton is a constraint on
one class, not a relationship between multiple classes.

## When to Use

- Exactly one instance of a class must exist for the program's lifetime, and that's a
  genuine invariant of the system, not just a convenience — e.g. a logging facility,
  a configuration store, a connection pool, a hardware resource manager.
- The single instance needs to be reachable from many unrelated parts of the codebase
  without threading a reference through every constructor or function signature.
- You need controlled, lazy initialization of an expensive shared resource — the
  instance shouldn't be constructed until it's actually needed.
- You want to guarantee, at the type level, that a second instance can never be
  accidentally created (e.g. by deleting the copy constructor and assignment
  operator).

## When to Avoid

- The "only one instance" requirement is really just convenience rather than a hard
  invariant — in that case, an ordinary object passed around explicitly (or injected
  via a constructor/parameter) is more flexible and easier to reason about.
- It would introduce hidden global state, which tends to make unit tests
  order-dependent, hard to isolate, and hard to reset between test runs.
- There's a real chance you'll need multiple configurations or instances later (e.g.
  multiple loggers writing to different destinations) — Singleton bakes in "exactly
  one" at the type level, and unwinding that later means changing every call site.
- The class is being made a singleton mainly to give it "easy global access," rather
  than because uniqueness is actually required — that's usually a sign to use
  dependency injection instead.

## How to Implement

1. **Make the constructor private (or otherwise inaccessible).** No external code
   should be able to construct the class directly.

   ```cpp
   class Logger {
   private:
       Logger() = default;
   };
   ```

2. **Delete copy construction and copy assignment.** This prevents a second instance
   from being created by copying an existing one.

   ```cpp
   Logger(const Logger&) = delete;
   Logger& operator=(const Logger&) = delete;
   ```

3. **Add a static accessor that owns and returns the single instance.** In C++11 and
   later, a function-local `static` variable (the "Meyers' Singleton") gives
   thread-safe, lazy initialization for free — the standard guarantees the first
   thread to reach the declaration performs the initialization, and any others wait.

   ```cpp
   class Logger {
   public:
       static Logger& instance() {
           static Logger instance_;
           return instance_;
       }

       Logger(const Logger&) = delete;
       Logger& operator=(const Logger&) = delete;

       void log(const std::string& message) {
           std::cout << "[log] " << message << "\n";
       }

   private:
       Logger() = default;
   };
   ```

4. **Access the instance only through the accessor.** Client code never constructs a
   `Logger` directly.

   ```cpp
   int main() {
       Logger::instance().log("application starting");
       Logger::instance().log("application finished");
   }
   ```

## Implementation Tips

- Prefer the function-local `static` (Meyers' Singleton) over a raw global pointer
  with manual `if (instance_ == nullptr)` checks — the latter is not thread-safe
  without extra locking, while the local static is guaranteed safe by the C++11+
  standard.
- If the singleton is accessed from multiple threads and has mutable state (like a
  log stream or a counter), guard that state with its own mutex — thread-safe
  *construction* doesn't imply thread-safe *methods*.
- Keep the singleton's responsibilities narrow. A singleton that accumulates more and
  more unrelated behavior over time becomes a hidden "god object" that's hard to test
  and hard to reason about.
- If you find yourself wanting to swap the singleton's behavior in tests (e.g. a fake
  logger), consider whether dependency injection with an interface — rather than a
  hardcoded global — would serve you better in the long run.