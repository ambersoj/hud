#include "UDPChannel.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

UDPChannel::UDPChannel(int port) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "[DEBUG] Created UDPChannel on port " << port << " with sockfd " << sockfd << std::endl;
}

UDPChannel::~UDPChannel() {
    if (sockfd >= 0) {
        close(sockfd);
        std::cout << "[DEBUG] Closed socket " << sockfd << std::endl;
    }
}

// Move constructor
UDPChannel::UDPChannel(UDPChannel&& other) noexcept 
    : sockfd(other.sockfd), addr(other.addr) {
    other.sockfd = -1; // Invalidate old socket
}

// Move assignment operator
UDPChannel& UDPChannel::operator=(UDPChannel&& other) noexcept {
    if (this != &other) {
        // Close current socket if valid
        if (sockfd >= 0) {
            close(sockfd);
        }

        sockfd = other.sockfd;
        addr = other.addr;
        other.sockfd = -1; // Invalidate old socket
    }
    return *this;
}

void UDPChannel::send(const std::string& ip, int port, const std::string& message) {
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &dest.sin_addr);

    sendto(sockfd, message.c_str(), message.size(), 0, 
           (struct sockaddr*)&dest, sizeof(dest));
}

std::string UDPChannel::receive() {
    char buffer[1024] = {0};
    struct sockaddr_in sender;
    socklen_t senderLen = sizeof(sender);
    
    ssize_t bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT, 
                                     (struct sockaddr*)&sender, &senderLen);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        return std::string(buffer);
    }
    return "";
}
