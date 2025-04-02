# include "CommandFactory.hpp"
# include <iostream>
# include <jsoncpp/json/json.h>
# include <queue>
# include <mutex>
# include <thread>

std::unique_ptr<Command> CommandFactory::createCommand(
    const std::string& commandName,
    const Json::Value& json,
    std::unordered_map<int, UDPSocket>& sockets,
    CommandInvoker& invoker
) {
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
                std::cerr << "[ERROR] No socket found for src_port: " << src_port << std::endl;
                return nullptr;
            }
        } else {
            std::cerr << "[ERROR] Missing required fields in send command JSON." << std::endl;
            return nullptr;
        }
    } 
    else if (commandName == "recv") {
        if (json.isMember("src_port")) {
            int src_port = json["src_port"].asInt();
            
            if (sockets.find(src_port) != sockets.end()) {
                return std::make_unique<RecvCommand>(sockets[src_port]);
            } else {
                std::cerr << "[ERROR] No socket found for src_port: " << src_port << std::endl;
                return nullptr;
            }
        } else {
            std::cerr << "[ERROR] Missing required field 'src_port' in recv command JSON." << std::endl;
            return nullptr;
        }
    } 
    else {
        std::cerr << "[ERROR] Unknown command: " << commandName << std::endl;
        return nullptr;
    }
}
