#include "UDPSocket.hpp"
#include "MPPNode.hpp"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

UDPSocket::UDPSocket(int port) {
    int existingSockfd = MPPNode::getInstance().getSockfdForPort(port);
    if (existingSockfd != -1) {
        sockfd = existingSockfd;
        return;
    }

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

    struct timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
}

UDPSocket::~UDPSocket() {
    if (sockfd >= 0) {
        close(sockfd);
    }
}

UDPSocket::UDPSocket(UDPSocket&& other) noexcept 
    : sockfd(other.sockfd), addr(other.addr) {
    other.sockfd = -1;
}

UDPSocket& UDPSocket::operator=(UDPSocket&& other) noexcept {
    if (this != &other) {
        if (sockfd >= 0) {
            close(sockfd);
        }
        sockfd = other.sockfd;
        addr = other.addr;
        other.sockfd = -1;
    }
    return *this;
}

void UDPSocket::send(const std::string& dst_ip, int dst_port, const std::string& message) {
    struct sockaddr_in dest;
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(dst_port);
    inet_pton(AF_INET, dst_ip.c_str(), &dest.sin_addr);

    sendto(sockfd, message.c_str(), message.size(), 0, 
           (struct sockaddr*)&dest, sizeof(dest));
}

std::string UDPSocket::receive() {  // FIX: No parameters
    char buffer[1024];
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    ssize_t n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, 
                         (struct sockaddr*)&cliaddr, &len);

    if (n < 0) {
        return "";
    }

    buffer[n] = '\0';
    return std::string(buffer);
}

int UDPSocket::getSocket() const {
    return sockfd;
}

