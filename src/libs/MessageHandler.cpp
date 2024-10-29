#include "MessageHandler.h"

void MessageHandler::send(const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    messageQueue.push(message);
    cv.notify_one();  // Értesítjük, hogy új üzenet érkezett
}

std::string MessageHandler::receive() {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this]() { return !messageQueue.empty(); });  // Várakozás, amíg a sor nem üres
    std::string message = messageQueue.front();
    messageQueue.pop();
    return message;
}
