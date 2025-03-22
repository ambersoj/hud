#ifndef UDP_SOCKET_HPP
#define UDP_SOCKET_HPP

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class UDPSocket {
private:
    int sockfd;
    sockaddr_in addr;

public:
    UDPSocket() : UDPSocket(0) {}
    UDPSocket(int port);
    ~UDPSocket();

    // Delete copy constructor & assignment operator (prevents multiple instances sharing the same socket)
    UDPSocket(const UDPSocket&) = delete;
    UDPSocket& operator=(const UDPSocket&) = delete;

    // Implement move constructor
    UDPSocket(UDPSocket&& other) noexcept;

    // Implement move assignment operator
    UDPSocket& operator=(UDPSocket&& other) noexcept;

    int getSocket() const;
    
    // ✅ FIX: Keep original function names and correct parameter order
    void send(const std::string& dst_ip, int dst_port, const std::string& message);
    std::string receive();  // ✅ FIX: No parameters, matching RecvCommand::execute()
};

#endif // UDP_SOCKET_HPP
