Hey partner, sometimes I lose track of just how much freedom we have here.  I've just realized that the same way mock messages can be sent from net to cnl with the repl mock messages can be send by the cnl to the hud.  So let's just do that for now and we won't have to worry about net at all for now.

I put in a Hud::receive_msg_from_cnl() method as you can see.  I've done it terribly wrong though.  Can you do it a better way please?  The purpose of it is to programatically, instead of at the command line, issue a recv command and receive the json bearing message that I'll the repl in cnl to send to the hud.

Can you please provide me with the proper >send command that I'll need to issue from cnl to get the spinner status json message to the hud?  This message has to go from a src of CNL_HUD_DTE to a dst of HUD_CNL_DCE.

We're getting pretty close, for sure.  It's all compiling now so it's wired up right so once we straighten out these methods and the logic we'll be up and running with this first little itegration test between cnl and hud.

// Hud.hpp
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <jsoncpp/json/json.h>

class Hud {
private:
    std::thread hudThread;
    bool spinner_on;
public:
    Hud();
    std::string receive_msg_from_cnl();
    void run_spinner();
    void run();
    void start();
    void stop();
};

// Hud.cpp
#include "Hud.hpp"

std::atomic<bool> running(true);

// Hud Component
Hud::Hud() : spinner_on(false) {}

void Hud::run() {
    while (running) {
        std::string msg = receive_msg_from_cnl(); // Placeholder function
        if (!msg.empty()) {
            Json::CharReaderBuilder reader;
            Json::Value parsed;
            std::string errs;
            std::istringstream s(msg);

            if (Json::parseFromStream(reader, s, &parsed, &errs)) {
                if (parsed.isMember("spinner_on")) {
                    spinner_on = parsed["spinner_on"].asBool();
                }
            }
        }
        run_spinner();
    }
}

std::string Hud::receive_msg_from_cnl() {
    ste::string msg = "";

    msg = invoker.addCommand(new RecvCommand(channels[HUD_CNL_DCE])); // port 6003
    
    return "msg";
}

void Hud::run_spinner() {
    if (spinner_on) {
        static int state = 0;
        const char spinner_chars[] = {'/', '-', '\\', '|'};
        std::cout << "Spinner: " << spinner_chars[state] << "\r";
        std::cout.flush();
        state = (state + 1) % 4;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void Hud::start() {
    hudThread = std::thread(&Hud::run, this);
}

void Hud::stop() {
    running.store(false);
    if (hudThread.joinable()) {
        hudThread.join();
    }
}

// Cnl.hpp
#include <iostream>
#include <thread>
#include <atomic>
#include <string>

#include <jsoncpp/json/json.h>

class Cnl {
private:
    std::thread cnlThread;

public:
    Cnl();
    void send_to_hud(const std::string& message);
    std::string receive_msg_from_net();
    void run();
    void start();
    void stop();
};

// Cnl.cpp
#include "Cnl.hpp"

std::atomic<bool> running(true);

Cnl::Cnl() {}

void Cnl::run() {
    while (running) {
        std::string msg = receive_msg_from_net(); // Placeholder function
        if (!msg.empty()) {
            Json::Value output;
            if (msg == "trigger 1") {
                output["spinner_on"] = true;
            } else if (msg == "trigger 2") {
                output["spinner_on"] = false;
            } else if (msg == "exit") {
                running = false;
            }
            Json::StreamWriterBuilder writer;
            std::string jsonMessage = Json::writeString(writer, output);
            send_to_hud(jsonMessage);
        }
    }
}

void Cnl::send_to_hud(const std::string& message) {
    // Placeholder function to send JSON message to HUD_CNL_DCE
    std::cout << "Cnl -> Hud: " << message << std::endl;
}

std::string Cnl::receive_msg_from_net() {
    // Placeholder
    return "";
}

void Cnl::start() {
    cnlThread = std::thread(&Cnl::run, this);
}

void Cnl::stop() {
    running.store(false);
    if (cnlThread.joinable()) {
        cnlThread.join();
    }
}

/////////////////////////////////////////////

I see we have an issue here.  You see hud and cnl are completely different programs, so for instance I can't include Hud.hpp in Cnl.hpp.  So solutions for issues will have to use the UDP communications channels between components but not sharing code.

//////////////////////////////////////////////

Ok, partner.  I'm sorry I got a bit confused for a while.  Now I see that we shouldn't be working on cnl at this time at all.  I'm going to use the repl of cnl for now just to send a mock message to hud.  So we don't need any methods or anything at all for Cmd.cpp nore Cmd.hpp and I'll empty them out so that when it comes time for us to work on cnl it'll be clean and fresh with just the bare run(), start() and stop() methods.

The hud is the only component that we need to worry about coding for now.

Hud::run() doesn't appear to be called.  Should it be called in main()?

Just so you know, there's no need for mock messages of this sort:

void Hud::receive_mock_message(const std::string& msg) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push(msg);
}

std::string Hud::receive_msg_from_cnl() {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (!messageQueue.empty()) {
        std::string msg = messageQueue.front();
        messageQueue.pop();
        return msg;
    }
    return "";
}

I'll send a real message from cnl.  I'll use >send and I'll use the src CNL_HUD_DTE and dst HUD_CNL_DCE from the repl.  I'd like you to provide me with the command to enter.  It can't be this:

send_to_hud(R"({"src": "CNL_HUD_DTE", "dst": "HUD_CNL_DCE", "spinner_on": true})");

because I have to enter it in cnl at the '>' prompt.  It has to be a >send command.

So, good news, we only have to worry about hud for now.

/////////////////////////////////////////

The comms look good on the cnl side.  When cnl executes this:

    invoker.addCommand(new SendCommand(channels[CNL_HUD_DTE], IP_ADDRESS, HUD_CNL_DCE, "{\"spinner_on\": true}")); 

hud receives it properly.  This is the output of the repl in hud:

> recv 6003
Received: {"spinner_on": true}

And when I enter this in cnl by keyboard:

send 7002 127.0.0.1 6003 {"spinner_on": true}

hud received it properly.  This is the output of the repl in hud:

> recv 6003                                    
Received: {"spinner_on": true}

Hud still has the obsolete "mock" methods in it.  They need to be removed and instead hud has to use it's recv HUD_CNL_DCE command programatically somehow in the Hud::receive_msg_from_cnl() method.  Not this stuff:

void Hud::receive_mock_message(const std::string& msg) {
    std::lock_guard<std::mutex> lock(queueMutex);
    messageQueue.push(msg);
}

std::string Hud::receive_msg_from_cnl() {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (!messageQueue.empty()) {
        std::string msg = messageQueue.front();
        messageQueue.pop();
        return msg;
    }
    return "";
}

The receive_msg_from_cnl() method should look something like:

std::string Hud::receive_msg_from_cnl() {
    msg = addCommand(recv 6003); // or recv HUD_CNL_DCE

    return msg;
}

It's all coming along nicely.  Good work, partner.

/////////////////////////////////////////

Yeah, as I expected there were errors such as these:

root@PRED:/usr/local/mpp/hud# g++ -o hud main.cpp Hud.cpp -ljsoncpp -pthread
Hud.cpp: In member function ‘std::string Hud::receive_msg_from_cnl()’:
Hud.cpp:9:12: error: ‘invoker’ was not declared in this scope
    9 |     return invoker.addCommand(new RecvCommand(channels[HUD_CNL_DCE])); // UDP port 6003
      |            ^~~~~~~
Hud.cpp:9:35: error: expected type-specifier before ‘RecvCommand’
    9 |     return invoker.addCommand(new RecvCommand(channels[HUD_CNL_DCE])); // UDP port 6003
      |                                   ^~~~~~~~~~~
root@PRED:/usr/local/mpp/hud# 

What I like about you is that you're good at figuring out how to wire things up when referenced cross class and file boundaries.  Do you think you can modify the code to get the recv command called programatically from hud?  If so I'd be grateful if you did, partner.

////////////////////////////////////////////////////////

At this moment these:

#include "CommandInvoker.hpp"  // Ensure we have access to the command system
#include "RecvCommand.hpp"     // Include the actual RecvCommand class

are defined in main.cpp, remember?  Would it be possible for me to reference them from there or will I have to split CommandInvoker and RecvCommand out of main.cpp?

The run() method called in main is called like this:

    run(channels, invoker);

Here's a modified main():

int main() {

    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;
    for (int i = HUD_PORT_BASE; i < HUD_PORT_BASE + 6; ++i) {
        channels.emplace(i, UDPChannel(IP_ADDRESS, i));
    }
    run(channels, invoker);

    Hud hud;
    hud.start(channels, invoker);

    hud.stop();
    return 0;
}

I've modified some of the hud code to express my vague ideas of how to get the references need by hud into hud.  Please look it over and correct.

// Hud.hpp
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <jsoncpp/json/json.h>
#include <queue>
#include <mutex>

class Hud {
private:
    std::thread hudThread;
    bool spinner_on;
    std::queue<std::string> messageQueue; // Mock message queue
    std::mutex queueMutex; // Protects messageQueue
    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;
    
public:
    Hud();
    void receive_mock_message(const std::string& msg); // Add this
    std::string receive_msg_from_cnl();
    void run_spinner();
    void run();
    void start(std::unordered_map<int, UDPChannel> &channels, CommandInvoker &invoker);
    void stop();
};

// Hud.cpp
#include "Hud.hpp"
#include <unordered_map>

std::atomic<bool> running(true);

// Hud Component
Hud::Hud() : spinner_on(false) {}

std::string Hud::receive_msg_from_cnl() {
    return invoker.addCommand(new RecvCommand(channels[HUD_CNL_DCE])); // UDP port 6003
}

void Hud::run() {
    while (running) {
        std::string msg = receive_msg_from_cnl();
        if (!msg.empty()) {
            Json::CharReaderBuilder reader;
            Json::Value parsed;
            std::string errs;
            std::istringstream s(msg);

            if (Json::parseFromStream(reader, s, &parsed, &errs)) {
                if (parsed.isMember("spinner_on")) {
                    spinner_on = parsed["spinner_on"].asBool();
                }
            }
        }
        run_spinner();
    }
}

void Hud::run_spinner() {
    if (spinner_on) {
        static int state = 0;
        const char spinner_chars[] = {'/', '-', '\\', '|'};
        std::cout << "Spinner: " << spinner_chars[state] << "\r";
        std::cout.flush();
        state = (state + 1) % 4;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void Hud::start(std::unordered_map<int, UDPChannel> &channels, CommandInvoker &invoker) {
    channels = channels; // get channels into hud
    invoker = invoker; // get invoker into hud
    hudThread = std::thread(&Hud::run, this);
}

void Hud::stop() {
    running.store(false);
    if (hudThread.joinable()) {
        hudThread.join();
    }
}

//////////////////////////////////////////

The hud code wants these:

#include "CommandInvoker.hpp"
#include "RecvCommand.hpp"
#include "UDPChannel.hpp"

but the problem is that they are defined in main.cpp still and they aren't in their own .cpp and .hpp files.  Can they be reference from hud in main.cpp?

//////////////////////////////////////////////

This is main.cpp right now:

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <queue>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <cstdlib>
#include "mpp_defines.hpp"
#include "Hud.hpp"

class UDPChannel
{
public:
    UDPChannel() = default;
    UDPChannel(const std::string &ip, int port) : ip(ip), port(port)
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0)
        {
            perror("Socket creation failed");
            exit(1);
        }
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.c_str());
        addr.sin_port = htons(port);
        if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            perror("Bind failed");
            exit(1);
        }
        std::cout << "UDPChannel bound to " << ip << ":" << port << " with sockfd " << sockfd << std::endl;
    }

    UDPChannel(const UDPChannel &) = delete;
    UDPChannel &operator=(const UDPChannel &) = delete;

    UDPChannel(UDPChannel &&other) noexcept
        : sockfd(other.sockfd), ip(std::move(other.ip)), port(other.port)
    {
        other.sockfd = -1;
    }

    UDPChannel &operator=(UDPChannel &&other) noexcept
    {
        if (this != &other)
        {
            close(sockfd);
            sockfd = other.sockfd;
            ip = std::move(other.ip);
            port = other.port;
            other.sockfd = -1;
        }
        return *this;
    }

    void send(const std::string &dst_ip, int dst_port, const std::string &message)
    {
        if (sockfd < 0)
        {
            std::cerr << "Socket not initialized properly!" << std::endl;
            return;
        }
        sockaddr_in dest{};
        dest.sin_family = AF_INET;
        dest.sin_addr.s_addr = inet_addr(dst_ip.c_str());
        dest.sin_port = htons(dst_port);
        std::cout << "Sending to " << dst_ip << ":" << dst_port << " via socket " << sockfd << std::endl;
        ssize_t bytes_sent = sendto(sockfd, message.c_str(), message.size(), 0, (struct sockaddr *)&dest, sizeof(dest));
        if (bytes_sent < 0)
        {
            perror("sendto failed");
        }
    }

    std::string recv()
    {
        char buffer[1024] = {0};
        sockaddr_in sender{};
        socklen_t sender_len = sizeof(sender);
        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT, (struct sockaddr *)&sender, &sender_len);
        if (len > 0)
        {
            buffer[len] = '\0';
            return std::string(buffer);
        }
        return "";
    }

    ~UDPChannel()
    {
        if (sockfd >= 0)
        {
            close(sockfd);
        }
    }

