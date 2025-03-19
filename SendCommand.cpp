#include "SendCommand.hpp"
#include <iostream>

SendCommand::SendCommand(UDPChannel &src_port, const std::string &dst_ip, int dst_port, const std::string &message)
    : src_port(src_port), dst_ip(dst_ip), dst_port(dst_port), message(message) {}

std::string SendCommand::execute() {
    src_port.send(dst_ip, dst_port, message);
    return "";
}
