#include "Hud.hpp"
#include "mpp_defines.hpp"
#include <ncurses.h>
#include <chrono>
#include <thread>

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

void Hud::draw() {
    clear();

    const char* spinner_frames = R"(|/-\)"; // Spinner animation frames
    static int spinner_state = 0;
    char spinner_char = spinner_on ? spinner_frames[spinner_state] : ' ';

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

    refresh();
    spinner_state = (spinner_state + 1) % 4;
}

void Hud::run() {
    initscr();
    noecho();
    curs_set(0);
    timeout(100); // Non-blocking input

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
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    endwin();
}

void Hud::start(std::unordered_map<int, UDPChannel>& channels, CommandInvoker& invoker) {
    this->channels = &channels;
    this->invoker = &invoker;
    hudThread = std::thread(&Hud::run, this);
}

void Hud::stop() {
    running.store(false);
    if (hudThread.joinable()) {
        hudThread.join();
    }
    endwin();
}