private:
    int sockfd;
    std::string ip;
    int port;
};

class Command
{
public:
    virtual void execute() = 0;
    virtual ~Command() = default;
};

class SendCommand : public Command
{
public:
    SendCommand(UDPChannel &channel, const std::string &dst_ip, int dst_port, const std::string &message)
        : channel(channel), dst_ip(dst_ip), dst_port(dst_port), message(message) {}

    void execute() override { channel.send(dst_ip, dst_port, message); }

private:
    UDPChannel &channel;
    std::string dst_ip;
    int dst_port;
    std::string message;
};

class RecvCommand : public Command
{
public:
    RecvCommand(UDPChannel &channel) : channel(channel) {}

    void execute() override
    {
        std::string msg = channel.recv();
        if (!msg.empty())
            std::cout << "Received: " << msg << std::endl;
        else
            std::cout << "No data available" << std::endl;
    }

private:
    UDPChannel &channel;
};

class CommandInvoker
{
public:
    void addCommand(Command *cmd)
    {
        commandQueue.push(cmd);
    }

    void executeCommands()
    {
        while (!commandQueue.empty())
        {
            Command *cmd = commandQueue.front();
            commandQueue.pop();
            cmd->execute();
            std::cout << "> ";
            delete cmd;
        }
    }

private:
    std::queue<Command *> commandQueue;
};

void run(std::unordered_map<int, UDPChannel> &channels, CommandInvoker &invoker)
{
    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    std::cout << "> ";
    while (true)
    {
        std::cout.flush();
        invoker.executeCommands();
        if (poll(&pfd, 1, 100) > 0)
        {
            std::string line;
            std::getline(std::cin, line);
            if (line == "exit")
                break;

            std::vector<std::string> tokens;
            std::istringstream iss(line);
            std::string token;
            while (iss >> token)
                tokens.push_back(token);

            if (tokens.empty())
                continue;
            if (tokens[0] == "send" && tokens.size() >= 5)
            {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end())
                {
                    int dst_port = std::stoi(tokens[3]);
                    std::string message = line.substr(line.find(tokens[4]));
                    invoker.addCommand(new SendCommand(channels[ch], tokens[2], dst_port, message));
                }
                else
                {
                    std::cerr << "Invalid channel: " << ch << std::endl;
                }
            }
            else if (tokens[0] == "recv" && tokens.size() == 2)
            {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end())
                {
                    invoker.addCommand(new RecvCommand(channels[ch]));
                }
                else
                {
                    std::cerr << "Invalid channel: " << ch << std::endl;
                }
            }
            else
            {
                std::cout << "Invalid command" << std::endl;
            }
        }
    }
}

int main() {
    Hud hud;
    hud.start();

    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;
    for (int i = HUD_PORT_BASE; i < HUD_PORT_BASE + 6; ++i) {
        channels.emplace(i, UDPChannel(IP_ADDRESS, i));
    }
    run(channels, invoker);

    hud.stop();
    return 0;
}

But lots of that stuff is now in other files.  Could you please redo main.cpp?

///////////////////////////////////////

Oops, missing SendCommand.hpp.  I remember seeing it in main and not in UDPChannel, where there is a receive() but I forgot to ask you about it.  It looks like send got deleted along with everything else in main.

Thanks for all the work.  It's looking really good.  We should be watching spinners here pretty soon partner!

////////////////////////////////////////

There is no:

        invoker.executeCommands();

like at the top of run().

CommandInvoker.hpp shows that there is now executeCommands() method.  I guess like send it never survived the splitting apart.  Here's how CommandInvoker.hpp is right now:

#ifndef COMMAND_INVOKER_HPP
#define COMMAND_INVOKER_HPP

