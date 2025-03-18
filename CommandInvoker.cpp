#include "CommandInvoker.hpp"
#include <iostream>

std::string CommandInvoker::addCommand(std::unique_ptr<Command> command) {
//    std::cout << "[DEBUG] Adding command: " << typeid(*command).name() << " | New queue size: " << commandQueue.size() + 1 << std::endl;
    commandQueue.push(std::move(command));
    return "";
}


void CommandInvoker::executeCommands() {
    int count = 0;
    while (!commandQueue.empty()) {
        auto cmd = std::move(commandQueue.front());
        commandQueue.pop();
        cmd->execute();
        std::cout << "> ";
        
        if (++count > 50) { // Prevent infinite loops
            std::cout << "\n[ERROR] Infinite loop suspected. Breaking out!\n";
            break;
        }
    }
}
