// Memento pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/memento/memento_examples.cpp -o bin/memento_example && ./bin/memento_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Memento lets you capture and externalize an object's internal state so it can be
// restored later, without violating encapsulation. The object being saved (the
// originator) creates and reads its own mementos; outside code (the caretaker)
// stores mementos but never inspects or modifies their contents.
//
// Use Memento when:
// - you need undo/rollback of an object's internal state, but exposing that state
//   publicly would break encapsulation;
// - you want to snapshot an object at a point in time and restore it later, without
//   the snapshot-holding code knowing the object's internal representation;
// - you want checkpoint/restore behavior (e.g. save points in a game, editor undo).
//
// Avoid Memento when:
// - the object's state is small and already safely exposed via a public copyable
//   struct — a plain copy is simpler than a dedicated memento abstraction;
// - snapshots would be large or frequent enough that memory/performance from storing
//   many full-state copies becomes a real concern (consider deltas instead);
// - undo/rollback genuinely isn't needed — don't add the mechanism speculatively.
//
// Tips:
// - Keep the memento's contents opaque to everyone except its originator.
// - Let the caretaker manage memento lifetime/history (a stack), but never its content.
// - Consider incremental/delta mementos instead of full snapshots for large state.

class EditorMemento {
public:
    explicit EditorMemento(std::string content) : content_(std::move(content)) {}

private:
    friend class TextEditor;
    std::string content_;
};

// Originator: creates mementos of its own state and can restore from one.
class TextEditor {
public:
    void type(const std::string& text) {
        content_ += text;
    }

    const std::string& content() const {
        return content_;
    }

    std::unique_ptr<EditorMemento> save() const {
        return std::make_unique<EditorMemento>(content_);
    }

    void restore(const EditorMemento& memento) {
        content_ = memento.content_;
    }

private:
    std::string content_;
};

// Caretaker: stores mementos and manages history, without reading their contents.
class History {
public:
    void push(std::unique_ptr<EditorMemento> memento) {
        mementos_.push_back(std::move(memento));
    }

    std::unique_ptr<EditorMemento> pop() {
        if (mementos_.empty()) {
            return nullptr;
        }
        auto memento = std::move(mementos_.back());
        mementos_.pop_back();
        return memento;
    }

private:
    std::vector<std::unique_ptr<EditorMemento>> mementos_;
};

int main() {
    std::cout << "Memento pattern example:\n";

    TextEditor editor;
    History history;

    editor.type("Hello");
    history.push(editor.save());

    editor.type(", world");
    history.push(editor.save());

    editor.type("!!! (typo)");
    std::cout << "current: " << editor.content() << "\n";

    history.pop();
    if (auto last = history.pop()) {
        editor.restore(*last);
    }
    std::cout << "after undo: " << editor.content() << "\n";

    return 0;
}