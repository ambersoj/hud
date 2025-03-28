#include "MPPCommands.hpp"

std::string StartCommand::execute() {
    MPPNode::getInstance().start();
    return "MPPNode started.";
}

std::string StopCommand::execute() {
    MPPNode::getInstance().stop();
    return "MPPNode stopped.";
}

std::string AddListenerCommand::execute() {
    MPPNode::getInstance().addCommandListener(port);
    return "Added command listener on port " + std::to_string(port);
}

std::string RemoveListenerCommand::execute() {
    MPPNode::getInstance().removeCommandListener(port);
    return "Removed command listener from port " + std::to_string(port);
}

RecvCommand::RecvCommand(UDPSocket& src_port) : src_port(src_port) {}

std::string RecvCommand::execute() {
    std::string data = src_port.receive();
    if(data != "") {
        std::cout << "Received data: " << data << std::endl;
    }
    return data;
}

SendCommand::SendCommand(UDPSocket &src_port, const std::string &dst_ip, int dst_port, const std::string &message)
    : src_port(src_port), dst_ip(dst_ip), dst_port(dst_port), message(message) {}

std::string SendCommand::execute() {
    src_port.send(dst_ip, dst_port, message);
    return "[INFO] SendCommand executed";
}

std::string UpdateHudCommand::execute() {
    std::ostringstream oss;
    oss << jsonData;
    hud.update(oss.str());
    return "[INFO] UpdateHudCommand executed";
}

StartHudCommand::StartHudCommand(Hud& hud, std::unordered_map<int, UDPSocket>& sockets, const Json::Value& json)
    : hud(hud), sockets(sockets), json(json) {}
    
std::string StartHudCommand::execute() {
    hud.start(sockets);
    hud.initializeFromJson(json.toStyledString());
    return "[INFO] Hud started and initialized.";
}

std::string StopHudCommand::execute() {
    hud.shutdownNcurses();
    hud.stop();
    return "[INFO] Hud stopped.";
}
 