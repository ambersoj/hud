#ifndef HUD_HPP
#define HUD_HPP

#include <iostream>
#include <mutex>
#include <jsoncpp/json/json.h>

class Hud {
public:
    Hud();

    void update(const std::string& jsonMsg);
    void draw() const;

private:
    mutable std::mutex hudMutex;
    std::string spinnerState;
    std::string channelColor;
    std::string direction;

    void applyJsonUpdate(const std::string& jsonMsg);
};

#endif
