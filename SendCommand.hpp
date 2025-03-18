#ifndef SEND_COMMAND_HPP
#define SEND_COMMAND_HPP

#include "Command.hpp"
#include "UDPChannel.hpp"

class SendCommand : public Command {
public:
    SendCommand(UDPChannel &channel, const std::string &dst_ip, int dst_port, const std::string &message);
    std::string execute() override;

private:
    UDPChannel &channel;
    std::string dst_ip;
    int dst_port;
    std::string message;
};

#endif // SEND_COMMAND_HPP
