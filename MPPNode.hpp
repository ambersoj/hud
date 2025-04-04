#ifndef MPPNODE_HPP
#define MPPNODE_HPP

#include "UDPSocket.hpp"
#include "MessageFIFO.hpp"
#include <unordered_map>
#include <thread>
#include <vector>
#include <atomic>

class MPPNode {
public:
    static MPPNode& getInstance();
    void send(int src_socket, const std::string& dst_ip, int dst_socket, const std::string& message);
    std::string recv(int dst_socket);
    void stop();

private:
    MPPNode();
    ~MPPNode();
    void listenerThread(int socket);

    std::unordered_map<int, UDPSocket> sockets;
    std::unordered_map<int, MessageFIFO> fifos;
    std::vector<std::thread> threads;

    std::atomic<bool> running;
};

#endif
