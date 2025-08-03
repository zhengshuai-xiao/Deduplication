#pragma once

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <algorithm>

class Buffer {
public:
    explicit Buffer(size_t size) : size_(size), data_(new char[size]) {
        if (data_ == nullptr) {
            throw std::bad_alloc();
        }
        std::memset(data_, 0, size_);
    }

    virtual ~Buffer() {
        delete[] data_;
        data_ = nullptr;
        size_ = 0;
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    Buffer(Buffer&& other) noexcept 
        : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    size_t size() const noexcept {
        return size_;
    }

    char* data() noexcept {
        return data_;
    }

    const char* data() const noexcept {
        return data_;
    }

    size_t write(const char* src, size_t len, size_t offset = 0) {
        if (offset >= size_) {
            return 0;
        }
        size_t write_len = std::min(len, size_ - offset);
        std::memcpy(data_ + offset, src, write_len);
        return write_len;
    }

    size_t read(char* dest, size_t len, size_t offset = 0) const {
        if (offset >= size_) {
            return 0;
        }
        size_t read_len = std::min(len, size_ - offset);
        std::memcpy(dest, data_ + offset, read_len);
        return read_len;
    }

    void clear() {
        std::memset(data_, 0, size_);
    }

    char& operator[](size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Buffer index out of range");
        }
        return data_[index];
    }

    const char& operator[](size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Buffer index out of range");
        }
        return data_[index];
    }

private:
    size_t size_; 
    char* data_;  
};
