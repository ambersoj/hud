#ifndef MPP_COMMANDS_HPP
#define MPP_COMMANDS_HPP

#include "Command.hpp"
#include "MPPNode.hpp"
#include "UDPSocket.hpp"
#include <iostream>

class StartCommand : public Command {
private:
    CommandInvoker& invoker;
    
public:
    explicit StartCommand(CommandInvoker& inv) : invoker(inv) {}
    std::string execute() override;
};

class StopCommand : public Command {
public:
    std::string execute() override;
};

class AddListenerCommand : public Command {
private:
    int port;
public:
    explicit AddListenerCommand(int port) : port(port) {}
    std::string execute() override;
};

class RemoveListenerCommand : public Command {
private:
    int port;
public:
    explicit RemoveListenerCommand(int port) : port(port) {}
    std::string execute() override;
};

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

#endif // MPP_COMMANDS_HPP
