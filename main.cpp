#include "MPPNode.hpp"
#include "mpp_defines.hpp"
#include <iostream>

int main() {
    MPPNode& node = MPPNode::getInstance();
    node.start();

    std::string command;
    while (true) {
        if (std::cin >> command) {
            if (command == "exit") break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
