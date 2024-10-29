#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H

#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>

class MessageHandler {
public:
    MessageHandler() = default;

    void send(const std::string& message);
    std::string receive();

private:
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> messageQueue;
};

#endif // MESSAGE_HANDLER_H
