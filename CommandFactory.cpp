#include "CommandFactory.hpp"
#include <iostream>

std::unique_ptr<Command> CommandFactory::createCommand(const std::string& commandName, 
                                                       const Json::Value& json, 
                                                       std::unordered_map<int, UDPSocket>& sockets,
                                                       CommandInvoker& invoker) {
    if (commandName == "send") {
        if (json.isMember("src_port") && json.isMember("dst_ip") && 
            json.isMember("dst_port") && json.isMember("message")) {
            int src_port = json["src_port"].asInt();
            std::string dst_ip = json["dst_ip"].asString();
            int dst_port = json["dst_port"].asInt();
            std::string message = json["message"].asString();

            if (sockets.find(src_port) != sockets.end()) {
                return std::make_unique<SendCommand>(sockets[src_port], dst_ip, dst_port, message);
            } else {
                std::cerr << "Invalid src_port: " << src_port << std::endl;
            }
        } else {
            std::cerr << "Missing required fields in send command JSON." << std::endl;
        }
    } 
    else if (commandName == "recv") {
        if (json.isMember("src_port")) {
            int src_port = json["src_port"].asInt();
            
            if (sockets.find(src_port) != sockets.end()) {
                return std::make_unique<RecvCommand>(sockets[src_port]);
            } else {
                std::cerr << "Invalid src_port: " << src_port << std::endl;
            }
        } else {
            std::cerr << "Missing required field 'src_port' in recv command JSON." << std::endl;
        }
    } 
    else if (commandName == "start") {
        return std::make_unique<StartCommand>(invoker);
    }
    else if (commandName == "stop") {
        return std::make_unique<StopCommand>();
    } 
    else if (commandName == "al") {
        return std::make_unique<AddListenerCommand>(json["port"].asInt());
    }
    else if (commandName == "rl") {
        return std::make_unique<RemoveListenerCommand>(json["port"].asInt());
    }
    else {
        std::cerr << "Unknown command: " << commandName << std::endl;
    }

    return nullptr;
}
