#ifndef MPP_COMMANDS_HPP
#define MPP_COMMANDS_HPP

#include "Command.hpp"
#include "Hud.hpp"
#include <jsoncpp/json/json.h>
#include "MPPNode.hpp"
#include "UDPSocket.hpp"
#include <iostream>

class RecvCommand : public Command {
    private:
        UDPSocket& src_port;
    public:
        explicit RecvCommand(UDPSocket& src_port);
        std::string execute() override;
};

class SendCommand : public Command {
    public:
        SendCommand(UDPSocket &src_port, const std::string &dst_ip, int dst_port, const std::string &message);
        std::string execute() override;
    
    private:
        UDPSocket &src_port;
        std::string dst_ip;
        int dst_port;
        std::string message;
};

#endif
