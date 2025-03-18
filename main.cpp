#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <poll.h>
#include "mpp_defines.hpp"
#include "Hud.hpp"
#include "UDPChannel.hpp"
#include "CommandInvoker.hpp"
#include "SendCommand.hpp"
#include "RecvCommand.hpp"
#include <unistd.h>

void run(std::unordered_map<int, UDPChannel> &channels, CommandInvoker &invoker) {
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    std::cout << "> ";
    while (true) {
        std::cout.flush();
        
        invoker.executeCommands();

        if (poll(&pfd, 1, 100) > 0) {
            std::string line;
            std::getline(std::cin, line);
            if (line == "exit")
                break;

            std::vector<std::string> tokens;
            std::istringstream iss(line);
            std::string token;
            while (iss >> token)
                tokens.push_back(token);

            if (tokens.empty())
                continue;
            if (tokens[0] == "send" && tokens.size() >= 5) {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end()) {
                    int dst_port = std::stoi(tokens[3]);
                    std::string message = line.substr(line.find(tokens[4]));
                    invoker.addCommand(std::make_unique<SendCommand>(channels[ch], tokens[2], dst_port, message));
                } else {
                    std::cerr << "Invalid channel: " << ch << std::endl;
                }
            } else if (tokens[0] == "recv" && tokens.size() == 2) {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end()) {
                    invoker.addCommand(std::make_unique<RecvCommand>(channels[ch]));
                } else {
                    std::cerr << "Invalid channel: " << ch << std::endl;
                }
            } else {
                std::cout << "Invalid command" << std::endl;
            }
        }
    }
}

int main() {
    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;

    // Initialize UDP channels
    for (int i = HUD_PORT_BASE; i < HUD_PORT_BASE + 6; ++i) {
        channels.emplace(i, UDPChannel(i));
    }
    
    // Start the HUD system
    Hud hud;
    hud.start(channels, invoker);

    // Run the REPL loop for command execution
    run(channels, invoker);

    // Stop HUD before exiting
    hud.stop();
    return 0;
}
