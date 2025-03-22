#include "UDPSocket.hpp"
#include "MPPNode.hpp"
#include "CommandInvoker.hpp"
#include "MPPCommands.hpp"
#include <iostream>

std::atomic<bool> running;

int main() {
    CommandInvoker invoker;
    MPPNode& node = MPPNode::getInstance();

    node.start(invoker);

    for (int i = 0; i < 6; i++) {
        node.getSockets().emplace(6000 + i, UDPSocket(6000 + i));
    }

    node.run();
    
    std::cout << "MPPNode is running. Use UDP commands to control it.\n";

    std::string command;
    while (true) {
        std::cin >> command;
        if (command == "exit") {
            break;
        }
    }

    node.stop();
    return 0;
}
