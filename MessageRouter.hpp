#pragma once
#include "MessageFIFO.hpp"
#include <map>

class MessageRouter {
public:
    MessageRouter(std::map<int, MessageFIFO>& fifos);

    void route();

private:
    std::map<int, MessageFIFO>& messageFIFOs;
};
