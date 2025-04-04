#include "MPPNode.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <csignal>

MPPNode& node = MPPNode::getInstance();

void handleSignal(int signal) {
    std::cout << "\n[INFO] Caught signal " << signal << ", shutting down...\n";
    node.stop();
    exit(0);
}

int main() {
    signal(SIGINT, handleSignal);

    std::cout << "[INFO] Ctrl+C to exit.\n";

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
