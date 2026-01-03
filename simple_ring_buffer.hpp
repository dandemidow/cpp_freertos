#ifndef SIMPLE_RING_BUFFER_H
#define SIMPLE_RING_BUFFER_H

#include <array>
#include <cstdint>

template <class T>
struct SimpleRingBuffer {
    using size_type = uint8_t;
    size_type read {};
    size_type write {};
    std::array<T, 8> buffer_ {};
    using value_type = T;
    static constexpr size_type kMask {0b0111U};

    SimpleRingBuffer() = default;

    bool Size() const {
        //assert(write >= read);
        return write - read;
    }

    void Push(value_type v) {
        if (Size() < buffer_.size()) {
          buffer_[write & kMask] = v;
          write++;
        }
    }
    T Pop() {
        if (Size()) {
          T res {buffer_[read & kMask]};
          read++;
          if (read == write) {
            read = write = {};
          }
          return res;
        }
        return {};
    }
};

#endif  // SIMPLE_RING_BUFFER_H
