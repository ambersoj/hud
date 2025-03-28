#include "Hud.hpp"
#include "mpp_defines.hpp"
#include "MPPCommands.hpp"
#include "CommandInvoker.hpp"
#include <ncurses.h>
#include <chrono>
#include <thread>

Hud::Hud() {
    running = false;
}

void Hud::start(std::unordered_map<int, UDPSocket>& sockets) {
    if (running.load()) {
        std::cerr << "[WARNING] Hud is already running!" << std::endl;
        return;
    }

    if (sockets.empty()) {
        std::cerr << "[ERROR] Sockets map is empty!" << std::endl;
        return;
    }
    this->sockets = &sockets;

    running.store(true);
    
    try {
        hudThread = std::thread(&Hud::run, this);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to start Hud thread: " << e.what() << std::endl;
    }
}


void Hud::stop() {
    running.store(false);
    if (hudThread.joinable()) {
        hudThread.join();
    }
}

void Hud::initializeFromJson(const std::string& jsonMsg) {
    applyJsonCommand(jsonMsg);
}

void Hud::shutdownNcurses() {
    // Cleanly shut down ncurses if it's running
    // If ncurses is running, call endwin();
}


void Hud::update(const std::string& jsonMsg) {
    std::lock_guard<std::mutex> lock(hudQueueMutex);
    hudCommandQueue.push(jsonMsg);
}

void Hud::setSpinnerState(const std::string& state) {
    {
        std::lock_guard<std::mutex> lock(hudMutex);
        spinnerState = state;
    }
    draw(); // Immediate refresh
}

void Hud::run() {
    try {
        while (running.load()) {
            processQueue();
            draw();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception in Hud::run: " << e.what() << std::endl;
    }
}

void Hud::processQueue() {
    std::lock_guard<std::mutex> lock(hudQueueMutex);
    while (!hudCommandQueue.empty()) {
        std::string cmd = hudCommandQueue.front();
        hudCommandQueue.pop();
        applyJsonCommand(cmd);
    }
}

void Hud::applyJsonCommand(const std::string& jsonMsg) {
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errs;
    std::istringstream s(jsonMsg);
    
    if (!Json::parseFromStream(reader, s, &root, &errs)) {
        std::cerr << "[ERROR] Failed to parse JSON: " << errs << std::endl;
        return;
    }

    std::cout << "[DEBUG] Parsed JSON: " << root << std::endl;  // Add this line for debugging

    if (root.isMember("spinnerState")) {
        setSpinnerState(root["spinnerState"].asString());
    }
    if (root.isMember("channelColor")) {
        channelColor = root["channelColor"].asString();
        std::cout << "[DEBUG] Updated channelColor: " << channelColor << std::endl;  // Debug
    }
    if (root.isMember("direction")) {
        direction = root["direction"].asString();
        std::cout << "[DEBUG] Updated direction: " << direction << std::endl;  // Debug
    }
}


void Hud::draw() {
    std::lock_guard<std::mutex> lock(hudMutex);
    std::cout << "[HUD] Spinner: " << spinnerState 
              << " | Channel Color: " << channelColor 
              << " | Direction: " << direction 
              << std::endl;
}