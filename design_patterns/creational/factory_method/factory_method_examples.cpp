// Factory Method pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/creational/factory_method/factory_method_examples.cpp -o bin/factory_method_example && ./bin/factory_method_example

#include <iostream>
#include <memory>
#include <string>

// Description:
// Factory Method defines an interface for creating an object, but lets subclasses
// (or overridable creator functions) decide which concrete class to instantiate.
// The client works only against the abstract product and abstract creator.
//
// Use Factory Method when:
// - a class can't anticipate which concrete subclass of an object it needs to create;
// - you want to defer instantiation logic to subclasses or configuration;
// - creation involves setup steps that should be centralized and reused.
//
// Avoid Factory Method when:
// - there's only ever one concrete product and no plausible variation;
// - a simple constructor call or a factory function without inheritance is enough;
// - the extra class hierarchy adds indirection without real polymorphic benefit.
//
// Tips:
// - Keep the product interface minimal and focused on what clients actually need.
// - Prefer returning std::unique_ptr from factory methods for clear ownership.
// - Combine with a registry/map of creators when the set of products is data-driven.

class Notifier {
public:
    virtual ~Notifier() = default;
    virtual void send(const std::string& message) = 0;
};

class EmailNotifier final : public Notifier {
public:
    void send(const std::string& message) override {
        std::cout << "email: " << message << "\n";
    }
};

class SmsNotifier final : public Notifier {
public:
    void send(const std::string& message) override {
        std::cout << "sms: " << message << "\n";
    }
};

// Creator declares the factory method; subclasses decide the concrete product.
class NotifierCreator {
public:
    virtual ~NotifierCreator() = default;

    void notify(const std::string& message) {
        std::unique_ptr<Notifier> notifier = createNotifier();
        notifier->send(message);
    }

protected:
    virtual std::unique_ptr<Notifier> createNotifier() = 0;
};

class EmailNotifierCreator final : public NotifierCreator {
protected:
    std::unique_ptr<Notifier> createNotifier() override {
        return std::make_unique<EmailNotifier>();
    }
};

class SmsNotifierCreator final : public NotifierCreator {
protected:
    std::unique_ptr<Notifier> createNotifier() override {
        return std::make_unique<SmsNotifier>();
    }
};

void alertUser(NotifierCreator& creator) {
    creator.notify("your order has shipped");
}

int main() {
    std::cout << "Factory Method pattern example:\n";

    EmailNotifierCreator emailCreator;
    SmsNotifierCreator smsCreator;

    alertUser(emailCreator);
    alertUser(smsCreator);

    return 0;
}