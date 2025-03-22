#include "MPPNode.hpp"
#include "mpp_defines.hpp"
#include "CommandFactory.hpp"
#include "CommandInvoker.hpp"
#include "Hud.hpp"
#include "nlohmann/json.hpp"
#include <iostream>
#include <thread>

using json = nlohmann::json;

MPPNode::MPPNode() : running(false) {}

MPPNode& MPPNode::getInstance() {
    static MPPNode instance;
    return instance;
}

void MPPNode::run() {
    if (running) {
        addCommandListener(BOOT_TIME_LISTENER);
        Hud hud;
        hud.start(sockets, *invoker);
        std::cout << "[INFO] MPPNode::run(). Global running = " << running << "\n";
    } else {
        std::cout << "[INFO] MPPNode::run(). Global running = " << running << "\n";
    }
}

void MPPNode::start(CommandInvoker& invoker) {
    this->invoker = &invoker;
    running = true;
    std::cout << "[INFO] MPPNode started.\n";
}

void MPPNode::stop() {
    running = false;
    std::cout << "[INFO] MPPNode stopped.\n";
}

std::unordered_map<int, UDPSocket>& MPPNode::getSockets() {  
    return sockets;
}

void MPPNode::addCommandListener(int port) {
    auto it = sockets.find(port);
    if (it != sockets.end()) {
        int existingSockfd = it->second.getSocket();
        std::cout << "[DEBUG] addCommandListener using existing sockfd: " << existingSockfd << std::endl;
        commandListeners[port] = &it->second;

        std::thread([this, port]() {
            std::cout << "[DEBUG] Listening for commands on port " << port << std::endl;
            while (true) {
                std::string message = sockets[port].receive();  
                if (!message.empty()) {
                    std::cout << "[DEBUG] Received command: " << message << std::endl;

                    try {
                        json root = json::parse(message);
                        std::string commandName = root["command"];

                        Json::Value commandData = Json::Value::null;
                        std::istringstream(root.dump()) >> commandData;

                        std::unique_ptr<Command> cmd = CommandFactory::createCommand(
                            commandName,  
                            commandData,  
                            sockets,     
                            *invoker      
                        );

                        if (cmd) {
                            cmd->execute();
                        } else {
                            std::cerr << "[ERROR] Unknown command type: " << commandName << std::endl;
                        }
                    } catch (json::parse_error& e) {
                        std::cerr << "[ERROR] Failed to parse JSON: " << e.what() << std::endl;
                    }
                }
            }
        }).detach();

        return;
    }

    std::cerr << "[ERROR] No UDPSocket found for port " << port << std::endl;
}

int MPPNode::getSockfdForPort(int port) {
    auto it = sockets.find(port);
    if (it != sockets.end()) {
        return it->second.getSocket();
    }
    return -1;
}

void MPPNode::removeCommandListener(int port) {
    auto it = commandListeners.find(port);
    if (it != commandListeners.end()) {
        commandListeners.erase(it);
        sockets.erase(port);
        std::cout << "[INFO] Command listener removed from port " << port << "\n";
    } else {
        std::cout << "[WARNING] No command listener found on port " << port << "\n";
    }
}
