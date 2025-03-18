#include "RecvCommand.hpp"
#include <iostream>

RecvCommand::RecvCommand(UDPChannel& channel) : channel(channel) {}

std::string RecvCommand::execute() {
    std::string data = channel.receive();
    if(data != "") {
        std::cout << "Received data: " << data << std::endl;
    }
    return data;
}
