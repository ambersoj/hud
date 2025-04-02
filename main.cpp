#include "MPPNode.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    MPPNode& node = MPPNode::getInstance();

    std::string command;
    while (true) {
        if (std::cin >> command) {
            if (command == "exit") break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
