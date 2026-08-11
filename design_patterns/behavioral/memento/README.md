# Memento

## Description

Memento is a behavioral design pattern that lets you capture and externalize an
object's internal state so it can be restored later, without violating
encapsulation. The object whose state is being saved — the **originator** — is the
only one that creates and reads its own mementos. Outside code — the **caretaker** —
stores mementos and manages their lifetime, but never inspects or modifies what's
inside them.

This gives you snapshot/restore behavior (undo, checkpoints, save points) without
exposing an object's private state through a public interface just so some other
class can save and restore it.

Typical participants:

- **Originator** — the object whose state is being saved; creates a memento
  capturing its current state, and can restore itself from one (e.g. `TextEditor`).
- **Memento** — an opaque snapshot of the originator's state; only the originator
  can read or write its contents, typically enforced via a `friend` relationship or
  a narrow interface (e.g. `EditorMemento`).
- **Caretaker** — holds and manages mementos (often as a history/stack), but never
  reads or writes their contents directly (e.g. `History`).

## When to Use

- You need undo or rollback of an object's internal state, but exposing that state
  publicly (just so external code can save and restore it) would break
  encapsulation.
- You want to snapshot an object at a point in time and restore it later, without
  the code holding the snapshot needing to know the object's internal
  representation.
- You want checkpoint/restore behavior — save points in a game, undo history in an
  editor, transaction rollback for an in-memory object's state.
- You want a clean separation between *who owns state* (the originator) and *who
  manages history* (the caretaker), so history management doesn't leak into the
  originator's own logic.

## When to Avoid

- The object's state is small and already safely exposed via a public, copyable
  struct — a plain copy of that struct is simpler than introducing a dedicated
  memento abstraction and friend relationships.
- Snapshots would be large or frequent enough that the memory/performance cost of
  storing many full-state copies becomes a real concern — consider storing deltas
  (incremental changes) instead of full snapshots in that case.
- Undo/rollback genuinely isn't a requirement for the object — don't add the
  mechanism speculatively just because "it might be nice to have."

## How to Implement

1. **Define the Memento.** Holds a snapshot of state, but keeps that state
   inaccessible to everyone except the originator (via a `friend` declaration or a
   deliberately narrow public interface).

   ```cpp
   class EditorMemento {
   public:
       explicit EditorMemento(std::string content) : content_(std::move(content)) {}

   private:
       friend class TextEditor;
       std::string content_;
   };
   ```

2. **Implement the Originator.** Creates mementos of its own state, and can restore
   itself from one.

   ```cpp
   class TextEditor {
   public:
       void type(const std::string& text) { content_ += text; }
       const std::string& content() const { return content_; }

       std::unique_ptr<EditorMemento> save() const {
           return std::make_unique<EditorMemento>(content_);
       }

       void restore(const EditorMemento& memento) {
           content_ = memento.content_;
       }

   private:
       std::string content_;
   };
   ```

3. **Implement the Caretaker.** Stores and manages mementos — typically as a
   history stack — without reading or writing their contents.

   ```cpp
   class History {
   public:
       void push(std::unique_ptr<EditorMemento> memento) {
           mementos_.push_back(std::move(memento));
       }

       std::unique_ptr<EditorMemento> pop() {
           if (mementos_.empty()) return nullptr;
           auto memento = std::move(mementos_.back());
           mementos_.pop_back();
           return memento;
       }

   private:
       std::vector<std::unique_ptr<EditorMemento>> mementos_;
   };
   ```

4. **Save checkpoints and restore as needed.**

   ```cpp
   TextEditor editor;
   History history;

   editor.type("Hello");
   history.push(editor.save());

   editor.type(", world");
   history.push(editor.save());

   editor.type("!!! (typo)");

   history.pop();                        // discard the "typo" state (never saved)
   if (auto last = history.pop()) {
       editor.restore(*last);            // back to "Hello"
   }
   ```

## Implementation Tips

- Keep the memento's contents opaque to everyone but its originator — a `friend`
  declaration (as above) is a clean way to grant exactly one class private access
  without opening up a public getter that anyone could use.
- Let the caretaker manage memento *lifetime and ordering* (a stack, a list with
  timestamps, etc.), but never let it inspect or manipulate memento *contents* —
  that responsibility belongs solely to the originator.
- For large or frequently-changing state, consider incremental/delta mementos
  (storing just what changed since the last snapshot) instead of full-state copies,
  to keep memory and copy cost bounded.
- If multiple originator types need memento support, consider a common
  `Memento`/`Originator` interface pair — but keep each concrete memento's actual
  fields private to its specific originator; a shared *interface* doesn't mean
  shared *visibility* into contents.
- Memento pairs naturally with Command for undo systems: a command can capture a
  memento before acting, and use it to implement `undo()` without needing to know
  how to reverse the action step by step.