#ifndef MPPSOCKET_HPP
#define MPPSOCKET_HPP

#include <unordered_map>
#include "Hud.hpp"
#include "UDPSocket.hpp"
#include "CommandInvoker.hpp"

class Hud;

class MPPNode {
    private:
        CommandInvoker* invoker;
        Hud* hud;
    protected:
        std::atomic<bool> running;
        std::unordered_map<int, UDPSocket> sockets;
        std::unordered_map<int, UDPSocket*> commandListeners;
    
    public:
        MPPNode();
        virtual ~MPPNode() = default;
    
        void start();
        void run();
        void stop();
        void addCommandListener(int port);
        void removeCommandListener(int port);
        std::unordered_map<int, UDPSocket>& getSockets();
        int getSockfdForPort(int port);
        CommandInvoker& getInvoker();
    
        static MPPNode& getInstance();
    };

#endif
