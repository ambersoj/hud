#ifndef COMMAND_INVOKER_HPP
#define COMMAND_INVOKER_HPP

#include "Command.hpp"
#include <memory>
#include <queue>

class CommandInvoker {
public:
    std::string addCommand(std::unique_ptr<Command> command);
    void executeCommands(); // 🔥 Added this back!

private:
    std::queue<std::unique_ptr<Command>> commandQueue;
};

#endif // COMMAND_INVOKER_HPP
