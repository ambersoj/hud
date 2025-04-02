#ifndef MPPNODE_HPP
#define MPPNODE_HPP

#include <unordered_map>
#include "UDPSocket.hpp"
#include "CommandInvoker.hpp"

class MPPNode {
private:
    std::unordered_map<int, UDPSocket> sockets;
    CommandInvoker invoker;

public:
    MPPNode();
    ~MPPNode() = default;

    std::unordered_map<int, UDPSocket>& getSockets();
    CommandInvoker& getInvoker();

    static MPPNode& getInstance();
};

#endif
