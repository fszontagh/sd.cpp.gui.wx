#ifndef __SERVER_EVENT_QUEUE__
#define __SERVER_EVENT_QUEUE__


class EventQueue {
public:
    using EventHandler = std::function<void()>;

    void Push(EventHandler handler) {
        std::lock_guard<std::mutex> lock(mutex_);
        events_.emplace(handler);
        cond_var_.notify_one();
    }

    EventHandler Pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_var_.wait(lock, [this] { return !events_.empty(); });
        auto event = events_.front();
        events_.pop();
        return event;
    }

    bool IsEmpty() {
        std::lock_guard<std::mutex> lock(mutex_);
        return events_.empty();
    }

private:
    std::queue<EventHandler> events_;
    std::mutex mutex_;
    std::condition_variable cond_var_;
};

#endif  // __SERVER_EVENT_QUEUE__