#include "SendCommand.hpp"
#include <iostream>

SendCommand::SendCommand(UDPChannel &channel, const std::string &dst_ip, int dst_port, const std::string &message)
    : channel(channel), dst_ip(dst_ip), dst_port(dst_port), message(message) {}

std::string SendCommand::execute() {
    channel.send(dst_ip, dst_port, message);
    return "";
}
