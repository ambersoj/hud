#ifndef UDP_CHANNEL_HPP
#define UDP_CHANNEL_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class UDPChannel {
private:
    int sockfd;
    struct sockaddr_in addr;

public:
    UDPChannel() = default;
    UDPChannel(int port);
    ~UDPChannel();

    // Delete copy constructor & assignment operator (prevents multiple instances sharing the same socket)
    UDPChannel(const UDPChannel&) = delete;
    UDPChannel& operator=(const UDPChannel&) = delete;

    // Implement move constructor
    UDPChannel(UDPChannel&& other) noexcept;

    // Implement move assignment operator
    UDPChannel& operator=(UDPChannel&& other) noexcept;

    void send(const std::string& ip, int port, const std::string& message);
    std::string receive();
    
    int getSocket() const { return sockfd; } // For debugging
};

#endif // UDP_CHANNEL_HPP
