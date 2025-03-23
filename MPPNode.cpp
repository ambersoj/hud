#include "MPPNode.hpp"
#include "Hud.hpp"
#include "mpp_defines.hpp"
#include "CommandFactory.hpp"
#include "CommandInvoker.hpp"
#include "jsoncpp/json/json.h"
#include <iostream>
#include <thread>

MPPNode::MPPNode() : running(false) {}

void MPPNode::start() {
    if (running) return;
    running = true;

    for (int i = 0; i < NUM_SOCKETS; i++) {
        sockets.insert({HUD_PORT_BASE + i, UDPSocket(HUD_PORT_BASE + i)});
    }

    std::cout << "[INFO] MPPNode::start()\n";

    invoker = new CommandInvoker();

    run();

    std::unique_ptr<Command> addListenerCmd = std::make_unique<AddListenerCommand>(BOOT_TIME_LISTENER_PORT);
    addListenerCmd->execute();
}

void MPPNode::run() {
    if (running) {
        std::cout << "[INFO] MPPNode::run()\n";
        if (!hud) {
            hud = new Hud();
        }
        hud->start(sockets);
    } else {
        std::cout << "[INFO] MPPNode::run() failed with running = " << running << "\n";
    }
}

void MPPNode::stop() {
    running = false;
    if (hud) {
        hud->stop();
        delete hud;
        hud = nullptr;
    }
    std::cout << "[INFO] MPPNode stopped.\n";
}

MPPNode& MPPNode::getInstance() {
    static MPPNode instance;
    return instance;
}

std::unordered_map<int, UDPSocket>& MPPNode::getSockets() {  
    if (sockets.empty()) {
        for (int i = 0; i < NUM_SOCKETS; i++) {
            sockets.insert({HUD_PORT_BASE + i, UDPSocket(HUD_PORT_BASE + i)});
        }
    }
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

                    Json::CharReaderBuilder reader;
                    Json::Value root;
                    std::string errs;
                    std::istringstream s(message);
                        
                    if (!Json::parseFromStream(reader, s, &root, &errs)) {
                        std::cerr << "[ERROR] Failed to parse JSON: " << errs << std::endl;
                        return;
                    }
                        
                    std::string commandName = root.isMember("command") ? root["command"].asString() : "";
                    if (commandName.empty()) {
                        std::cerr << "[ERROR] Received JSON but 'command' field is missing or empty." << std::endl;
                        return;
                    }
                                            
                    Json::Value commandData = root;

                    Json::StreamWriterBuilder writer;
                    std::string commandStr = Json::writeString(writer, root);
                    std::istringstream(commandStr) >> commandData;
                    

                    std::unique_ptr<Command> cmd = CommandFactory::createCommand(
                        commandName,  
                        commandData,  
                        sockets,     
                        *invoker,
                        *hud      
                    );

                    if (cmd) {
                        cmd->execute();
                    } else {
                        std::cerr << "[ERROR] Unknown command type: " << commandName << std::endl;
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

CommandInvoker& MPPNode::getInvoker() {
    return *invoker;
}
