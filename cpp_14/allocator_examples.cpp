// C++14 allocator examples
// Reference: https://en.cppreference.com/w/cpp/memory/allocator_traits
// Compile with: g++ -std=c++14 -O2 cpp_14/allocator_examples.cpp -o bin/cpp14_allocator && ./bin/cpp14_allocator

#include <iostream>
#include <memory>
#include <vector>

// Problem solved: gives generic containers a uniform way to rebind and use allocator types.
// Before C++14: custom containers repeated allocator details and handled rebinding manually.
// Tips for C++14 allocator usage:
// - Prefer allocator_traits over calling allocator-specific members directly.
// - Use rebind_traits when an allocator must allocate a different object type.
// - Keep allocation and object lifetime operations separate and exception-safe.
// - Use a custom allocator only when measurement shows a real allocation benefit.

template <typename T, typename Allocator = std::allocator<T>>
class SmallBuffer {
public:
    using Traits = std::allocator_traits<Allocator>;

    explicit SmallBuffer(const Allocator& allocator = Allocator())
        : allocator_(allocator), data_(Traits::allocate(allocator_, 3)), size_(0) {}

    ~SmallBuffer() {
        clear();
        Traits::deallocate(allocator_, data_, 3);
    }

    void push(const T& value) {
        Traits::construct(allocator_, data_ + size_, value);
        ++size_;
    }

    void clear() {
        while (size_ > 0) {
            --size_;
            Traits::destroy(allocator_, data_ + size_);
        }
    }

    const T& operator[](std::size_t index) const {
        return data_[index];
    }

private:
    Allocator allocator_;
    T* data_;
    std::size_t size_;
};

int main() {
    std::cout << "C++14 allocator examples:\n";

    SmallBuffer<int> buffer;
    buffer.push(10);
    buffer.push(20);
    std::cout << "buffer values = " << buffer[0] << ", " << buffer[1] << "\n";

    using ByteAllocator = std::allocator_traits<std::allocator<int>>::rebind_alloc<char>;
    ByteAllocator bytes;
    char* storage = std::allocator_traits<ByteAllocator>::allocate(bytes, 4);
    storage[0] = 'C';
    storage[1] = '+';
    storage[2] = '+';
    storage[3] = '\0';
    std::cout << "rebound storage = " << storage << "\n";
    std::allocator_traits<ByteAllocator>::deallocate(bytes, storage, 4);

    return 0;
}
