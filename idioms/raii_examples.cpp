// RAII (Resource Acquisition Is Initialization) example
// Compile with: g++ -std=c++20 -O2 idioms/raii_examples.cpp -o bin/raii_example && ./bin/raii_example

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

// Description:
// RAII ties a resource's lifetime to an object's lifetime. Acquisition happens in
// the constructor; release happens in the destructor. Because C++ guarantees
// destructors run when an object goes out of scope — including during stack
// unwinding from an exception — a resource wrapped this way can never be leaked
// by a missed cleanup call, an early return, or an exception.
//
// This isn't a GoF design pattern; it's a core C++ idiom, and it underlies things
// you already use every day: std::unique_ptr, std::lock_guard, std::fstream, and
// std::vector all manage a resource (memory, a lock, a file handle) via RAII.
//
// Use RAII when:
// - you're managing any resource that must be released exactly once: heap memory,
//   file handles, mutex locks, network sockets, database connections;
// - cleanup must happen reliably even if an exception is thrown partway through,
//   or if a function returns early from one of several exit points;
// - you want to eliminate manual acquire/release pairs (and the bugs that come
//   from forgetting the release, or releasing on the wrong path).
//
// Avoid / adapt RAII when:
// - the resource has no clear single owner — shared ownership needs a
//   reference-counted RAII wrapper (shared_ptr) rather than a simple exclusive one;
// - the resource must be released at a different time than the owning object's
//   scope ends — in that case, expose an explicit release() and make the
//   destructor a safe fallback rather than the primary release mechanism.
//
// Tips:
// - Delete the copy constructor/assignment (or implement them correctly) so a
//   resource isn't accidentally released twice; implement move semantics so
//   ownership can be transferred cleanly.
// - Keep RAII wrapper classes narrow: acquire one resource, release it, nothing more.
// - Prefer standard library RAII types (unique_ptr, lock_guard, fstream) over
//   hand-rolled ones whenever they already cover the need.

class FileHandle {
public:
    explicit FileHandle(const std::string& filename) {
        file_ = std::fopen(filename.c_str(), "w");
        if (!file_) {
            throw std::runtime_error("failed to open file: " + filename);
        }
        std::cout << "opened " << filename << "\n";
    }

    // Non-copyable: only one FileHandle should own a given FILE*.
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // Movable: ownership can transfer, but never duplicate.
    FileHandle(FileHandle&& other) noexcept : file_(other.file_) {
        other.file_ = nullptr;
    }
    FileHandle& operator=(FileHandle&& other) noexcept {
        if (this != &other) {
            close();
            file_ = other.file_;
            other.file_ = nullptr;
        }
        return *this;
    }

    ~FileHandle() {
        close();
    }

    void write(const std::string& text) {
        std::fputs(text.c_str(), file_);
    }

private:
    void close() {
        if (file_) {
            std::fclose(file_);
            std::cout << "closed file\n";
            file_ = nullptr;
        }
    }

    std::FILE* file_ = nullptr;
};

void writeReport(bool triggerError) {
    FileHandle file("report.txt");   // resource acquired here
    file.write("starting report\n");

    if (triggerError) {
        throw std::runtime_error("something went wrong mid-report");
        // file's destructor still runs during stack unwinding — no leak.
    }

    file.write("report complete\n");
}   // resource released here, on every exit path

int main() {
    std::cout << "RAII example:\n";

    writeReport(false);   // normal path: opens, writes, closes

    try {
        writeReport(true);   // exception path: opens, throws, still closes
    } catch (const std::exception& e) {
        std::cout << "caught: " << e.what() << "\n";
    }

    std::cout << "-- mutex RAII (standard library) --\n";
    std::mutex mtx;
    {
        std::lock_guard<std::mutex> lock(mtx);   // acquires the lock
        std::cout << "critical section\n";
    }   // lock released automatically here, even if this block threw

    return 0;
}