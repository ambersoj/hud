#ifndef RECV_COMMAND_HPP
#define RECV_COMMAND_HPP

#include "Command.hpp"
#include "UDPChannel.hpp"

class RecvCommand : public Command {
private:
    UDPChannel& channel;
public:
    explicit RecvCommand(UDPChannel& channel);
    std::string execute() override;
};

#endif // RECV_COMMAND_HPP
