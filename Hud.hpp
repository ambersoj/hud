#ifndef HUD_HPP
#define HUD_HPP

#include <iostream>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <jsoncpp/json/json.h>
#include "UDPSocket.hpp"
#include "CommandInvoker.hpp"

class Hud {
public:
    Hud();

    void start(std::unordered_map<int, UDPSocket>& sockets);
    void stop();
    void initializeFromJson(const std::string& jsonMsg);
    void shutdownNcurses();

    void update(const std::string& jsonMsg);

    void setSpinnerState(const std::string& state);

private:
std::atomic<bool> running;
std::thread hudThread;
std::unordered_map<int, UDPSocket>* sockets;
std::queue<std::string> hudCommandQueue;
std::mutex hudQueueMutex;
std::mutex hudMutex;
std::string spinnerState;
std::string channelColor;
std::string direction;


    void run();

    void processQueue();

    void applyJsonCommand(const std::string& jsonMsg);

    void draw();
};

#endif