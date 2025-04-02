#include "MessageFIFO.hpp"

void MessageFIFO::enqueue(const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(message);
}

std::string MessageFIFO::dequeue() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (queue_.empty()) return {};
    std::string msg = queue_.front();
    queue_.pop();
    return msg;
}

bool MessageFIFO::empty() {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.empty();
}
