#include "MPPCommands.hpp"

RecvCommand::RecvCommand(UDPSocket& src_port) : src_port(src_port) {}

std::string RecvCommand::execute() {
    std::string data = src_port.receive();
    if(data != "") {
        std::cout << "Received data: " << data << std::endl;
    }
    return data;
}

SendCommand::SendCommand(UDPSocket &src_port, const std::string &dst_ip, int dst_port, const std::string &message)
    : src_port(src_port), dst_ip(dst_ip), dst_port(dst_port), message(message) {}

std::string SendCommand::execute() {
    src_port.send(dst_ip, dst_port, message);
    return "[INFO] SendCommand executed";
}
