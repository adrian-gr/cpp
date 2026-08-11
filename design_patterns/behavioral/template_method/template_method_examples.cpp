// Template Method pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/behavioral/template_method/template_method_examples.cpp -o bin/template_method_example && ./bin/template_method_example

#include <iostream>
#include <string>

// Description:
// Template Method defines the skeleton of an algorithm in a base class method,
// deferring some steps to subclasses. Subclasses override specific steps without
// changing the algorithm's overall structure, which stays fixed in the base class.
//
// Use Template Method when:
// - several classes implement algorithms that are the same at a high level but
//   differ in specific steps, and you want to avoid duplicating the shared skeleton;
// - you want to control the algorithm's overall structure in one place, letting
//   subclasses customize only the parts that should legitimately vary;
// - you want to enforce a fixed sequence of steps while still allowing some steps
//   to be optional (via hook methods with default no-op behavior).
//
// Avoid Template Method when:
// - the algorithm's structure itself needs to vary, not just individual steps —
//   composition-based patterns like Strategy fit that need better;
// - there's only one implementation of the algorithm and no other variant is
//   anticipated — a single concrete class is simpler than a base/subclass split;
// - relying on inheritance to customize behavior is undesirable (e.g. you need
//   runtime-swappable behavior) — Strategy achieves similar reuse via composition.
//
// Tips:
// - Mark the template method itself non-virtual (or final) so subclasses can't
//   override the algorithm's structure, only its steps.
// - Use hook methods (virtual methods with a default no-op body) for genuinely
//   optional steps, and pure virtual methods for steps every subclass must define.
// - Keep the template method's step count small and readable; a sprawling skeleton
//   defeats the purpose of factoring it out.

class DataExporter {
public:
    virtual ~DataExporter() = default;

    // Template method: fixed skeleton, calls out to steps that subclasses define.
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

    // Hook: optional step with a sensible default; subclasses may override.
    virtual void writeFooter() {
        std::cout << "(no footer)\n";
    }

private:
    void openTarget() {
        std::cout << "opening target\n";
    }

    void closeTarget() {
        std::cout << "closing target\n";
    }
};

class CsvExporter final : public DataExporter {
protected:
    void writeHeader() override {
        std::cout << "id,name,price\n";
    }
    void writeBody() override {
        std::cout << "1,widget,9.99\n2,gadget,14.50\n";
    }
};

class JsonExporter final : public DataExporter {
protected:
    void writeHeader() override {
        std::cout << "{\n  \"items\": [\n";
    }
    void writeBody() override {
        std::cout << "    {\"id\": 1, \"name\": \"widget\", \"price\": 9.99},\n";
        std::cout << "    {\"id\": 2, \"name\": \"gadget\", \"price\": 14.50}\n";
    }
    void writeFooter() override {
        std::cout << "  ]\n}\n";
    }
};

int main() {
    std::cout << "Template Method pattern example:\n";

    CsvExporter csv;
    std::cout << "-- CSV export --\n";
    csv.exportData();

    JsonExporter json;
    std::cout << "-- JSON export --\n";
    json.exportData();

    return 0;
}