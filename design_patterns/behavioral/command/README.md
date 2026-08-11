# Command

## Description

Command is a behavioral design pattern that encapsulates a request as an object.
Instead of calling a method directly, you wrap "an action to perform, on a specific
target, with specific arguments" into a standalone `Command` object. This lets you
parameterize other objects with different requests, queue or log requests, and
support undoable operations — all without the caller needing to know what a command
actually does or what it acts on.

The pattern separates three concerns that a direct method call blends together:

- **What to do** — the `Command` itself.
- **Who does it** — the `Receiver`, the object that holds the real logic/state.
- **When to trigger it** — the `Invoker`, which holds and fires commands without
  knowing their concrete type or receiver.

Typical participants:

- **Command** — declares the interface for executing (and, if needed, undoing) a
  request (e.g. `execute()`, `undo()`).
- **ConcreteCommand** — binds a specific receiver and any parameters/state needed to
  perform (and reverse) the action (e.g. `InsertTextCommand`).
- **Receiver** — the object that contains the actual business logic the command
  invokes (e.g. `TextDocument`).
- **Invoker** — triggers commands and optionally keeps a history of them, without
  knowing what each command does (e.g. `CommandHistory`).

## When to Use

- You want to parameterize objects with an action to perform — pass an operation
  around as data (store it, queue it, hand it to another object) rather than calling
  it directly at the point of decision.
- You need to queue, log, delay, batch, or replay requests independently of whatever
  triggered them — e.g. a task queue, a macro recorder, or a request log for
  auditing/replay.
- You need undo/redo functionality. Since each command is a self-contained object,
  it can capture exactly what's needed to reverse its own effect, and an invoker can
  keep a history stack to walk backward (and forward) through executed commands.
- You want to decouple UI elements (buttons, menu items, shortcuts) from the code
  that handles them — each just holds a `Command` and calls `execute()`, without
  knowing which receiver or logic is behind it.

## When to Avoid

- The action is a single, simple call with no need to queue, log, delay, undo, or
  swap it out — calling the method directly is clearer and has less ceremony.
- The overhead of a command class per action outweighs the flexibility you'd gain —
  e.g. a handful of trivial one-off operations that will never need queuing, undo, or
  substitution.
- Undo/redo, logging, and queuing genuinely aren't needed now and aren't likely to be
  needed later — introducing the extra Command/Invoker/Receiver split preemptively
  just adds indirection.

## How to Implement

1. **Define the Receiver.** The object that holds the actual logic/state the command
   will act on.

   ```cpp
   class TextDocument {
   public:
       void insert(const std::string& text) { content_ += text; }
       void removeLast(size_t count) { content_.erase(content_.size() - count); }

   private:
       std::string content_;
   };
   ```

2. **Define the Command interface.** Usually `execute()`, plus `undo()` if undo
   support is required.

   ```cpp
   class Command {
   public:
       virtual ~Command() = default;
       virtual void execute() = 0;
       virtual void undo() = 0;
   };
   ```

3. **Implement Concrete Commands.** Each binds a specific receiver and captures the
   parameters (and any state) needed to perform — and reverse — the action.

   ```cpp
   class InsertTextCommand final : public Command {
   public:
       InsertTextCommand(TextDocument& document, std::string text)
           : document_(document), text_(std::move(text)) {}

       void execute() override { document_.insert(text_); }
       void undo() override { document_.removeLast(text_.size()); }

   private:
       TextDocument& document_;
       std::string text_;
   };
   ```

4. **Implement the Invoker.** Triggers commands and, if undo is needed, keeps a
   history of executed commands.

   ```cpp
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
   ```

5. **Wire it together at the call site.** The invoker never references
   `InsertTextCommand` or `TextDocument` directly.

   ```cpp
   TextDocument document;
   CommandHistory history;

   history.execute(std::make_unique<InsertTextCommand>(document, "Hello"));
   history.execute(std::make_unique<InsertTextCommand>(document, ", world"));

   history.undoLast();   // reverses the last executed command
   ```

## Implementation Tips

- Keep the receiver (the object doing the real work) separate from the command
  (which just knows how and when to invoke it) — this lets receivers and commands
  vary independently, and lets the same receiver be driven by many different
  commands.
- If undo is required, make sure each command stores everything it needs to reverse
  its own effect — don't rely on the receiver to "remember" prior state for you.
- An invoker's history (a stack or vector of executed commands) is what makes
  undo/redo possible — walking backward through it undoes, walking forward again
  (if you keep undone commands rather than discarding them) redoes.
- For commands with no natural undo (e.g. sending an email), it's fine to leave
  `undo()` a no-op or omit it from the interface entirely if undo truly isn't a
  requirement for any command in the system.
- Consider a `CompositeCommand` (a command that holds and executes a list of other
  commands) when you need to treat a batch of actions as a single undoable unit.