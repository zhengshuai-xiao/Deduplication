#include "buffer.h"
#include <queue>
#include <memory>
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include "logger.h"

template<class Data>
class BufferQueue {
public:

    explicit BufferQueue(uint32_t max=MAX_TRUNKS) : max_trunks_(max),
    total_size_(0) {}

    BufferQueue(const BufferQueue&) = delete;
    BufferQueue& operator=(const BufferQueue&) = delete;

    BufferQueue(BufferQueue&&) noexcept = default;
    BufferQueue& operator=(BufferQueue&&) noexcept = default;

    void enqueue(std::unique_ptr<Data>&& pdata) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(buffer_queue_.size() >= max_trunks_){
            LOG_TRACE() << "hit the upper limiation(" << max_trunks_ << "), waitting";
        }

        not_full_.wait(lock, [this]() { return buffer_queue_.size() < max_trunks_; });

        total_size_+=pdata->size();
        buffer_queue_.emplace(std::move(pdata));

        not_empty_.notify_one();
    }

    void enqueue(size_t size) {
        std::unique_lock<std::mutex> lock(mutex_);
        if(buffer_queue_.size() >= max_trunks_){
            std::cout << "hit the upper limiation(" << max_trunks_ << "), waitting" << std::endl;
        }

        not_full_.wait(lock, [this]() { return buffer_queue_.size() < max_trunks_; });
        
        buffer_queue_.emplace(std::make_unique<Data>(size));
        total_size_+=size;

        not_empty_.notify_one();
    }

    std::unique_ptr<Data> dequeue() {
        std::unique_lock<std::mutex> lock(mutex_);
        if(buffer_queue_.size() == 0){
            LOG_TRACE() << "there is no data in queue, waitting";
        }
        not_empty_.wait(lock, [this]() { return !buffer_queue_.empty(); });
        
        std::unique_ptr<Data> buffer_ptr = std::move(buffer_queue_.front());
        buffer_queue_.pop();
        total_size_-= buffer_ptr->size();
        
        not_full_.notify_one();

        return std::move(buffer_ptr);
    }

    Data& front() {
        std::unique_lock<std::mutex> lock(mutex_);

        if (is_empty()) {
            LOG_ERROR() << "BufferQueue is empty";
            throw std::runtime_error("BufferQueue is empty");
        }

        return *buffer_queue_.front();
    }

    const Data& front() const {
        std::unique_lock<std::mutex> lock(mutex_);

        if (is_empty()) {
            LOG_ERROR() << "BufferQueue is empty";
            throw std::runtime_error("BufferQueue is empty");
        }
        return *buffer_queue_.front();
    }

    Data& back() {
        
        std::unique_lock<std::mutex> lock(mutex_);

        if (is_empty()) {
            LOG_ERROR() << "BufferQueue is empty";
            throw std::runtime_error("BufferQueue is empty");
        }
        return *buffer_queue_.back();
    }

    const Data& back() const {
        
        std::unique_lock<std::mutex> lock(mutex_);

        if (is_empty()) {
            LOG_ERROR() << "BufferQueue is empty";
            throw std::runtime_error("BufferQueue is empty");
        }
        return *buffer_queue_.back();
    }

    bool is_empty() const noexcept {
        //no need lock
        return buffer_queue_.empty();
    }

    size_t size() const noexcept {
        std::unique_lock<std::mutex> lock(mutex_);
        return buffer_queue_.size();
    }

    
    size_t total_size() const noexcept {
        std::unique_lock<std::mutex> lock(mutex_);
        return total_size_;
;
    }

    void clear() noexcept {
        std::unique_lock<std::mutex> lock(mutex_);
        buffer_queue_.clear();
        total_size_=0;
    }

private:
    mutable std::mutex mutex_;
    std::queue<std::unique_ptr<Data>> buffer_queue_; 
    uint32_t max_trunks_;
    uint32_t total_size_;
    //TODO:
    std::condition_variable not_empty_; 
    std::condition_variable not_full_;
};