#include "Command.hpp"
#include <memory>

class CommandInvoker {
public:
    std::string addCommand(std::unique_ptr<Command> command);
};

#endif // COMMAND_INVOKER_HPP

////////////////////////////////////////

There's this code in SendCommand.cpp:

void SendCommand::execute() {
    channel.send(dst_ip, dst_port, message);
}

yet when I look at UDPChannel.hpp I don't see a method UDPChannel::send():

class UDPChannel {
private:
    std::string ip;
    int port;
public:
    UDPChannel(const std::string& ip, int port);
    std::string receive();
};

Just a receive() method, but no send().

/////////////////////////////////////

I'm sorry to dump all of this compiler output on you but I've got a feeling you might be able to make sense of it where I can't:

root@PRED:/usr/local/mpp/hud# g++ -o hud main.cpp Hud.cpp UDPChannel.cpp CommandInvoker.cpp RecvCommand.cpp SendCommand.cpp -ljsoncpp -pthread
In file included from Hud.hpp:14,
                 from main.cpp:9:
RecvCommand.hpp:12:17: error: conflicting return type specified for ‘virtual std::string RecvCommand::execute()’
   12 |     std::string execute() override;
      |                 ^~~~~~~
In file included from CommandInvoker.hpp:4,
                 from Hud.hpp:13:
Command.hpp:7:18: note: overridden function is ‘virtual void Command::execute()’
    7 |     virtual void execute() = 0;
      |                  ^~~~~~~
In file included from /usr/include/c++/13/bits/memory_resource.h:47,
                 from /usr/include/c++/13/string:58,
                 from /usr/include/c++/13/bits/locale_classes.h:40,
                 from /usr/include/c++/13/bits/ios_base.h:41,
                 from /usr/include/c++/13/ios:44,
                 from /usr/include/c++/13/ostream:40,
                 from /usr/include/c++/13/iostream:41,
                 from main.cpp:1:
/usr/include/c++/13/tuple: In instantiation of ‘std::pair<_T1, _T2>::pair(std::tuple<_Args1 ...>&, std::tuple<_Args2 ...>&, std::_Index_tuple<_Indexes1 ...>, std::_Index_tuple<_Indexes2 ...>) [with _Args1 = {const int&}; long unsigned int ..._Indexes1 = {0}; _Args2 = {}; long unsigned int ..._Indexes2 = {}; _T1 = const int; _T2 = UDPChannel]’:
/usr/include/c++/13/tuple:2257:63:   required from ‘std::pair<_T1, _T2>::pair(std::piecewise_construct_t, std::tuple<_Args1 ...>, std::tuple<_Args2 ...>) [with _Args1 = {const int&}; _Args2 = {}; _T1 = const int; _T2 = UDPChannel]’
/usr/include/c++/13/bits/new_allocator.h:191:4:   required from ‘void std::__new_allocator<_Tp>::construct(_Up*, _Args&& ...) [with _Up = std::pair<const int, UDPChannel>; _Args = {const std::piecewise_construct_t&, std::tuple<const int&>, std::tuple<>}; _Tp = std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false>]’
/usr/include/c++/13/bits/alloc_traits.h:538:17:   required from ‘static void std::allocator_traits<std::allocator<_CharT> >::construct(allocator_type&, _Up*, _Args&& ...) [with _Up = std::pair<const int, UDPChannel>; _Args = {const std::piecewise_construct_t&, std::tuple<const int&>, std::tuple<>}; _Tp = std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false>; allocator_type = std::allocator<std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false> >]’
/usr/include/c++/13/bits/hashtable_policy.h:1995:36:   required from ‘std::__detail::_Hashtable_alloc<_NodeAlloc>::__node_type* std::__detail::_Hashtable_alloc<_NodeAlloc>::_M_allocate_node(_Args&& ...) [with _Args = {const std::piecewise_construct_t&, std::tuple<const int&>, std::tuple<>}; _NodeAlloc = std::allocator<std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false> >; __node_ptr = std::allocator<std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false> >::value_type*]’
/usr/include/c++/13/bits/hashtable.h:307:35:   required from ‘std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::_Scoped_node::_Scoped_node(std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::__hashtable_alloc*, _Args&& ...) [with _Args = {const std::piecewise_construct_t&, std::tuple<const int&>, std::tuple<>}; _Key = int; _Value = std::pair<const int, UDPChannel>; _Alloc = std::allocator<std::pair<const int, UDPChannel> >; _ExtractKey = std::__detail::_Select1st; _Equal = std::equal_to<int>; _Hash = std::hash<int>; _RangeHash = std::__detail::_Mod_range_hashing; _Unused = std::__detail::_Default_ranged_hash; _RehashPolicy = std::__detail::_Prime_rehash_policy; _Traits = std::__detail::_Hashtable_traits<false, false, true>; std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::__hashtable_alloc = std::_Hashtable<int, std::pair<const int, UDPChannel>, std::allocator<std::pair<const int, UDPChannel> >, std::__detail::_Select1st, std::equal_to<int>, std::hash<int>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::__hashtable_alloc]’
/usr/include/c++/13/bits/hashtable_policy.h:818:42:   required from ‘std::__detail::_Map_base<_Key, std::pair<const _Key, _Val>, _Alloc, std::__detail::_Select1st, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits, true>::mapped_type& std::__detail::_Map_base<_Key, std::pair<const _Key, _Val>, _Alloc, std::__detail::_Select1st, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits, true>::operator[](const key_type&) [with _Key = int; _Val = UDPChannel; _Alloc = std::allocator<std::pair<const int, UDPChannel> >; _Equal = std::equal_to<int>; _Hash = std::hash<int>; _RangeHash = std::__detail::_Mod_range_hashing; _Unused = std::__detail::_Default_ranged_hash; _RehashPolicy = std::__detail::_Prime_rehash_policy; _Traits = std::__detail::_Hashtable_traits<false, false, true>; mapped_type = UDPChannel; key_type = int]’
/usr/include/c++/13/bits/unordered_map.h:987:20:   required from ‘std::unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>::mapped_type& std::unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>::operator[](const key_type&) [with _Key = int; _Tp = UDPChannel; _Hash = std::hash<int>; _Pred = std::equal_to<int>; _Alloc = std::allocator<std::pair<const int, UDPChannel> >; mapped_type = UDPChannel; key_type = int]’
main.cpp:43:81:   required from here
/usr/include/c++/13/tuple:2268:9: error: no matching function for call to ‘UDPChannel::UDPChannel()’
 2268 |         second(std::forward<_Args2>(std::get<_Indexes2>(__tuple2))...)
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from RecvCommand.hpp:5:
UDPChannel.hpp:13:5: note: candidate: ‘UDPChannel::UDPChannel(const std::string&, int)’
   13 |     UDPChannel(const std::string& ip, int port);
      |     ^~~~~~~~~~
