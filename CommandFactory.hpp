#ifndef COMMAND_FACTORY_HPP
#define COMMAND_FACTORY_HPP

#include "Command.hpp"
#include "SendCommand.hpp"
#include "RecvCommand.hpp"
#include <jsoncpp/json/json.h>
#include <unordered_map>
#include <memory>

class CommandFactory {
public:
    static std::unique_ptr<Command> createCommand(const std::string& commandName, const Json::Value& json, 
                                                  std::unordered_map<int, UDPChannel>& channels) {
        if (commandName == "send") {
            if (json.isMember("src_port") && json.isMember("dst_ip") && json.isMember("dst_port") && json.isMember("message")) {
                int src_port = json["src_port"].asInt();
                std::string dst_ip = json["dst_ip"].asString();
                int dst_port = json["dst_port"].asInt();
                std::string message = json["message"].asString();

                if (channels.find(src_port) != channels.end()) {
                    return std::make_unique<SendCommand>(channels[src_port], dst_ip, dst_port, message);
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
                
                if (channels.find(src_port) != channels.end()) {
                    return std::make_unique<RecvCommand>(channels[src_port]);
                } else {
                    std::cerr << "Invalid src_port: " << src_port << std::endl;
                }
            } else {
                std::cerr << "Missing required field 'src_port' in recv command JSON." << std::endl;
            }
        } 
        else {
            std::cerr << "Unknown command: " << commandName << std::endl;
        }

        return nullptr;
    }
};

#endif // COMMAND_FACTORY_HPP
