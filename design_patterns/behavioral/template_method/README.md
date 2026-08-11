# Template Method

## Description

Template Method is a behavioral design pattern that defines the skeleton of an
algorithm in a base class method, deferring some of its steps to subclasses.
Subclasses override specific steps of the algorithm without changing its overall
structure — the sequence of steps stays fixed in the base class, and only the
individual step implementations vary.

This is inheritance-based reuse: the shared algorithm shape lives once, in the
base class; each subclass fills in only the parts that legitimately differ.

Typical participants:

- **AbstractClass** — defines the template method (the fixed algorithm skeleton)
  and declares the steps that make it up — some abstract (must be overridden), some
  hooks (optional, with a sensible default), some fully fixed and private (e.g.
  `DataExporter`).
- **ConcreteClass** — implements the abstract steps, and optionally overrides hook
  steps, without touching the template method itself (e.g. `CsvExporter`,
  `JsonExporter`).

## When to Use

- Several classes implement algorithms that are the same at a high level but differ
  in specific steps, and you want to avoid duplicating the shared skeleton across
  every implementation.
- You want to control an algorithm's overall structure in exactly one place,
  letting subclasses customize only the parts that should legitimately vary — this
  prevents subclasses from accidentally reordering or skipping steps that must stay
  fixed.
- You want to enforce a fixed sequence of steps while still allowing some steps to
  be optional — hook methods with a default no-op (or sensible default) body let
  subclasses opt in to customizing a step only when they need to.
- You're formalizing a process that's already conceptually fixed-shape: data export
  pipelines, request-handling lifecycles (validate → process → respond), test
  setup/teardown scaffolding.

## When to Avoid

- The algorithm's *structure* itself needs to vary, not just individual steps —
  Template Method fixes the skeleton by design, so if the sequence of steps itself
  needs to change, a composition-based pattern like Strategy fits better.
- There's only one implementation of the algorithm and no other variant is
  anticipated — a single concrete class is simpler than splitting into a base class
  and one subclass.
- Relying on inheritance to customize behavior is undesirable — for example, if you
  need runtime-swappable behavior (choosing a variant at runtime rather than at
  compile time via subclassing), Strategy achieves similar reuse through
  composition instead, which is more flexible for that case.

## How to Implement

1. **Define the AbstractClass with the template method.** The template method
   itself should be non-virtual (or `final`) so subclasses can't override the
   algorithm's structure — only the individual steps.

   ```cpp
   class DataExporter {
   public:
       virtual ~DataExporter() = default;

       void exportData() {
           openTarget();
           writeHeader();
           writeBody();
           writeFooter();
           closeTarget();
       }

   protected:
       virtual void writeHeader() = 0;
       virtual void writeBody() = 0;
       virtual void writeFooter() { std::cout << "(no footer)\n"; }  // hook

   private:
       void openTarget() { std::cout << "opening target\n"; }
       void closeTarget() { std::cout << "closing target\n"; }
   };
   ```

2. **Implement Concrete Classes.** Override the abstract steps every subclass must
   define, and optionally override hook steps where a variant needs one.

   ```cpp
   class CsvExporter final : public DataExporter {
   protected:
       void writeHeader() override { std::cout << "id,name,price\n"; }
       void writeBody() override { std::cout << "1,widget,9.99\n"; }
       // writeFooter() left as the default hook implementation.
   };

   class JsonExporter final : public DataExporter {
   protected:
       void writeHeader() override { std::cout << "{\n  \"items\": [\n"; }
       void writeBody() override { std::cout << "    {...}\n"; }
       void writeFooter() override { std::cout << "  ]\n}\n"; }
   };
   ```

3. **Call the template method from client code.** The caller never touches the
   individual steps directly.

   ```cpp
   CsvExporter csv;
   csv.exportData();   // fixed sequence, CSV-specific steps

   JsonExporter json;
   json.exportData();  // same fixed sequence, JSON-specific steps
   ```

## Implementation Tips

- Mark the template method non-virtual (or `final` if it must remain virtual for
  other reasons) so subclasses genuinely cannot override the algorithm's structure
  — only its individual steps.
- Use hook methods (virtual methods with a default, often no-op, body) for
  genuinely optional steps, and pure virtual methods for steps every subclass must
  define — this makes the "must override" vs. "may override" distinction explicit
  in the interface itself.
- Keep fully fixed steps (like `openTarget()`/`closeTarget()` above) private and
  non-virtual — if a step should never vary, don't expose it as overridable at all.
- Keep the template method's step count small and readable. A sprawling skeleton
  with a dozen steps defeats the purpose of factoring it out — split it into
  multiple smaller template methods if it grows too large.
- If you find yourself wanting to swap the whole algorithm shape at runtime rather
  than just its steps, that's a signal to reach for Strategy (composition) instead
  of Template Method (inheritance).