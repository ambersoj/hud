#include "MPPNode.hpp"
#include "mpp_defines.hpp"
#include <iostream>

MPPNode::MPPNode() {
    for (int i = 0; i < NUM_SOCKETS; i++) {
        sockets.insert({HUD_PORT_BASE + i, UDPSocket(HUD_PORT_BASE + i)});
    }
    std::cout << "[INFO] MPPNode initialized.\n";
}

std::unordered_map<int, UDPSocket>& MPPNode::getSockets() {  
    return sockets;
}

CommandInvoker& MPPNode::getInvoker() {
    return invoker;
}

MPPNode& MPPNode::getInstance() {
    static MPPNode instance;
    return instance;
}
