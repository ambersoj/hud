#include "RecvCommand.hpp"
#include <iostream>

RecvCommand::RecvCommand(UDPChannel& src_port) : src_port(src_port) {}

std::string RecvCommand::execute() {
    std::string data = src_port.receive();
    if(data != "") {
        std::cout << "Received data: " << data << std::endl;
    }
    return data;
}
