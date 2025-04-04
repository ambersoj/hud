#pragma once
#include <queue>
#include <mutex>
#include <string>

class MessageFIFO {
public:
    MessageFIFO();
    MessageFIFO(MessageFIFO&&) noexcept = default;
    MessageFIFO& operator=(MessageFIFO&&) noexcept = default;
    void enqueue(const std::string& message);
    std::string dequeue();
    bool empty();

private:
    std::queue<std::string> queue_;
    std::mutex mutex_;

};
