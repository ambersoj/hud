#include "Hud.hpp"

Hud::Hud() : spinnerState("idle"), channelColor("default"), direction("none") {}

void Hud::update(const std::string& jsonMsg) {
    std::lock_guard<std::mutex> lock(hudMutex);
    applyJsonUpdate(jsonMsg);
}

void Hud::applyJsonUpdate(const std::string& jsonMsg) {
    Json::CharReaderBuilder reader;
    Json::Value root;
    std::string errs;
    std::istringstream s(jsonMsg);
    
    if (!Json::parseFromStream(reader, s, &root, &errs)) {
        std::cerr << "[ERROR] Failed to parse JSON: " << errs << std::endl;
        return;
    }

    if (root.isMember("spinnerState")) {
        spinnerState = root["spinnerState"].asString();
    }
    if (root.isMember("channelColor")) {
        channelColor = root["channelColor"].asString();
    }
    if (root.isMember("direction")) {
        direction = root["direction"].asString();
    }
}

void Hud::draw() const {
    std::lock_guard<std::mutex> lock(hudMutex);
    std::cout << "[HUD] Spinner: " << spinnerState 
              << " | Channel Color: " << channelColor 
              << " | Direction: " << direction 
              << std::endl;
}
