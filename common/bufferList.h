#pragma once
#include "buffer.h"
#include <vector>
#include <memory>
#include <stdexcept>

class BufferList {
public:
    BufferList() = default;

    BufferList(const BufferList&) = delete;
    BufferList& operator=(const BufferList&) = delete;

    BufferList(BufferList&&) noexcept = default;
    BufferList& operator=(BufferList&&) noexcept = default;

    void add_buffer(size_t size) {
        buffers_.emplace_back(std::make_unique<Buffer>(size));
    }

    void add_buffer(Buffer&& buffer) {
        buffers_.emplace_back(std::make_unique<Buffer>(std::move(buffer)));
    }

    Buffer& get_buffer(size_t index) {
        if (index >= buffers_.size()) {
            throw std::out_of_range("BufferList index out of range");
        }
        return *buffers_[index];
    }

    const Buffer& get_buffer(size_t index) const {
        if (index >= buffers_.size()) {
            throw std::out_of_range("BufferList index out of range");
        }
        return *buffers_[index];
    }

    void remove_buffer(size_t index) {
        if (index >= buffers_.size()) {
            throw std::out_of_range("BufferList index out of range");
        }
        buffers_.erase(buffers_.begin() + index);
    }

    size_t count() const noexcept {
        return buffers_.size();
    }

    size_t total_size() const noexcept {
        size_t total = 0;
        for (const auto& buf : buffers_) {
            total += buf->size();
        }
        return total;
    }

    void clear() noexcept {
        buffers_.clear();
    }

    size_t write_buffers(const char* data, size_t len, size_t start_index = 0) {
        if (start_index >= buffers_.size() || len == 0) {
            return 0;
        }

        size_t remaining = len;
        size_t total_written = 0;
        const char* current_data = data;

        for (size_t i = start_index; i < buffers_.size() && remaining > 0; ++i) {
            Buffer& buf = *buffers_[i];
            size_t written = buf.write(current_data, remaining);
            
            total_written += written;
            current_data += written;
            remaining -= written;
        }

        return total_written;
    }

    auto begin() { return buffers_.begin(); }
    auto end() { return buffers_.end(); }
    auto begin() const { return buffers_.begin(); }
    auto end() const { return buffers_.end(); }
    auto cbegin() const { return buffers_.cbegin(); }
    auto cend() const { return buffers_.cend(); }

private:
    std::vector<std::unique_ptr<Buffer>> buffers_;
};


/*
#include "buffer.h"
#include <list>
#include <memory>
#include <stdexcept>
#include <iterator>

class BufferList {
public:
    BufferList() = default;

    BufferList(const BufferList&) = delete;
    BufferList& operator=(const BufferList&) = delete;

    BufferList(BufferList&&) noexcept = default;
    BufferList& operator=(BufferList&&) noexcept = default;

    void add_buffer(size_t size) {
        buffers_.emplace_back(std::make_unique<Buffer>(size));
    }

    void add_buffer(Buffer&& buffer) {
        buffers_.emplace_back(std::make_unique<Buffer>(std::move(buffer)));
    }

    void insert_buffer(size_t index, size_t buffer_size) {
        if (index > buffers_.size()) {
            throw std::out_of_range("BufferList index out of range");
        }
        auto it = buffers_.begin();
        std::advance(it, index);
        buffers_.insert(it, std::make_unique<Buffer>(buffer_size));
    }

    Buffer& get_buffer(size_t index) {
        auto it = get_iterator(index);
        return **it;
    }

    const Buffer& get_buffer(size_t index) const {
        auto it = get_iterator(index);
        return**it;
    }

    void remove_buffer(size_t index) {
        auto it = get_iterator(index);
        buffers_.erase(it);
    }

    size_t count() const noexcept {
        return buffers_.size();
    }

    size_t total_size() const noexcept {
        size_t total = 0;
        for (const auto& buf : buffers_) {
            total += buf->size();
        }
        return total;
    }

    void clear() noexcept {
        buffers_.clear();
    }

    size_t write_buffers(const char* data, size_t len, size_t start_index = 0) {
        if (start_index >= buffers_.size() || len == 0) {
            return 0;
        }

        size_t remaining = len;
        size_t total_written = 0;
        const char* current_data = data;
        size_t current_index = 0;

        for (auto& buf : buffers_) {
            if (current_index >= start_index && remaining > 0) {
                size_t written = buf->write(current_data, remaining);
                total_written += written;
                current_data += written;
                remaining -= written;
            }
            current_index++;
        }

        return total_written;
    }

    auto begin() { return buffers_.begin(); }
    auto end() { return buffers_.end(); }
    auto begin() const { return buffers_.begin(); }
    auto end() const { return buffers_.end(); }
    auto cbegin() const { return buffers_.cbegin(); }
    auto cend() const { return buffers_.cend(); }

private:

    auto get_iterator(size_t index) const {
        if (index >= buffers_.size()) {
            throw std::out_of_range("BufferList index out of range");
        }
        auto it = buffers_.begin();
        std::advance(it, index);
        return it;
    }

    std::list<std::unique_ptr<Buffer>> buffers_; 
};

*/