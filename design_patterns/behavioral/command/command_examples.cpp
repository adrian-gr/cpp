// Command pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/command/command_examples.cpp -o bin/command_example && ./bin/command_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Command encapsulates a request as an object, letting you parameterize clients
// with different requests, queue or log requests, and support undoable operations.
// The invoker holds and triggers commands without knowing what they actually do or
// which receiver they act on.
//
// Use Command when:
// - you want to parameterize objects with an action to perform — pass an operation
//   around like data, rather than calling it directly;
// - you need to queue, log, delay, or replay requests independently of the sender;
// - you need undo/redo, since each command can capture what's needed to reverse it.
//
// Avoid Command when:
// - the action is a single, simple call with no need to queue, log, delay, undo, or
//   swap it out — calling the method directly is clearer;
// - the overhead of a command class per action outweighs the flexibility gained,
//   e.g. a handful of trivial one-off operations that will never change;
// - undo/redo or queuing genuinely aren't needed and aren't likely to be needed.
//
// Tips:
// - Keep the receiver (the object that does the real work) separate from the
//   command (which just knows how to invoke it) so both can vary independently.
// - Store enough state in each command to support undo, if undo is required.
// - An invoker's history (a stack/vector of executed commands) is what enables undo/redo.

class TextDocument {
public:
    void insert(const std::string& text) {
        content_ += text;
    }

    void removeLast(size_t count) {
        content_.erase(content_.size() - count);
    }

    void show() const {
        std::cout << "document: \"" << content_ << "\"\n";
    }

private:
    std::string content_;
};

class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class InsertTextCommand final : public Command {
public:
    InsertTextCommand(TextDocument& document, std::string text)
        : document_(document), text_(std::move(text)) {}

    void execute() override {
        document_.insert(text_);
    }

    void undo() override {
        document_.removeLast(text_.size());
    }

private:
    TextDocument& document_;
    std::string text_;
};

// Invoker: triggers commands and keeps a history for undo, without knowing what
// each command actually does or which receiver it acts on.
class CommandHistory {
public:
    void execute(std::unique_ptr<Command> command) {
        command->execute();
        history_.push_back(std::move(command));
    }

    void undoLast() {
        if (!history_.empty()) {
            history_.back()->undo();
            history_.pop_back();
        }
    }

private:
    std::vector<std::unique_ptr<Command>> history_;
};

int main() {
    std::cout << "Command pattern example:\n";

    TextDocument document;
    CommandHistory history;

    history.execute(std::make_unique<InsertTextCommand>(document, "Hello"));
    document.show();

    history.execute(std::make_unique<InsertTextCommand>(document, ", world"));
    document.show();

    history.undoLast();
    document.show();

    return 0;
}