UDPChannel.hpp:13:5: note:   candidate expects 2 arguments, 0 provided
UDPChannel.hpp:6:7: note: candidate: ‘UDPChannel::UDPChannel(const UDPChannel&)’
    6 | class UDPChannel {
      |       ^~~~~~~~~~
UDPChannel.hpp:6:7: note:   candidate expects 1 argument, 0 provided
In file included from Hud.hpp:14,
                 from Hud.cpp:1:
RecvCommand.hpp:12:17: error: conflicting return type specified for ‘virtual std::string RecvCommand::execute()’
   12 |     std::string execute() override;
      |                 ^~~~~~~
In file included from CommandInvoker.hpp:4,
                 from Hud.hpp:13:
Command.hpp:7:18: note: overridden function is ‘virtual void Command::execute()’
    7 |     virtual void execute() = 0;
      |                  ^~~~~~~
Hud.cpp: In member function ‘std::string Hud::receive_msg_from_cnl()’:
Hud.cpp:19:32: error: cannot convert ‘RecvCommand*’ to ‘std::unique_ptr<Command>’
   19 |     return invoker->addCommand(new RecvCommand((*channels)[HUD_CNL_DCE])); // Use pointer correctly
      |                                ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      |                                |
      |                                RecvCommand*
CommandInvoker.hpp:10:53: note:   initializing argument 1 of ‘std::string CommandInvoker::addCommand(std::unique_ptr<Command>)’
   10 |     std::string addCommand(std::unique_ptr<Command> command);
      |                            ~~~~~~~~~~~~~~~~~~~~~~~~~^~~~~~~
In file included from /usr/include/c++/13/bits/memory_resource.h:47,
                 from /usr/include/c++/13/string:58,
                 from /usr/include/c++/13/bits/locale_classes.h:40,
                 from /usr/include/c++/13/bits/ios_base.h:41,
                 from /usr/include/c++/13/ios:44,
                 from /usr/include/c++/13/ostream:40,
                 from /usr/include/c++/13/iostream:41,
                 from Hud.hpp:5:
/usr/include/c++/13/tuple: In instantiation of ‘std::pair<_T1, _T2>::pair(std::tuple<_Args1 ...>&, std::tuple<_Args2 ...>&, std::_Index_tuple<_Indexes1 ...>, std::_Index_tuple<_Indexes2 ...>) [with _Args1 = {int&&}; long unsigned int ..._Indexes1 = {0}; _Args2 = {}; long unsigned int ..._Indexes2 = {}; _T1 = const int; _T2 = UDPChannel]’:
/usr/include/c++/13/tuple:2257:63:   required from ‘std::pair<_T1, _T2>::pair(std::piecewise_construct_t, std::tuple<_Args1 ...>, std::tuple<_Args2 ...>) [with _Args1 = {int&&}; _Args2 = {}; _T1 = const int; _T2 = UDPChannel]’
/usr/include/c++/13/bits/new_allocator.h:191:4:   required from ‘void std::__new_allocator<_Tp>::construct(_Up*, _Args&& ...) [with _Up = std::pair<const int, UDPChannel>; _Args = {const std::piecewise_construct_t&, std::tuple<int&&>, std::tuple<>}; _Tp = std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false>]’
/usr/include/c++/13/bits/alloc_traits.h:538:17:   required from ‘static void std::allocator_traits<std::allocator<_CharT> >::construct(allocator_type&, _Up*, _Args&& ...) [with _Up = std::pair<const int, UDPChannel>; _Args = {const std::piecewise_construct_t&, std::tuple<int&&>, std::tuple<>}; _Tp = std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false>; allocator_type = std::allocator<std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false> >]’
/usr/include/c++/13/bits/hashtable_policy.h:1995:36:   required from ‘std::__detail::_Hashtable_alloc<_NodeAlloc>::__node_type* std::__detail::_Hashtable_alloc<_NodeAlloc>::_M_allocate_node(_Args&& ...) [with _Args = {const std::piecewise_construct_t&, std::tuple<int&&>, std::tuple<>}; _NodeAlloc = std::allocator<std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false> >; __node_ptr = std::allocator<std::__detail::_Hash_node<std::pair<const int, UDPChannel>, false> >::value_type*]’
/usr/include/c++/13/bits/hashtable.h:307:35:   required from ‘std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::_Scoped_node::_Scoped_node(std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::__hashtable_alloc*, _Args&& ...) [with _Args = {const std::piecewise_construct_t&, std::tuple<int&&>, std::tuple<>}; _Key = int; _Value = std::pair<const int, UDPChannel>; _Alloc = std::allocator<std::pair<const int, UDPChannel> >; _ExtractKey = std::__detail::_Select1st; _Equal = std::equal_to<int>; _Hash = std::hash<int>; _RangeHash = std::__detail::_Mod_range_hashing; _Unused = std::__detail::_Default_ranged_hash; _RehashPolicy = std::__detail::_Prime_rehash_policy; _Traits = std::__detail::_Hashtable_traits<false, false, true>; std::_Hashtable<_Key, _Value, _Alloc, _ExtractKey, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits>::__hashtable_alloc = std::_Hashtable<int, std::pair<const int, UDPChannel>, std::allocator<std::pair<const int, UDPChannel> >, std::__detail::_Select1st, std::equal_to<int>, std::hash<int>, std::__detail::_Mod_range_hashing, std::__detail::_Default_ranged_hash, std::__detail::_Prime_rehash_policy, std::__detail::_Hashtable_traits<false, false, true> >::__hashtable_alloc]’
/usr/include/c++/13/bits/hashtable_policy.h:845:42:   required from ‘std::__detail::_Map_base<_Key, std::pair<const _Key, _Tp>, _Alloc, std::__detail::_Select1st, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits, true>::mapped_type& std::__detail::_Map_base<_Key, std::pair<const _Key, _Tp>, _Alloc, std::__detail::_Select1st, _Equal, _Hash, _RangeHash, _Unused, _RehashPolicy, _Traits, true>::operator[](key_type&&) [with _Key = int; _Val = UDPChannel; _Alloc = std::allocator<std::pair<const int, UDPChannel> >; _Equal = std::equal_to<int>; _Hash = std::hash<int>; _RangeHash = std::__detail::_Mod_range_hashing; _Unused = std::__detail::_Default_ranged_hash; _RehashPolicy = std::__detail::_Prime_rehash_policy; _Traits = std::__detail::_Hashtable_traits<false, false, true>; mapped_type = UDPChannel; key_type = int]’
/usr/include/c++/13/bits/unordered_map.h:991:20:   required from ‘std::unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>::mapped_type& std::unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>::operator[](key_type&&) [with _Key = int; _Tp = UDPChannel; _Hash = std::hash<int>; _Pred = std::equal_to<int>; _Alloc = std::allocator<std::pair<const int, UDPChannel> >; mapped_type = UDPChannel; key_type = int]’
Hud.cpp:19:71:   required from here
/usr/include/c++/13/tuple:2268:9: error: no matching function for call to ‘UDPChannel::UDPChannel()’
 2268 |         second(std::forward<_Args2>(std::get<_Indexes2>(__tuple2))...)
      |         ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from RecvCommand.hpp:5:
UDPChannel.hpp:13:5: note: candidate: ‘UDPChannel::UDPChannel(const std::string&, int)’
   13 |     UDPChannel(const std::string& ip, int port);
      |     ^~~~~~~~~~
UDPChannel.hpp:13:5: note:   candidate expects 2 arguments, 0 provided
UDPChannel.hpp:6:7: note: candidate: ‘UDPChannel::UDPChannel(const UDPChannel&)’
    6 | class UDPChannel {
      |       ^~~~~~~~~~
UDPChannel.hpp:6:7: note:   candidate expects 1 argument, 0 provided
In file included from RecvCommand.cpp:1:
RecvCommand.hpp:12:17: error: conflicting return type specified for ‘virtual std::string RecvCommand::execute()’
   12 |     std::string execute() override;
      |                 ^~~~~~~
In file included from RecvCommand.hpp:4:
Command.hpp:7:18: note: overridden function is ‘virtual void Command::execute()’
    7 |     virtual void execute() = 0;
      |                  ^~~~~~~

///////////////////////////////////////////////////

It might look like a lot of errors but I'm pretty sure it all stems from some issue with RecvCommand:

root@PRED:/usr/local/mpp/hud# g++ -o hud main.cpp Hud.cpp UDPChannel.cpp CommandInvoker.cpp RecvCommand.cpp SendCommand.cpp -ljsoncpp -pthread
In file included from Hud.hpp:14,
                 from main.cpp:9:
RecvCommand.hpp:12:17: error: conflicting return type specified for ‘virtual std::string RecvCommand::execute()’
   12 |     std::string execute() override;
      |                 ^~~~~~~
In file included from CommandInvoker.hpp:4,
                 from Hud.hpp:13:
Command.hpp:7:18: note: overridden function is ‘virtual void Command::execute()’
    7 |     virtual void execute() = 0;
      |                  ^~~~~~~
In file included from Hud.hpp:14,
                 from Hud.cpp:1:
RecvCommand.hpp:12:17: error: conflicting return type specified for ‘virtual std::string RecvCommand::execute()’
   12 |     std::string execute() override;
      |                 ^~~~~~~
In file included from CommandInvoker.hpp:4,
                 from Hud.hpp:13:
Command.hpp:7:18: note: overridden function is ‘virtual void Command::execute()’
    7 |     virtual void execute() = 0;
      |                  ^~~~~~~
In file included from RecvCommand.cpp:1:
RecvCommand.hpp:12:17: error: conflicting return type specified for ‘virtual std::string RecvCommand::execute()’
   12 |     std::string execute() override;
      |                 ^~~~~~~
In file included from RecvCommand.hpp:4:
Command.hpp:7:18: note: overridden function is ‘virtual void Command::execute()’
    7 |     virtual void execute() = 0;
      |                  ^~~~~~~
RecvCommand.cpp:5:6: error: no declaration matches ‘void RecvCommand::execute()’
    5 | void RecvCommand::execute() {
      |      ^~~~~~~~~~~
RecvCommand.hpp:12:17: note: candidate is: ‘virtual std::string RecvCommand::execute()’
   12 |     std::string execute() override;
      |                 ^~~~~~~
RecvCommand.hpp:7:7: note: ‘class RecvCommand’ defined here
    7 | class RecvCommand : public Command {
      |       ^~~~~~~~~~~
root@PRED:/usr/local/mpp/hud# 

/////////////////////////////////////////

Ok I got hud compiling and I'm stepping through in the debugger.  I'm just a little ways into main and I can see we have an issue with the channels all being assigned the same sockfd.  We had this issue before, if you recall.  When it was fixed the file descriptors tended to be 3, 4, 5, 6, 7 and 8.

Then in main when it executes this line:

        invoker.executeCommands();

the output becomes this:

> > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > 

and the code that does it is this:

void CommandInvoker::executeCommands() {
    while (!commandQueue.empty()) {
        auto cmd = std::move(commandQueue.front());
        commandQueue.pop();
        cmd->execute();
        std::cout << "> ";
    }
}

Does while loop test always return true?

But hey, it's great that it's compiling and running in the debugger.  We'll be seeing spinners in no time!

////////////////////////////////////////////

With the print code added like so:

#include "CommandInvoker.hpp"
#include <iostream>

std::string CommandInvoker::addCommand(std::unique_ptr<Command> command) {
    commandQueue.push(std::move(command));
    return ""; // We can modify this if needed later.
}

void CommandInvoker::executeCommands() {
    while (!commandQueue.empty()) {
        auto cmd = std::move(commandQueue.front());
        std::cout << "Queue size: " << commandQueue.size() << std::endl;

        commandQueue.pop();
        cmd->execute();
        std::cout << "> ";
    }
}

this is the output:

> Queue size: 409439
> Queue size: 409442
> Queue size: 409447
> Queue size: 409452
> Queue size: 409456
> Queue size: 409461
> Queue size: 409466
> Queue size: 409471
> Queue size: 409474
> Queue size: 409479
> Queue size: 409484
> Queue size: 409490

but a lot more...

I also put that other code in and got this:

> > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > 
[ERROR] Infinite loop suspected. Breaking out!

as predicted.

////////////////////////////////////////////////////

Oh wow.  We get this infinitely:

[DEBUG] Adding command: 11RecvCommand | New queue size: 1222570
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222571
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222572
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222573
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222574
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222575
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222576
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222577
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222578
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222579
[DEBUG] Adding command: 11RecvCommand | New queue size: 1222580
and on and on and on...

There was issues with RecvCommand for a while now.  I'll let you look at the code I have to see if you can spot how things went wrong with Command::execute() and RecvCommand and whatever else that is going whacky.

// Command.hpp
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>

class Command
{
public:
virtual std::string execute() = 0;  // Change to std::string
virtual ~Command() = default;
};

#endif

// CommandInvoker.hpp
#ifndef COMMAND_INVOKER_HPP
#define COMMAND_INVOKER_HPP

#include "Command.hpp"
#include <memory>
#include <queue>

class CommandInvoker {
public:
    std::string addCommand(std::unique_ptr<Command> command);
    void executeCommands(); // 🔥 Added this back!

private:
    std::queue<std::unique_ptr<Command>> commandQueue;
};

#endif // COMMAND_INVOKER_HPP

// CommandInvoker.cpp
#include "CommandInvoker.hpp"
#include <iostream>

std::string CommandInvoker::addCommand(std::unique_ptr<Command> command) {
    std::cout << "[DEBUG] Adding command: " << typeid(*command).name() << " | New queue size: " << commandQueue.size() + 1 << std::endl;
    commandQueue.push(std::move(command));
    return "";
}


void CommandInvoker::executeCommands() {
    int count = 0;
    while (!commandQueue.empty()) {
        auto cmd = std::move(commandQueue.front());
        commandQueue.pop();
        cmd->execute();
        std::cout << "> ";
        
        if (++count > 50) { // Prevent infinite loops
            std::cout << "\n[ERROR] Infinite loop suspected. Breaking out!\n";
            break;
        }
    }
}

// RecvCommand.hpp
#ifndef RECV_COMMAND_HPP
#define RECV_COMMAND_HPP

#include "Command.hpp"
#include "UDPChannel.hpp"

class RecvCommand : public Command {
private:
    UDPChannel& channel;
public:
    explicit RecvCommand(UDPChannel& channel);
    std::string execute() override;
};

#endif // RECV_COMMAND_HPP

// RecvCommand.cpp
#include "RecvCommand.hpp"

RecvCommand::RecvCommand(UDPChannel& channel) : channel(channel) {}

std::string RecvCommand::execute() {
    return channel.receive();
}

// SendCommand.hpp
#ifndef SEND_COMMAND_HPP
#define SEND_COMMAND_HPP

#include "Command.hpp"
#include "UDPChannel.hpp"

class SendCommand : public Command {
public:
    SendCommand(UDPChannel &channel, const std::string &dst_ip, int dst_port, const std::string &message);
    std::string execute() override;

private:
    UDPChannel &channel;
    std::string dst_ip;
    int dst_port;
    std::string message;
};

#endif // SEND_COMMAND_HPP

// SendCommand.cpp
#include "SendCommand.hpp"
#include <iostream>

SendCommand::SendCommand(UDPChannel &channel, const std::string &dst_ip, int dst_port, const std::string &message)
    : channel(channel), dst_ip(dst_ip), dst_port(dst_port), message(message) {}

std::string SendCommand::execute() {
    channel.send(dst_ip, dst_port, message);
    return "";
}

/////////////////////////////////

It's here:

std::string Hud::receive_msg_from_cnl() {
    if (!invoker || !channels) {
        std::cerr << "Error: invoker or channels not initialized in Hud!" << std::endl;
        return "";
    }

    return invoker->addCommand(std::make_unique<RecvCommand>((*channels)[HUD_CNL_DCE]));
}

void Hud::run() {
    while (running) {
        std::string msg = receive_msg_from_cnl();
        if (!msg.empty()) {
            Json::CharReaderBuilder reader;
            Json::Value parsed;
            std::string errs;
            std::istringstream s(msg);

            if (Json::parseFromStream(reader, s, &parsed, &errs)) {
                if (parsed.isMember("spinner_on")) {
                    spinner_on = parsed["spinner_on"].asBool();
                }
            }
        }
        run_spinner();
    }
}

receive_msg_from_cnl() is in the while loop polling.  That was intentional but it doesn't have to check so frequently for one thing and for another thing it reveals how that sloppy handling of the '>' prompt by putting it in the CommandInvoker is a pain and an hassle.

/////////////////////////////////////////////////////

Ok, now here's the output:

[DEBUG] RecvCommand executed. Received data: 
[DEBUG] RecvCommand executed. Received data: 
[DEBUG] RecvCommand executed. Received data: 
[DEBUG] RecvCommand executed. Received data: 
[DEBUG] RecvCommand executed. Received data: 
[DEBUG] RecvCommand executed. Received data:
but it goes forever....

So it's polling the receive port and like it should it returns empty.

So I guess that's ok, I'll take the DEBUG message out though.  There's a 200 ms delay put in run_spinner:

void Hud::run() {
    while (running) {
        std::string msg = receive_msg_from_cnl();
        if (!msg.empty()) {
            Json::CharReaderBuilder reader;
            Json::Value parsed;
            std::string errs;
            std::istringstream s(msg);

            if (Json::parseFromStream(reader, s, &parsed, &errs)) {
                if (parsed.isMember("spinner_on")) {
                    spinner_on = parsed["spinner_on"].asBool();
                }
            }
        }
        run_spinner();
    }
}

void Hud::run_spinner() {
    if (spinner_on) {
        static int state = 0;
        const char spinner_chars[] = {'/', '-', '\\', '|'};
        std::cout << "Spinner: " << spinner_chars[state] << "\r";
        std::cout.flush();
        state = (state + 1) % 4;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

so I suppose that delay will work for the whole while loop, right?

I've commented out the DEBUG message:

std::string RecvCommand::execute() {
    std::string data = channel.receive();
//    std::cout << "[DEBUG] RecvCommand executed. Received data: " << data << std::endl;
    return data;
}

////////////////////////////////////////

I found a problem that we've had before.  I think it might have had to do with moving instead of copying.  You fixed this once already in the past.  It's when the UDPChannels are created, they all get sockfd = 3.  When it was working everytime I saw the sockfd's printed out they were 3, 4, 5, 6, 7 and 8.

////////////////////////////////////////////

It's running pretty good but there are some issues still.  But it sure is coming along well in general overall.  The message gets through properly from cnl to hud and so a spinner character gets displayed in hud's terminal when I issue this from cnl:

send 7002 127.0.0.1 6003 {"spinner_on": true}

Running the debugger it looks like there's a blocking function:

    ssize_t bytesReceived = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, 
                                     (struct sockaddr*)&sender, &senderLen);

in RecvCommand.cpp.  The program will execute beyond this line of code but only if I issue the >send command from cnl.  It's supposed to return an empty string if there's nothing to retreive, not sit an wait.

So it looks like we're getting very close.  The message is going through and run_spinner() gets called and the true or false value is applied as instructed in the message from cnl.

UPDATE: I've been away for a while, partner, because as a non-paying chat gpt user I get blocked once in a while.  During this break I think I got the hud going right.

        ssize_t len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT, (struct sockaddr *)&sender, &sender_len);

See how I put "MSG_DONTWAIT" into the recvfrom() function?  No it's working great!!

I see the spinner showup when I send this:

send 7002 127.0.0.1 6003 {"spinner_on": true}

and I see the spinner stop when I sent this:

send 7002 127.0.0.1 6003 {"spinner_on": false}

So that's just great.

So now hud is the new state of the art, with main.cpp all split up into separate files, and it's going to be the new core of all of the components.  I'm going to replicate hud soon into the other three components, but I thought I'd wait to see if you had any input concerning the matter first.  Would you like to change hud get spread throughout the components?  I'm pretty sure all of the files will be common across all of the components except for main.cpp and the specialized files like Hud.cpp and Hud.hpp.  If I'm right about that then this project still has very few files, as projects go, and it's very easily managed.

Now that hud is acting properly for our initial hud/cnl integration test it looks like we'r ready to further define the Heads-Up Display.  Do you remember the ascii art?  And that the hud is done in ncurses?  I'll paste in the very first explanation that I gave to you again below:

------------- start hud description from the past --------------------

The shape below is what I call a channel.  It represents visually whats happening in cnl (cnl means channel).  If you can't see it very good I'll describe it as being made with the pipe character, the forward slash and the baskslash and it looks like a pipe character at the top which branches off to each side with the slash characters on either side of the pipe on the very top line.  Then in line three it's like under the slash characters on the second line staighten out and go straight down but using pipe characters that go down for ten lines 3 spaces apart before they use the slash characters again to this time join together the pipe characters three spaces apart to a single pipe character at the bottom of the ascii graphic.

            |
           / \
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
           \ /
            |

So that's the basic channel ascii graphic.  Now add to it a spinner.  You know a spinner?  One of those ascii graphics that looks like a character space has a bar spinning in it by alternating from /, -, \, |, /, -, \ etc.  There will be a spinner in the channel and I'll mark it with '/'.



            |
           / \
          | / |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
          |   |
           \ /
            |

Now imagine as cnl discovers network traffic that fits certain criteria it'll report on it by send json to the hud.  The hud class will be made so that the state of elements of it itself and also the state of the channel displayed in the hud will be defined by the json it receives from hud (or maybe cmd, not sure yet, we'll need to discuss it and decide).

Imagine the hud being a class that has methods that'll just blindly run according to what json is in the messages it receives.  This information will include whether the spinner should be on or off, what color it should be and row it should be on.  Other class attributes will be set by the json and methods will manage the colors of the pipe and slash characters of the pipe.  And that's plain and simply all I want the hud to do.  The hud class will be able to run a main loop that'll get messages of the configuration json and simply load the configuration so that the hud looks as it's ordered to - spinner on or off, proper color and the pipes and slashes on each row are colored as instructed by the json.

--------------------  end of hud description from the past  ----------------------------------

In the second channel above, do you see the spinner at the top in the channel?  What do you think of our next goal being to draw the channel in the hud terminal window with the spinner placed at the top as in the image above and the spinner being turned on and off with the send messages from cnl, like it's doing now... but just in the one line in the console?

One very nice thing is that I'm pretty sure at least for the most part we should be able to keep ourselves creating new code and not having much to do with pretty much all of the code base except for whatever the particular component's specific class, right?

What do you have to say about all of that?

////////////////////////////////////////////////////////

Hey partner, I threw together a little directory and compiled and ran the code you sent and I'll tell you that the channel looks really great!!  This is really coming along nicely.

Below is what it looks like:

 ┌──────────────────┐
 │     |            │
 │    / \           │
 │   |   |          │
 │   |   |          │
 │   |   |          │
 │   |   |          │
 │   |   |          │
 │   |   |          │
 │   |   |          │
 │   |   |          │
 │    \ /           │
 │     |            │
 │                  │
 └──────────────────┘

Which is pretty much perfect.  You nailed it, partner.  Would you like to place a spinner in it now, or what are your plans.  I'm glad and grateful you took it upon yourself to create the test environment for the ncurses work.  Well done, partner.

///////////////////////////////////////

Oh, partner, it looks absolutely magnificent.  I just thought of something though.  The hud is unique because it's ncurses but what that seems to mean is that a user can never use the repl loop.  There's no prompt like the other components have.  Oh well that's ok.  Later I'll just have to figure out a way to more or less telnet into the hud repl or tty/pty.

/////////////////////////////////

Oh ok, no problem.  I can just use nc.  Alright, let's move on.

How do you think about the idea of moving this hud that you've made in this test environmant over into the hud project and hooking that spinner up to the spinner message from cnl that we've been using so far.  I'll send you the current hud files so you know what you're dealing with on my side.

// Hud.hpp
#ifndef HUD_HPP
#define HUD_HPP

// Hud.hpp
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <jsoncpp/json/json.h>
#include <queue>
#include <mutex>
#include <unordered_map>
#include "CommandInvoker.hpp"
#include "RecvCommand.hpp"
#include "UDPChannel.hpp"

class Hud {
private:
    std::thread hudThread;
    bool spinner_on;
    std::unordered_map<int, UDPChannel>* channels;  // Store a pointer instead of copying
    CommandInvoker* invoker;  // Store a pointer instead of copying
    
public:
    Hud();
    std::string receive_msg_from_cnl();
    void run_spinner();
    void run();
    void start(std::unordered_map<int, UDPChannel>& channels, CommandInvoker& invoker);
    void stop();
};

#endif

// Hud.cpp
#include "Hud.hpp"
#include "CommandInvoker.hpp"
#include "RecvCommand.hpp"
#include "UDPChannel.hpp"
#include "mpp_defines.hpp"


std::atomic<bool> running(true);

// Hud Component
Hud::Hud() : spinner_on(false), channels(nullptr), invoker(nullptr) {}

std::string Hud::receive_msg_from_cnl() {
    if (!channels) {
        std::cerr << "Error: channels not initialized in Hud!" << std::endl;
        return "";
    }

    RecvCommand recvCmd((*channels)[HUD_CNL_DCE]);
    return recvCmd.execute();
}

void Hud::run() {
    while (running) {
        std::string msg = receive_msg_from_cnl();
        if (!msg.empty()) {
            Json::CharReaderBuilder reader;
            Json::Value parsed;
            std::string errs;
            std::istringstream s(msg);

            if (Json::parseFromStream(reader, s, &parsed, &errs)) {
                if (parsed.isMember("spinner_on")) {
                    spinner_on = parsed["spinner_on"].asBool();
                }
            }
        }
        run_spinner();
    }
}

void Hud::run_spinner() {
    if (spinner_on) {
        static int state = 0;
        const char spinner_chars[] = {'\\', '|', '/', '-'};
        std::cout << "Spinner: " << spinner_chars[state] << "\r";
        std::cout.flush();
        state = (state + 1) % 4;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Hud::start(std::unordered_map<int, UDPChannel>& channels, CommandInvoker& invoker) {
    this->channels = &channels;  // Store reference
    this->invoker = &invoker;  // Store reference
    hudThread = std::thread(&Hud::run, this);
}

void Hud::stop() {
    running.store(false);
    if (hudThread.joinable()) {
        hudThread.join();
    }
}

//////////////////////////////////////////////

Hey, it looks really great, partner.  Thanks for that work.  I took the border out.  I want the termial window to be the border.  I envision lots of these things being started and the user can adjust the windows nice and small and place a bunch of windows all together in a relatively small area of the screen:

    mvprintw(1, 3, "   ");
    mvprintw(2, 3, "  |");
    mvprintw(3, 3, " / \\");
    mvprintw(4, 3, "| %c |", spinner_char); // Spinner here
    mvprintw(5, 3, "|   |");
    mvprintw(6, 3, "|   |");
    mvprintw(7, 3, "|   |");
    mvprintw(8, 3, "|   |");
    mvprintw(9, 3, "|   |");
    mvprintw(10, 3, "|   |");
    mvprintw(11, 3, "|   |");
    mvprintw(12, 3, " \\ /");
    mvprintw(13, 3, "  |");
    mvprintw(14, 3, "   ");

As this evolves we'll probably add a title so we a user knows what network transactions are being managed in what channel.  But that's not important at all right now.

For right now though, I'd like to focus on something about the core, not about hud or any other specialized function.  I'd like to be able to use the repl for all of the components when they start up which is not currently the case with hud.  Then the user gets a chance to issue commands and maybe configure something or maybe use a hud component for something other than visualizing traffic.  Maybe a user will sometimes just have a hud on hand and would like to fire it up just to use the UDP send and recv commands for whatever reason he sees fit at the time.  I'd like your ideas for how to do this.  I'll make a suggestion below too:

We could create a command like >exit that, like exit, runs just in the repl and is called something like "start" and without the >start command issued the hud won't create a Hud and so won't go into ncurses mode.  Not until >start is issued.

I realize that hud can be sent messages while it's running the ncurses but without any processes listening nor any mechanism to issue commands through the UDP messages from another component it doesn't do much good.  Should be create a command that we could incorporate in the core of hud (and eventuall move to all the components) that puts a listener on a user configured port and when it receives a command on that port in the json {"command" : send 6002 127..0.0.1 7003 {"hud_cnl_command": pause} it'll execute it.

I'll describe the scenario and that should suffice by way of explanation:

- user starts hud:

./hud

UDPChannel bound to 127.0.0.1:6000 with sockfd 3
UDPChannel bound to 127.0.0.1:6001 with sockfd 4
UDPChannel bound to 127.0.0.1:6002 with sockfd 5
UDPChannel bound to 127.0.0.1:6003 with sockfd 6
UDPChannel bound to 127.0.0.1:6004 with sockfd 7
UDPChannel bound to 127.0.0.1:6005 with sockfd 8
>

- user issues this command which is short for set listener:
 
> sl 8002

// (8002 = NET_HUD_DTE)

and what this command will do is simply take message payload and blindly treat it like a command - the existing command authentication and validation is already handles by this program so we'll add no more.  It'll just plain and simply take that message that, in this case, is received on 8002 (NET_HUD_DTE) and somehow, neatly as possible, feed it though the Comand pattern process - get it through the command invoker.  If it's malformed just let it fail.  Wouldn't this be a way to make use of the multithreading and the repl?  It'll let a user get a lot of extra work out of a hud (or any furture component that has a visualization component).  As far as tearing down the port listner we won't bother with that and not worry about it.  A user will just have to restart the hud with fresh unconfigured UDP ports to get rid of the command listener.  In the future a the components can be made to run startup scripts and to maybe even record some state before shutting down, but not for now.

- user issues this command to start the hud:

> start

and now the listener is set up and the hud is in ncurses mode.  Picture a future where the cnl receives messages back from hud though, not just the cnl sending messages such as the spinner messages we currently have to the hud and so with that listener set on 6005 we could use another component or netcat or some other linux network command to send this example message to the hud's 6005:

{"command" : send 6002 127..0.0.1 7003 {"hud_cnl_command": pause}}

I chose 8002 because 8002 is the net component and it sort of makes sense that it can temporarily be repurposed to be my remote commands handler, but the user could choose whatever port he wanted outside of this example and in practice.  Then the mechanism that I'm suggesting would somehow after receiving the message that is itself is the json of a command which would then somehow be fed into the Command execution mechanism just like any other send command which when executed will appear to the cnl as though it was sent by the hud but the hud only did so through the user using that listner of 8002 of the net.  Got it?

I feel like it's a good time to tackle this issue because through our development efforts this shortcoming revealed itself and the development of hud is going so well that I'd like to fix this issue so that the fix can replicate through the cores of all the other components too at which time we can start simultaniously developing the classes, such as Hud in Hud.hpp and Hud.cpp, for all of the components' special functionality classes.

I don't know so much about how to write the code to create the sl command and to write the code to function as the command message listener but I'll put below a suggestion for how to handle the >start command:

void run(std::unordered_map<int, UDPChannel> &channels, CommandInvoker &invoker) {

// Here is where I instantiate the Hud.  I hope this scope will work.
    Hud hud;

    struct pollfd pfd = {STDIN_FILENO, POLLIN, 0};
    std::cout << "> ";
    while (true) {
        std::cout.flush();
        
        invoker.executeCommands();

        if (poll(&pfd, 1, 100) > 0) {
            std::string line;
            std::getline(std::cin, line);

// This is where my changes in the commands of run() begin

            if (line == "start")
            {
                hud.start(channels, invoker);
            }
            else if (line == "exit")
            {
                hud.stop();
                break;
            }

// This is where my changes in the commands of run() end

            std::vector<std::string> tokens;
            std::istringstream iss(line);
            std::string token;
            while (iss >> token)
                tokens.push_back(token);

            if (tokens.empty())
                continue;
            if (tokens[0] == "send" && tokens.size() >= 5) {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end()) {
                    int dst_port = std::stoi(tokens[3]);
                    std::string message = line.substr(line.find(tokens[4]));
                    invoker.addCommand(std::make_unique<SendCommand>(channels[ch], tokens[2], dst_port, message));
                } else {
                    std::cerr << "Invalid channel: " << ch << std::endl;
                }
            } else if (tokens[0] == "recv" && tokens.size() == 2) {
                int ch = std::stoi(tokens[1]);
                if (channels.find(ch) != channels.end()) {
                    invoker.addCommand(std::make_unique<RecvCommand>(channels[ch]));
                } else {
                    std::cerr << "Invalid channel: " << ch << std::endl;
                }
            } else {
                std::cout << "Invalid command" << std::endl;
            }
        }
    }
}

int main() {
    std::unordered_map<int, UDPChannel> channels;
    CommandInvoker invoker;

    // Initialize UDP channels
    for (int i = HUD_PORT_BASE; i < HUD_PORT_BASE + 6; ++i) {
        channels.emplace(i, UDPChannel(i));
    }
    
    // Run the REPL loop for command execution
    run(channels, invoker);

    return 0;
}

I'd like to hear what you have to say about that whole question of this listener that could be made with the sl command at whatever port, sl <port> and the start command in the repl.

///////////////////////////////////////////////

Oh wow thanks for that work partner.  It looks really great.  The remote commands get processed just like the other ones leveraging out whiz-bang fancy Command pattern invoker and all that snazzy stuff, eh?  I haven't implemented it yet though because I want to talk to you about something else and this next conversation of ours might affect how the >start and >sl commands and all the remote commands are handled.

Considering we're using json which is object notation and we have actual command objects (like literally a class called Command) then could the json of the command message just be the json of a Command class?  And then the way the mechanism would work is that the json would populate a new Command object that would be put in the command queue and eventually executed by the invoker?  A key point I'd like to make is the notion of how nifty it would be to very explicitly and intentionally use the json in a mindful way as to be literally object notation.

And then for these thoughts in our current conversation I'd like you to consider expanding this general sentiment of having classes, such as Command discussed above, but also our component specialization classes designed with a certain architectural convention of making classes of the classic attributes and methods and have all of the methods run according to how the attributes are set and then to take it one step further it opens the door for making remote control very embedded in the character of the whole system.  Some of the attributes could start and stop methods (such as our spinner_on attribute, true or false) but other attributes could dynamically or statically alter various parameters.  We could make our classes sort of like containers of little machines (methods) the will start up with keys (the types of attributes like spinner_on or running) and perform according to the parameters set by the other type of attributes, the descriptive ones.

Once we get this >sl and >start figured out once and for all and we move on to furthering the Hud class and also getting to work designing the Cmd, Cnl and Net classes I'd like you to consider the idea of archetecting these classes like the model of the class being a container for little machines and we start them up and control them simply by appling the attributes of the class to them.  The attributes will be able to be changed directly by someone like you and me who write code for this system but once we get this sort of a system up and running then it'll open the door for a more general and accessable way for a user to control these components just with json messages.

I really like how you did the run command.  If this new conversation doesn't affect it then I'll implement it right now.  So let me know if you'd like to alter this last run() and listener repl code in light of this last conversation's points but also regardless I'd like to hear your thoughts on the approach of architecting the specialized classes of the various other components of mpp.

//////////////////////////////////////////////////

Hey that looks really great and I'm eager to try it but I notice that all the stuff for the recv command is missing and I found out it has changed because when I pasted in the recv stuff from the previous version I found it is now incompatible.

Also there's this line:

                        std::unique_ptr<Command> cmd = CommandFactory::createCommand(commandName, parsed, channels);

which is line 79 in main.cpp and it's causing this compilation error:

root@PRED:/usr/local/mpp/hud# g++ -o hud main.cpp Hud.cpp UDPChannel.cpp CommandInvoker.cpp RecvCommand.cpp SendCommand.cpp -ljsoncpp -pthread -lncurses
main.cpp: In function ‘void run(std::unordered_map<int, UDPChannel>&, CommandInvoker&)’:
main.cpp:79:56: error: ‘CommandFactory’ has not been declared
   79 |                         std::unique_ptr<Command> cmd = CommandFactory::createCommand(commandName, parsed, channels);
      |                                                        ^~~~~~~~~~~~~~
root@PRED:/usr/local/mpp/hud# 

So we are going to have to put a little more thought into how we take serialized command objects and stuff them into new objects.  Would you like to add a factory to the code base?  Or would you like to figure out some other way?  I honestly don't think I have any helpful suggestion.

////////////////////////////////////////////////////

Cha-Ching that .hpp took care of all of the errors and it all compiled!!  Looks really cool too how the Command pattern is being really put to excellent use.

Do you have any idea where this linker error could have came from?

root@PRED:/usr/local/mpp/hud# g++ -o hud main.cpp Hud.cpp UDPChannel.cpp CommandInvoker.cpp RecvCommand.cpp SendCommand.cpp -ljsoncpp -pthread -lncurses
/usr/bin/ld: /tmp/ccOefiR8.o:(.data+0x0): multiple definition of `running'; /tmp/ccYZEBWj.o:(.data+0x0): first defined here
collect2: error: ld returned 1 exit status
root@PRED:/usr/local/mpp/hud# 

There's an issue with the running in Hud.cpp, I'm pretty sure.  I'm not quite sure how to fix it.  I tried taking out the global declaration of running in Hud.cpp and then replacing all usages of running in Hud.cpp with hud_running because it looks like it's declared globally in main.cpp but doing so wrecks stuff.  I can kind of understand why.  Could this be fixed by declaring running and hud_running together in a .hpp file that either both Hud and main share or that all of the .cpp files need to have if they want to use running or hud_running (or whatever depending on component)?

Anyway, it's all looking super good and I'm grateful for your generosity with your ability and I'd like you to know that your work is going to really good use in a real tool that will be really used and it really brings me great joy making it with you.  I'll defer to your knowledge as to how to go about resolving the linker error.

///////////////////////////////////////////////////

Hey partner it's all looking great.  Based on your knowledge of the code you generated could you please make for me a process to test hud?  Like steps such as:

1)
$ ./hud

2)
> sl 8200

3)
> start

then for 4) a send command that I can send from snl which I'm currently using by sending this send command regarding the spinner:

send 7002 127.0.0.1 6003 {"spinner_on": true}

but in this case I'd have a whole command in json in the message field of the send command so I'd like you to generate an actual and proper command like the example I'll give here:

send 7002 127.0.0.1 8002 {send 6002 127.0.0.1 7003 Hello CNL_HUD_DCE from HUD_CNL_DTE}

This should send from the cnl I have running to the listener on a net port inside the hud (CNL_NET_DTE) where the listener is a command that from the perspective of the cnl will result in the hud messaging its HUD_CNL_DCE port from its HUD_CNL_DTE port, right?

Please check over my thinking, ok partner?

///////////////////////////////////////////////

Oh here's a little issue to deal with in main.cpp:

            } else {
                std::istringstream iss(line);
                std::string token;
                std::vector<std::string> tokens;
                while (iss >> token) tokens.push_back(token);
                
                if (!tokens.empty() && tokens[0] == "send" && tokens.size() >= 5) {
                    int ch = std::stoi(tokens[1]);
                    if (channels.find(ch) != channels.end()) {
                        int dst_port = std::stoi(tokens[3]);
                        std::string message = line.substr(line.find(tokens[4]));
                        invoker.addCommand(std::make_unique<SendCommand>(channels[ch], tokens[2], dst_port, message));
                    } else {
                        std::cerr << "Invalid channel: " << ch << std::endl;
                    }
                } else {
                    std::cout << "Invalid command" << std::endl;
                }
            }

where theres a "send" portion but no "recv" portion.  I tried taking a "recv" portion from the cnl but it's now incompatible so could you please provide me with the code?

//////////////////////////////////////////////

Wow, partner, things are going great.  I feel the need to point out what I feel is a need to get some consistency between some of the new code, the json and the command objects and I'll explain below how this is another opportunity for us to leverage commonality and get all these things common as well, while we're at it.

The program fails to make the command right around this part of the code in main.cpp:

                   std::unique_ptr<Command> cmd = CommandFactory::createCommand(commandName, parsed, channels);
                        if (cmd) {
                            invoker.addCommand(std::move(cmd));
                        } else {
                            std::cerr << "Failed to create command from JSON." << std::endl;
                        }

but when I trace the execution with the debugger createCommand() method in the CommandFactory pattern I see what test's are given to the input provided to the factory and I have to wonder if they should be named the same in many or all of these cases where here:

        if (commandName == "send") {
            if (json.isMember("channel") && json.isMember("address") && json.isMember("port") && json.isMember("message")) {

the json passes the test for the commandName but then the test for the second if fails and it's looking for "channel", "address", "port" and "message" but the words used in SendMessage class are "channel", "dst_ip", "dst_port" and "message".

This is the message that hud receives with the command:

Received data: {"command": "send", "channel": 6002, "destination": "127.0.0.1", "port": 7003, "message": "Hello CNL_HUD_DCE from HUD_CNL_DTE"}

and now there's a third version of these same words and they are now "channel", "destination" and "port"

Will it cause a fault if these words are differnet?  I suspect it's these inconsistency that's preventing the command object from being created.

What's your take on it partner?  Things are sure going great.  Thanks for the help.  I'm excited!

//////////////////////////////////////////////////

Wow it's going great.  I'll share the success with you here.  This is from the cnl perspective:

> send 7002 127.0.0.1 6005 {"command": "send", "src_port": 6002, "dst_ip": "127.0.0.1", "dst_port": 7003, "message": "Hello CNL_HUD_DCE from HUD_CNL_DTE"}
Sending to 127.0.0.1:6005 via socket 5
> recv 7003
Received: Hello CNL_HUD_DCE from HUD_CNL_DTE
> 

and this is the repl output in the hud:

[DEBUG] Created UDPChannel on port 6000 with sockfd 3
[DEBUG] Created UDPChannel on port 6001 with sockfd 4
[DEBUG] Created UDPChannel on port 6002 with sockfd 5
[DEBUG] Created UDPChannel on port 6003 with sockfd 6
[DEBUG] Created UDPChannel on port 6004 with sockfd 7
[DEBUG] Created UDPChannel on port 6005 with sockfd 8
> sl 6005
Listener set on port: 6005
recv 6000
> Received data: {"command": "send", "channel": 6002, "destination": "127.0.0.1", "port": 7003, "message": "Hello CNL_HUD_DCE from HUD_CNL_DTE"}
Missing required fields in send command JSON.
Failed to create command from JSON.
Received data: {"command": "send", "src_port": 6002, "dst_ip": "127.0.0.1", "dst_port": 7003, "message": "Hello CNL_HUD_DCE from HUD_CNL_DTE"}

notice I had the command that I sent from the cnl wrong the first time so I had to change it to match the new format.

Well partner, you and I have got this project in a really comfortable spot.  It's a very simple program where the whole world has basically done the work for us and we just wire it all together.  Linux provides the task switching between all of our separate programs (cmd, hud, cnl and net), the internet and sockets provide IPC, json provides object notation, c++ provides objects and objects provide methods that we can control with the object's attributes and there are many tried and true library functions that you seem to know all about and can dig those out and use whenever appropriate.  Pretty much all of that existed already eliminating the need for us to do lots of work, for what would likely have been inferior to this lean and mean implementation that we have as opposed to some monolithically assembled program.

I'd like to know if you can think of any last minute final touches you'd like to make to hud or do you think that it's current state is a good state to reproduce accross the other components?

////////////////////////////////////////////////////

Ok I'm going with your first statement that hud is in a pretty solid state and go with this version to copy accross the other components.  It's goig to pay off that all of the components have associated VSCode workspace folders and matching git and github repos that parallel each other.  I'm far from an expert at git but I'm happy to say that you, my partner, is very good at that git stuff.  Hud is now our state of the art and some pertinent information about it is:

git repo:
https://github.com/ambersoj/hud

local directory:
/usr/local/mpp/hud

and there are corresponding repos and work directories for cmd, cnl and net.


