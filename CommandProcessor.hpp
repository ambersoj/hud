#pragma once
#include <string>
#include <queue>
#include <mutex>

class CommandProcessor {
public:
    void enqueue(const std::string& cmd);
    void run();

private:
    std::queue<std::string> queue_;
    std::mutex mutex_;
};
