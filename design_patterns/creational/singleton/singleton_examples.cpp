// Singleton pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/creational/singleton/singleton_examples.cpp -o bin/singleton_example && ./bin/singleton_example

#include <iostream>
#include <mutex>
#include <string>

// Description:
// Singleton ensures a class has only one instance and provides a single, well-known
// global access point to it. The class controls its own instantiation, typically by
// making constructors private and exposing a static accessor that creates the
// instance on first use.
//
// Use Singleton when:
// - exactly one instance of a class must exist for the lifetime of the program
//   (e.g. a logger, a configuration store, a hardware resource manager);
// - that single instance must be reachable from many unrelated parts of the code
//   without threading a reference through every constructor;
// - controlled, lazy initialization of an expensive shared resource is required.
//
// Avoid Singleton when:
// - the "single instance" requirement is really just convenience, not a real
//   invariant — an ordinary object passed around (or injected) is more testable;
// - it would introduce hidden global state that makes unit tests order-dependent
//   or hard to isolate;
// - multiple configurations/instances might legitimately be needed later — Singleton
//   makes that change painful since callers depend on a single global access point.
//
// Tips:
// - Use a function-local static (Meyers' Singleton) for thread-safe lazy init in C++11+.
// - Delete the copy constructor and copy assignment operator explicitly.
// - Keep the singleton's responsibilities narrow; a singleton that does too much
//   becomes a hidden god-object.

class Logger {
public:
    static Logger& instance() {
        static Logger instance_;   // Meyers' Singleton: thread-safe, lazily initialized.
        return instance_;
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "[log] " << message << "\n";
    }

private:
    Logger() = default;
    std::mutex mutex_;
};

void doWork() {
    Logger::instance().log("doWork started");
}

int main() {
    std::cout << "Singleton pattern example:\n";

    Logger::instance().log("application starting");
    doWork();
    Logger::instance().log("application finished");

    return 0;
}