# Observer

## Description

Observer is a behavioral design pattern that defines a one-to-many dependency
between objects, so that when one object (the **subject**) changes state, all of its
dependents (**observers**) are notified and updated automatically. The subject only
knows that it has a list of objects implementing a common observer interface — it
never knows, or needs to know, their concrete types.

This is the pattern behind most publish/subscribe and event-listener mechanisms:
one piece of state changes, and an open-ended, runtime-configurable set of
subscribers react to that change without the state owner being coupled to any of
them individually.

Typical participants:

- **Subject** — owns the state and a collection of subscribed observers; exposes
  `subscribe()`/`unsubscribe()` and triggers notification when its state changes
  (e.g. `TemperatureSensor`).
- **Observer** — the common interface all subscribers implement, usually a single
  update-style method (e.g. `onTemperatureChanged(double)`).
- **ConcreteObserver** — a specific subscriber reacting to notifications in its own
  way (e.g. `DisplayPanel`, `AlarmSystem`).

## When to Use

- A change to one object must automatically propagate to an open-ended set of other
  objects, and the subject shouldn't need to know their concrete types up front.
- The set of dependents can change at runtime — objects subscribe and unsubscribe
  as the program runs, rather than being fixed at compile time.
- You want to decouple the object that owns and mutates state from the objects that
  merely react to changes in that state, so each side can evolve independently.
- You're implementing UI data-binding, event systems, or any "notify interested
  parties when X happens" mechanism.

## When to Avoid

- There's a small, fixed set of dependents that will never change — a direct method
  call is simpler and easier to trace through than an observer indirection layer.
- Notification order or delivery guarantees matter a great deal, and a simple
  broadcast loop can't express what you need — an explicit event bus or message
  queue with defined ordering/guarantees is a better fit.
- Update chains can cascade — an observer's reaction triggers another state change,
  which notifies more observers — and you're not prepared to guard against feedback
  loops; this can turn into a hard-to-debug chain of implicit calls.
- You need to reason precisely about *when* and *in what order* observers run;
  Observer's implicit, distributed control flow makes that harder to guarantee than
  explicit sequential calls.

## How to Implement

1. **Define the Observer interface.** Usually one method, describing what changed.

   ```cpp
   class Observer {
   public:
       virtual ~Observer() = default;
       virtual void onTemperatureChanged(double celsius) = 0;
   };
   ```

2. **Implement Concrete Observers.** Each reacts to the notification in its own way.

   ```cpp
   class DisplayPanel final : public Observer {
   public:
       explicit DisplayPanel(std::string label) : label_(std::move(label)) {}

       void onTemperatureChanged(double celsius) override {
           std::cout << label_ << " display: " << celsius << "C\n";
       }

   private:
       std::string label_;
   };
   ```

3. **Implement the Subject.** It owns the state, a list of observer pointers, and
   `subscribe()`/`unsubscribe()` methods to manage that list at runtime.

   ```cpp
   class TemperatureSensor {
   public:
       void subscribe(Observer* observer) { observers_.push_back(observer); }

       void unsubscribe(Observer* observer) {
           observers_.erase(std::remove(observers_.begin(), observers_.end(), observer),
                             observers_.end());
       }

       void setTemperature(double celsius) {
           temperature_ = celsius;
           notify();
       }

   private:
       void notify() {
           for (Observer* observer : observers_) {
               observer->onTemperatureChanged(temperature_);
           }
       }

       double temperature_ = 0.0;
       std::vector<Observer*> observers_;
   };
   ```

4. **Wire observers to the subject, and let state changes drive notification.**

   ```cpp
   TemperatureSensor sensor;
   DisplayPanel kitchenDisplay("kitchen");
   AlarmSystem alarm;

   sensor.subscribe(&kitchenDisplay);
   sensor.subscribe(&alarm);

   sensor.setTemperature(22.5);   // both observers notified automatically

   sensor.unsubscribe(&kitchenDisplay);
   sensor.setTemperature(41.0);   // only the alarm is notified now
   ```

## Implementation Tips

- Keep the observer interface minimal — usually a single update method is enough;
  a bloated interface makes every observer implement methods it doesn't care about.
- Let observers manage their own subscribe/unsubscribe lifetime where practical
  (e.g. subscribe in a constructor, unsubscribe in a destructor) to avoid dangling
  pointers if an observer outlives its usefulness but the subject still holds it.
- Prefer passing just the changed data (as in `onTemperatureChanged(double)`) rather
  than a reference to the whole subject — this keeps observers loosely coupled to
  the subject's full interface and internal state.
- If observers might unsubscribe themselves *during* a notification callback, be
  careful about iterating and mutating the same list at once — copy the list or use
  an iteration strategy that tolerates in-flight removal.
- For multithreaded subjects, guard the observer list (and notification) with a
  mutex, or use a lock-free/copy-on-write list, to avoid races between subscribe,
  unsubscribe, and notify.