#include "MPPNode.hpp"
#include "mpp_defines.hpp"
#include <iostream>

MPPNode::MPPNode() : running(true) {
    for (int i = 0; i < NUM_SOCKETS; i++) {
        int port = HUD_PORT_BASE + i;
        sockets.emplace(port, UDPSocket(port));
        fifos.emplace(std::piecewise_construct, std::forward_as_tuple(port), std::forward_as_tuple());
        
        threads.emplace_back(&MPPNode::listenerThread, this, port);
    }
    std::cout << "[INFO] MPPNode initialized.\n";
}

MPPNode::~MPPNode() {
    stop();
}

void MPPNode::stop() {
    running.store(false);

    for (auto& thread : threads) {
        if (thread.joinable()) thread.join();
    }

    std::cout << "[INFO] MPPNode shutting down.\n";
}

MPPNode& MPPNode::getInstance() {
    static MPPNode instance;
    return instance;
}

void MPPNode::send(int src_socket, const std::string& dst_ip, int dst_socket, const std::string& message) {
    if (sockets.find(src_socket) != sockets.end()) {
        sockets[src_socket].send(dst_ip, dst_socket, message);
    }
}

std::string MPPNode::recv(int dst_socket) {
    if (fifos.find(dst_socket) != fifos.end()) {
        return fifos[dst_socket].dequeue();
    }
    return "";
}

void MPPNode::listenerThread(int socket) {
    while (running.load()) {
        std::string message = sockets[socket].receive();
        if (!message.empty()) {
            fifos[socket].enqueue(message);
        }
    }
    std::cout << "[INFO] Listener thread on port " << socket << " exiting.\n";
}
