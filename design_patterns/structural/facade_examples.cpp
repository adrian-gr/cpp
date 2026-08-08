// Facade pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/facade_examples.cpp -o bin/facade_example && ./bin/facade_example

#include <iostream>
#include <string>

// Description:
// Facade provides a simple interface to a larger subsystem of classes.
// It hides coordination details while keeping the subsystem available for
// clients that need more control.
//
// Use Facade when:
// - clients repeatedly coordinate several subsystem objects;
// - the subsystem is complex or likely to change internally;
// - you want a clear entry point for common workflows.
//
// Avoid Facade when:
// - clients need most subsystem operations directly;
// - the facade would become a second, overly broad subsystem API;
// - a simple function or constructor would be enough.
//
// Tips:
// - Keep facade methods aligned with useful client workflows.
// - Let subsystem classes own their detailed behavior.
// - Prefer composition so the facade can coordinate existing subsystem objects.

class Amplifier final {
public:
    void on() const {
        std::cout << "amplifier on\n";
    }

    void setVolume(int volume) const {
        std::cout << "amplifier volume: " << volume << "\n";
    }

    void off() const {
        std::cout << "amplifier off\n";
    }
};

class Projector final {
public:
    void on() const {
        std::cout << "projector on\n";
    }

    void wideScreenMode() const {
        std::cout << "projector widescreen mode\n";
    }

    void off() const {
        std::cout << "projector off\n";
    }
};

class StreamingPlayer final {
public:
    void on() const {
        std::cout << "streaming player on\n";
    }

    void play(const std::string& movie) const {
        std::cout << "streaming player playing: " << movie << "\n";
    }

    void stop() const {
        std::cout << "streaming player stopped\n";
    }

    void off() const {
        std::cout << "streaming player off\n";
    }
};

class HomeTheaterFacade final {
public:
    HomeTheaterFacade(Amplifier& amplifier,
                      Projector& projector,
                      StreamingPlayer& player)
        : amplifier_(amplifier), projector_(projector), player_(player) {}

    void watchMovie(const std::string& movie) const {
        std::cout << "Get ready to watch a movie...\n";
        amplifier_.on();
        amplifier_.setVolume(5);
        projector_.on();
        projector_.wideScreenMode();
        player_.on();
        player_.play(movie);
    }

    void endMovie() const {
        std::cout << "Shutting movie theater down...\n";
        player_.stop();
        player_.off();
        projector_.off();
        amplifier_.off();
    }

private:
    Amplifier& amplifier_;
    Projector& projector_;
    StreamingPlayer& player_;
};

int main() {
    std::cout << "Facade pattern example:\n";

    Amplifier amplifier;
    Projector projector;
    StreamingPlayer player;
    HomeTheaterFacade homeTheater(amplifier, projector, player);

    homeTheater.watchMovie("The Matrix");
    homeTheater.endMovie();

    return 0;
}
