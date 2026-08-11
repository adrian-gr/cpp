// Mediator pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/mediator/mediator_examples.cpp -o bin/mediator_example && ./bin/mediator_example

#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Description:
// Mediator defines an object that encapsulates how a set of other objects interact.
// Instead of colleagues referencing and calling each other directly (a tangled
// many-to-many web of dependencies), each colleague talks only to the mediator,
// which coordinates the interaction between them.
//
// Use Mediator when:
// - a set of objects communicate in complex, tangled ways and you want to centralize
//   that interaction logic in one place instead of scattering it across colleagues;
// - reusing a colleague class is hard because it's tightly coupled to many others;
// - you want to change how objects interact without modifying the objects themselves.
//
// Avoid Mediator when:
// - only two objects interact, or interactions are simple — direct calls are clearer;
// - the mediator itself risks becoming a bloated "god object" that knows too much
//   about every colleague's internal behavior, not just their coordination;
// - the objects' communication pattern is stable and unlikely to need central control.
//
// Tips:
// - Keep colleagues unaware of each other; they should only know the mediator interface.
// - Give the mediator a narrow, coordination-only responsibility — no business logic
//   that belongs inside a colleague.
// - Consider an event/notify style interface so colleagues can be added without
//   changing the mediator's public surface each time.

class ChatMediator {
public:
    virtual ~ChatMediator() = default;
    virtual void sendMessage(const std::string& message, class User* sender) = 0;
};

class User {
public:
    User(ChatMediator& mediator, std::string name) : mediator_(mediator), name_(std::move(name)) {}
    virtual ~User() = default;

    const std::string& name() const { return name_; }

    void send(const std::string& message) {
        mediator_.sendMessage(message, this);
    }

    virtual void receive(const std::string& sender, const std::string& message) {
        std::cout << name_ << " received from " << sender << ": " << message << "\n";
    }

private:
    ChatMediator& mediator_;
    std::string name_;
};

// Concrete mediator: knows all participants and routes messages between them.
class ChatRoom final : public ChatMediator {
public:
    void addUser(User* user) {
        users_.push_back(user);
    }

    void sendMessage(const std::string& message, User* sender) override {
        for (User* user : users_) {
            if (user != sender) {
                user->receive(sender->name(), message);
            }
        }
    }

private:
    std::vector<User*> users_;
};

int main() {
    std::cout << "Mediator pattern example:\n";

    ChatRoom chatRoom;

    User alice(chatRoom, "Alice");
    User bob(chatRoom, "Bob");
    User carol(chatRoom, "Carol");

    chatRoom.addUser(&alice);
    chatRoom.addUser(&bob);
    chatRoom.addUser(&carol);

    alice.send("Hey everyone!");
    bob.send("Hi Alice!");

    return 0;
}