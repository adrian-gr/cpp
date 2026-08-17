# Facade

## Description

Facade is a structural design pattern that provides a simplified, unified
interface to a set of interfaces in a complex subsystem. Instead of clients
coordinating a subsystem's components directly — learning each class's interface
and calling them in the right order — clients interact with a single facade object
that hides that complexity behind a small set of high-level operations.

The facade doesn't replace the subsystem or restrict access to it; it just gives
most callers an easier way in. The subsystem's classes stay fully intact and
independently usable by anyone who needs finer-grained control.

Typical participants:

- **Facade** — offers a small set of high-level operations, internally
  coordinating calls to the subsystem in the right sequence (e.g.
  `HomeTheaterFacade`).
- **Subsystem classes** — the existing, more complex set of classes with their own
  (often lower-level, interdependent) interfaces, unaware that a facade exists
  (e.g. `Amplifier`, `Projector`, `StreamingPlayer`).

## When to Use

- A subsystem is complex, with many classes and interdependent calls, and most
  clients only need a handful of common, high-level operations from it — not the
  subsystem's full flexibility.
- You want to decouple client code from a subsystem's internals, so the subsystem
  can be refactored or reimplemented internally without breaking every caller that
  depends on it.
- You want to layer a system, with each layer talking to the layer below it
  through a facade rather than reaching directly into that layer's internal
  classes — this keeps layer boundaries clean and dependencies one-directional.

## When to Avoid

- The subsystem is already simple enough that a wrapper adds a layer of
  indirection without actually simplifying anything for callers.
- Advanced clients genuinely need the subsystem's full flexibility, and a
  simplified facade would hide operations they legitimately need — in that case,
  make sure the subsystem's classes stay directly accessible rather than being
  gated exclusively behind the facade.
- The facade starts absorbing real business logic beyond coordination — if it's
  making decisions rather than just sequencing calls, it's grown into a
  god-object doing the subsystem's job, not simplifying access to it.

## How to Implement

1. **Identify the subsystem classes and their interdependent calls.** These stay
   as they are — Facade doesn't require modifying them.

   ```cpp
   class Amplifier {
   public:
       void on() { std::cout << "amplifier: on\n"; }
       void setVolume(int level) { std::cout << "amplifier: volume set to " << level << "\n"; }
       void off() { std::cout << "amplifier: off\n"; }
   };
   // Projector, StreamingPlayer follow the same shape.
   ```

2. **Define the Facade, holding references to the subsystem classes it
   coordinates.**

   ```cpp
   class HomeTheaterFacade {
   public:
       HomeTheaterFacade(Amplifier& amp, Projector& projector, StreamingPlayer& player)
           : amp_(amp), projector_(projector), player_(player) {}

   private:
       Amplifier& amp_;
       Projector& projector_;
       StreamingPlayer& player_;
   };
   ```

3. **Implement high-level operations that sequence the low-level subsystem
   calls.**

   ```cpp
   void watchMovie(const std::string& title) {
       amp_.on();
       amp_.setVolume(7);
       projector_.on();
       projector_.setInput("streaming");
       player_.on();
       player_.play(title);
   }

   void endMovie() {
       player_.off();
       projector_.off();
       amp_.off();
   }
   ```

4. **Have clients call the facade instead of the subsystem directly.**

   ```cpp
   Amplifier amp;
   Projector projector;
   StreamingPlayer player;
   HomeTheaterFacade homeTheater(amp, projector, player);

   homeTheater.watchMovie("The Matrix");   // one call instead of six
   homeTheater.endMovie();
   ```

## Implementation Tips

- Keep the facade's interface focused on the common-case operations most clients
  actually need — resist growing it into a full mirror of the subsystem's
  interface, or it stops simplifying anything.
- The facade should coordinate, not replace, the subsystem — let advanced clients
  still reach the subsystem's classes directly when they need finer control that
  the facade doesn't expose.
- Avoid putting subsystem-specific business logic in the facade; it should
  orchestrate calls in the right order, not contain the real decision-making logic
  itself — that logic belongs in the subsystem classes.
- If different groups of clients need meaningfully different simplified views of
  the same subsystem, consider multiple facades (or facade methods) rather than
  cramming every use case into one interface.
- A facade can be combined with other patterns — e.g. it might use Singleton if
  only one instance ever makes sense, or Abstract Factory internally to construct
  the subsystem objects it coordinates.