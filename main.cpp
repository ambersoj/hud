#include "Hud.hpp"
#include "CommandFactory.hpp"
#include "CommandInvoker.hpp"
#include "RecvCommand.hpp"
#include "SendCommand.hpp"
#include "UDPChannel.hpp"
#include "mpp_defines.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <sstream>
#include <unordered_map>
#include <poll.h>
#include <jsoncpp/json/json.h>

std::atomic<bool> running(true);
std::atomic<bool> hud_running(false);
int listener_port = -1; // Default: no listener

void run(std::unordered_map<int, UDPChannel> &channels, CommandInvoker &invoker) {
    Hud hud;
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    std::cout << "> ";
    while (running) {
        std::cout.flush();
        invoker.executeCommands();
        
        if (poll(&pfd, 1, 100) > 0) {
            std::string line;
            std::getline(std::cin, line);
            
            if (line == "start") {
                if (!hud_running) {
                    hud.start(channels, invoker);
                    hud_running = true;
                } else {
                    std::cout << "Hud already running." << std::endl;
                }
            } else if (line == "exit") {
                hud.stop();
                running = false;
                break;
            } else if (line.rfind("sl ", 0) == 0) { // Set listener command
                listener_port = std::stoi(line.substr(3));
                std::cout << "Listener set on port: " << listener_port << std::endl;
            } else {
                std::istringstream iss(line);
                std::string token;
                std::vector<std::string> tokens;
                while (iss >> token) tokens.push_back(token);
                
                if (!tokens.empty()) {
                    if (tokens[0] == "send" && tokens.size() >= 5) {
                        int ch = std::stoi(tokens[1]);
                        if (channels.find(ch) != channels.end()) {
                            int dst_port = std::stoi(tokens[3]);
                            std::string message = line.substr(line.find(tokens[4]));
                            invoker.addCommand(std::make_unique<SendCommand>(channels[ch], tokens[2], dst_port, message));
                        } else {
                            std::cerr << "Invalid channel: " << ch << std::endl;
                        }
                    } 
                    else if (tokens[0] == "recv" && tokens.size() >= 2) {
                        int ch = std::stoi(tokens[1]);
                        if (channels.find(ch) != channels.end()) {
                            invoker.addCommand(std::make_unique<RecvCommand>(channels[ch]));
                        } else {
                            std::cerr << "Invalid channel: " << ch << std::endl;
                        }
                    } 
                    else {
                        std::cout << "Invalid command" << std::endl;
                    }
                }
            }
        }
        
        if (listener_port != -1) {
            RecvCommand recvCmd(channels[listener_port]);
            std::string msg = recvCmd.execute();
            if (!msg.empty()) {
                Json::CharReaderBuilder reader;
                Json::Value parsed;
                std::string errs;
                std::istringstream s(msg);
                
                if (Json::parseFromStream(reader, s, &parsed, &errs)) {
                    if (parsed.isMember("command")) {
                        std::string commandName = parsed["command"].asString();
                        std::unique_ptr<Command> cmd = CommandFactory::createCommand(commandName, parsed, channels);
                        if (cmd) {
                            invoker.addCommand(std::move(cmd));
                        } else {
                            std::cerr << "Failed to create command from JSON." << std::endl;
                        }
                    }
                }
            }
        }
    }
}

int main() {
    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;
    
    for (int i = HUD_PORT_BASE; i < HUD_PORT_BASE + 6; ++i) {
        channels.emplace(i, UDPChannel(i));
    }
    
    run(channels, invoker);
    return 0;
}
