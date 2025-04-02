// MessageRouter.cpp
#include "MessageRouter.hpp"
#include <iostream>

MessageRouter::MessageRouter(std::map<int, MessageFIFO>& fifos)
    : messageFIFOs(fifos) {}

void MessageRouter::route() {
    for (auto& [socket, fifo] : messageFIFOs) {
        while (!fifo.empty()) {
            std::string msg = fifo.dequeue();
            // TODO: determine message type and route
            std::cout << "Routing message from socket " << socket << ": " << msg << "\n";
        }
    }
}
