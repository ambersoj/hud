#ifndef SEND_COMMAND_HPP
#define SEND_COMMAND_HPP

#include "Command.hpp"
#include "UDPChannel.hpp"

class SendCommand : public Command {
public:
    SendCommand(UDPChannel &src_port, const std::string &dst_ip, int dst_port, const std::string &message);
    std::string execute() override;

private:
    UDPChannel &src_port;
    std::string dst_ip;
    int dst_port;
    std::string message;
};

#endif // SEND_COMMAND_HPP
