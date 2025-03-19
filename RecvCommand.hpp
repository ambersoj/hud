#ifndef RECV_COMMAND_HPP
#define RECV_COMMAND_HPP

#include "Command.hpp"
#include "UDPChannel.hpp"

class RecvCommand : public Command {
private:
    UDPChannel& src_port;
public:
    explicit RecvCommand(UDPChannel& src_port);
    std::string execute() override;
};

#endif // RECV_COMMAND_HPP
