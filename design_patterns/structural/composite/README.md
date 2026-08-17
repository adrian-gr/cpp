# Composite

## Description

Composite is a structural design pattern that lets you compose objects into tree
structures, and then treat individual objects and compositions of objects
uniformly through a single shared interface. A client calling an operation on a
single leaf node uses the exact same interface as calling it on an entire subtree
— it never needs to special-case "is this one item, or a group of items?"

The key mechanism is that a composite (a container node) holds children of the
*same* interface type it implements, which lets containers hold other containers,
building an arbitrarily deep tree without the client's code needing to change.

Typical participants:

- **Component** — the shared interface for both leaves and composites, declaring
  operations that make sense for both (e.g. `FileSystemComponent`).
- **Leaf** — a component with no children, implementing the operations directly
  (e.g. `File`).
- **Composite** — a component that holds children (of the Component type) and
  implements operations by delegating to and aggregating over those children (e.g.
  `Directory`).

## When to Use

- You need to represent part-whole hierarchies of objects — a natural tree
  structure like a file system, a UI widget hierarchy, an org chart, a menu with
  submenus, or a document with nested sections.
- Client code should treat individual objects and compositions of objects
  uniformly, without needing to distinguish between "a single item" and "a group of
  items" at the call site — both should support the same operations.
- Operations need to apply recursively across an entire structure, and it's
  simpler to let each composite handle recursing into its own children than to have
  the client manage that traversal itself.

## When to Avoid

- The structure isn't naturally hierarchical — forcing a tree shape onto
  fundamentally flat data just to use this pattern adds complexity without a real
  benefit.
- Leaves and composites need such different interfaces that a single shared
  interface would end up with awkward or meaningless methods on one side (e.g.
  `add()`/`remove()` being nonsensical on a leaf, forcing empty or throwing
  implementations).
- The tree is small, fixed, and simple enough that direct, explicit handling of
  each case is clearer than the uniform-interface abstraction — not every nested
  structure needs Composite.

## How to Implement

1. **Define the Component interface.** Operations that make sense for both leaves
   and composites.

   ```cpp
   class FileSystemComponent {
   public:
       virtual ~FileSystemComponent() = default;
       virtual void display(int indent = 0) const = 0;
       virtual long size() const = 0;
   };
   ```

2. **Implement the Leaf.** Handles operations directly, with no children of its
   own.

   ```cpp
   class File final : public FileSystemComponent {
   public:
       File(std::string name, long sizeInBytes) : name_(std::move(name)), sizeInBytes_(sizeInBytes) {}

       void display(int indent = 0) const override {
           std::cout << std::string(indent, ' ') << "- " << name_ << "\n";
       }

       long size() const override { return sizeInBytes_; }

   private:
       std::string name_;
       long sizeInBytes_;
   };
   ```

3. **Implement the Composite.** Holds children of the *Component* type (not the
   concrete leaf type), and implements operations by delegating to and aggregating
   over those children.

   ```cpp
   class Directory final : public FileSystemComponent {
   public:
       explicit Directory(std::string name) : name_(std::move(name)) {}

       void add(std::shared_ptr<FileSystemComponent> component) {
           children_.push_back(std::move(component));
       }

       void display(int indent = 0) const override {
           std::cout << std::string(indent, ' ') << "+ " << name_ << "/\n";
           for (const auto& child : children_) child->display(indent + 2);
       }

       long size() const override {
           long total = 0;
           for (const auto& child : children_) total += child->size();
           return total;
       }

   private:
       std::string name_;
       std::vector<std::shared_ptr<FileSystemComponent>> children_;
   };
   ```

4. **Build a tree and operate on it uniformly.** The client never checks whether
   it's holding a `File` or a `Directory`.

   ```cpp
   auto root = std::make_shared<Directory>("project");
   auto src = std::make_shared<Directory>("src");
   src->add(std::make_shared<File>("main.cpp", 1200));

   root->add(src);
   root->add(std::make_shared<File>("LICENSE", 150));

   root->display();                          // recurses through the whole tree
   std::cout << root->size() << " bytes\n";  // sums the whole tree in one call
   ```

## Implementation Tips

- Keep the Component interface focused on operations that genuinely make sense for
  both leaves and composites; avoid leaking container-only operations (like
  `add()`/`remove()`) into it if leaves can't meaningfully support them.
- Let composites delegate operations to their children recursively — this is what
  makes tree-wide operations (like `size()` summing an entire nested structure)
  work correctly with a single top-level call.
- Decide deliberately whether child-management methods (`add`/`remove`) belong on
  the shared Component interface or only on the Composite class. Putting them on
  Component gives more uniform traversal code but forces leaves to implement (or
  reject) operations that don't apply to them; keeping them Composite-only is more
  type-safe but means client code sometimes needs to know it's dealing with a
  composite specifically.
- Use `shared_ptr` (or another shared-ownership handle) for children when the same
  component might need to be referenced from multiple places, or when composite
  lifetime management benefits from reference counting rather than strict single
  ownership.
- Watch for cycles when building the tree (a directory accidentally added as its
  own descendant) — recursive operations like `size()` will loop forever if the
  structure isn't a genuine acyclic tree.