// Observer pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/observer/observer_examples.cpp -o bin/observer_example && ./bin/observer_example

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

// Description:
// Observer defines a one-to-many dependency between objects so that when one object
// (the subject) changes state, all its dependents (observers) are notified and
// updated automatically. The subject knows only that it has observers implementing
// a common interface — never their concrete types.
//
// Use Observer when:
// - a change to one object must automatically propagate to an open-ended set of
//   other objects, without the subject knowing their concrete types in advance;
// - the set of dependents can change at runtime (subscribe/unsubscribe);
// - you want to decouple the object that owns state from the objects that react
//   to changes in that state.
//
// Avoid Observer when:
// - there's a small, fixed set of dependents that never changes — direct calls are
//   simpler and easier to trace than an indirection layer;
// - notification order or delivery guarantees matter a lot and a simple broadcast
//   can't express them (consider an explicit event bus/queue instead);
// - update chains can cascade into other notifications, risking hard-to-debug
//   feedback loops if not carefully controlled.
//
// Tips:
// - Keep the observer interface minimal — usually just one update-style method.
// - Let observers control their own subscribe/unsubscribe lifetime where practical.
// - Consider passing only the changed data (not the whole subject) to keep coupling low.

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onTemperatureChanged(double celsius) = 0;
};

class DisplayPanel final : public Observer {
public:
    explicit DisplayPanel(std::string label) : label_(std::move(label)) {}

    void onTemperatureChanged(double celsius) override {
        std::cout << label_ << " display: " << celsius << "C\n";
    }

private:
    std::string label_;
};

class AlarmSystem final : public Observer {
public:
    void onTemperatureChanged(double celsius) override {
        if (celsius >= threshold_) {
            std::cout << "ALARM: temperature " << celsius << "C exceeds threshold\n";
        }
    }

private:
    double threshold_ = 40.0;
};

// Subject: owns the state and the list of subscribers, but knows nothing about
// concrete observer types.
class TemperatureSensor {
public:
    void subscribe(Observer* observer) {
        observers_.push_back(observer);
    }

    void unsubscribe(Observer* observer) {
        observers_.erase(std::remove(observers_.begin(), observers_.end(), observer), observers_.end());
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

int main() {
    std::cout << "Observer pattern example:\n";

    TemperatureSensor sensor;

    DisplayPanel kitchenDisplay("kitchen");
    DisplayPanel labDisplay("lab");
    AlarmSystem alarm;

    sensor.subscribe(&kitchenDisplay);
    sensor.subscribe(&labDisplay);
    sensor.subscribe(&alarm);

    sensor.setTemperature(22.5);
    sensor.setTemperature(41.0);

    sensor.unsubscribe(&labDisplay);
    sensor.setTemperature(23.0);

    return 0;
}