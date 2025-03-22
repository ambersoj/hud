#ifndef MPPSOCKET_HPP
#define MPPSOCKET_HPP

#include "CommandInvoker.hpp"
#include "UDPSocket.hpp"
#include <unordered_map>
#include "globals.hpp"

class MPPNode {
    private:
        CommandInvoker* invoker;
    protected:
        std::atomic<bool> running;
        std::unordered_map<int, UDPSocket> sockets;
        std::unordered_map<int, UDPSocket*> commandListeners;
    
    public:
        MPPNode();
        virtual ~MPPNode() = default;
    
        void start(CommandInvoker& invoker);
        void run();
        void stop();
        void addCommandListener(int port);
        void removeCommandListener(int port);
        std::unordered_map<int, UDPSocket>& getSockets();
        int getSockfdForPort(int port);
    
        static MPPNode& getInstance();
    };

#endif
