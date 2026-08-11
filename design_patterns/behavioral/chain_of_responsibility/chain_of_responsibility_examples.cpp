// Chain of Responsibility pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/chain_of_responsibility/chain_of_responsibility_examples.cpp -o bin/chain_of_responsibility_example && ./bin/chain_of_responsibility_example

#include <iostream>
#include <memory>
#include <string>

// Description:
// Chain of Responsibility passes a request along a chain of handlers, where each
// handler decides either to process the request or to pass it to the next handler
// in the chain. The sender doesn't need to know which handler will ultimately
// handle the request, or how many handlers exist.
//
// Use Chain of Responsibility when:
// - more than one object may handle a request, and the handler isn't known in
//   advance — it should be determined at runtime by trying handlers in sequence;
// - you want to issue a request without specifying its receiver explicitly;
// - the set of handlers, and their order, should be configurable independently
//   of the code that issues requests.
//
// Avoid Chain of Responsibility when:
// - exactly one handler will always process a given request — a direct call is
//   clearer than a chain that only ever stops at one fixed link;
// - it's important that requests are guaranteed to be handled — a chain can let a
//   request silently fall through with no handler claiming it, unless you add an
//   explicit fallback;
// - a long chain becomes hard to debug because there's no single place that shows
//   the full decision logic for a request.
//
// Tips:
// - Give each handler a single, narrow responsibility rather than broad conditionals.
// - Always provide a way to terminate the chain (a default/fallback handler, or a
//   sentinel of "no next handler") so requests don't vanish silently.
// - Keep chain construction (wiring handlers together) separate from handler logic.

enum class LogLevel { Info, Warning, Error };

class LogHandler {
public:
    virtual ~LogHandler() = default;

    void setNext(std::shared_ptr<LogHandler> next) {
        next_ = std::move(next);
    }

    void handle(LogLevel level, const std::string& message) {
        if (canHandle(level)) {
            process(message);
        } else if (next_) {
            next_->handle(level, message);
        }
    }

protected:
    virtual bool canHandle(LogLevel level) const = 0;
    virtual void process(const std::string& message) const = 0;

private:
    std::shared_ptr<LogHandler> next_;
};

class ConsoleHandler final : public LogHandler {
protected:
    bool canHandle(LogLevel level) const override {
        return level == LogLevel::Info;
    }
    void process(const std::string& message) const override {
        std::cout << "console: " << message << "\n";
    }
};

class EmailHandler final : public LogHandler {
protected:
    bool canHandle(LogLevel level) const override {
        return level == LogLevel::Warning;
    }
    void process(const std::string& message) const override {
        std::cout << "email alert: " << message << "\n";
    }
};

class PagerHandler final : public LogHandler {
protected:
    bool canHandle(LogLevel level) const override {
        return level == LogLevel::Error;
    }
    void process(const std::string& message) const override {
        std::cout << "page on-call: " << message << "\n";
    }
};

int main() {
    std::cout << "Chain of Responsibility pattern example:\n";

    auto console = std::make_shared<ConsoleHandler>();
    auto email = std::make_shared<EmailHandler>();
    auto pager = std::make_shared<PagerHandler>();

    // Wire the chain: console -> email -> pager.
    console->setNext(email);
    email->setNext(pager);

    console->handle(LogLevel::Info, "server started");
    console->handle(LogLevel::Warning, "disk usage above 80%");
    console->handle(LogLevel::Error, "database connection lost");

    return 0;
}