// Facade pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/facade/facade_examples.cpp -o bin/facade_example && ./bin/facade_example

#include <iostream>
#include <string>

// Description:
// Facade provides a simplified, unified interface to a set of interfaces in a
// complex subsystem. Clients interact with the facade instead of coordinating the
// subsystem's components directly, hiding their complexity and interdependencies
// behind one easy-to-use entry point.
//
// Use Facade when:
// - a subsystem is complex, with many classes and interdependent calls, and most
//   clients only need a handful of common, high-level operations from it;
// - you want to decouple client code from a subsystem's internals, so the
//   subsystem can change internally without breaking every caller;
// - you want to layer a system, with each layer talking to the one below it
//   through a facade rather than reaching into its internals directly.
//
// Avoid Facade when:
// - the subsystem is already simple enough that a wrapper adds a layer of
//   indirection without simplifying anything;
// - advanced clients genuinely need the subsystem's full flexibility and a
//   simplified facade would hide operations they legitimately need;
// - the facade starts absorbing business logic beyond coordination, growing into
//   a god-object that does the subsystem's job rather than simplifying access to it.
//
// Tips:
// - Keep the facade's interface focused on the common-case operations most clients need.
// - The facade should coordinate, not replace, the subsystem — let advanced
//   clients still reach the subsystem's classes directly if they need to.
// - Avoid putting subsystem-specific business logic in the facade; it should
//   orchestrate calls, not contain the real logic itself.

// Subsystem: several interdependent classes with a nontrivial, low-level interface.
class Amplifier {
public:
    void on() { std::cout << "amplifier: on\n"; }
    void setVolume(int level) { std::cout << "amplifier: volume set to " << level << "\n"; }
    void off() { std::cout << "amplifier: off\n"; }
};

class Projector {
public:
    void on() { std::cout << "projector: on\n"; }
    void setInput(const std::string& source) { std::cout << "projector: input set to " << source << "\n"; }
    void off() { std::cout << "projector: off\n"; }
};

class StreamingPlayer {
public:
    void on() { std::cout << "streaming player: on\n"; }
    void play(const std::string& title) { std::cout << "streaming player: playing \"" << title << "\"\n"; }
    void off() { std::cout << "streaming player: off\n"; }
};

// Facade: a single, simplified entry point coordinating the subsystem classes.
class HomeTheaterFacade {
public:
    HomeTheaterFacade(Amplifier& amp, Projector& projector, StreamingPlayer& player)
        : amp_(amp), projector_(projector), player_(player) {}

    void watchMovie(const std::string& title) {
        std::cout << "-- starting movie night --\n";
        amp_.on();
        amp_.setVolume(7);
        projector_.on();
        projector_.setInput("streaming");
        player_.on();
        player_.play(title);
    }

    void endMovie() {
        std::cout << "-- shutting down --\n";
        player_.off();
        projector_.off();
        amp_.off();
    }

private:
    Amplifier& amp_;
    Projector& projector_;
    StreamingPlayer& player_;
};

int main() {
    std::cout << "Facade pattern example:\n";

    Amplifier amp;
    Projector projector;
    StreamingPlayer player;
    HomeTheaterFacade homeTheater(amp, projector, player);

    homeTheater.watchMovie("The Matrix");
    homeTheater.endMovie();

    return 0;
}