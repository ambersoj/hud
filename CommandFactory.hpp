#ifndef COMMAND_FACTORY_HPP
#define COMMAND_FACTORY_HPP

#include "Command.hpp"
#include "MPPCommands.hpp"
#include "UDPSocket.hpp"
#include "jsoncpp/json/json.h"
#include <unordered_map>
#include <memory>

class CommandFactory {

private:
    Hud& hud;

public:
    CommandFactory(Hud& hud) : hud(hud) {}
    static std::unique_ptr<Command> createCommand(
        const std::string& commandName, 
        const Json::Value& json, 
        std::unordered_map<int, UDPSocket>& sockets,
        CommandInvoker& invoker,
        Hud& hud
    );
};

#endif // COMMAND_FACTORY_HPP
