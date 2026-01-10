#ifndef LAF_ENGINE_HPP
#define LAF_ENGINE_HPP

/*============================================================================*\
| LAF Engine - Lis of Ascii Frames player.                                     |
|==============================================================================|
| This header defines the LAFEngine class,                                     |
| designed to parse and play ASCII animations                                  |
| from .laf files directly in the Windows console.                             |
| It handles console configuration, frame rendering, and playback control.     |
\*============================================================================*/

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <windows.h>
#include <conio.h>

/* Handles Windows terminal configuration using RAII.
** Ensures console state is restored on destruction.
*/
struct ConsoleContext {
    HANDLE handle;
    HWND window;
    bool is_fullscreen;
    CONSOLE_CURSOR_INFO original_cursor;

    inline ConsoleContext(bool fullscreen) : is_fullscreen(fullscreen) {
        handle = GetStdHandle(STD_OUTPUT_HANDLE);
        window = GetConsoleWindow();

        GetConsoleCursorInfo(handle, &original_cursor);
        CONSOLE_CURSOR_INFO hide_cursor = original_cursor;
        hide_cursor.bVisible = FALSE;
        SetConsoleCursorInfo(handle, &hide_cursor);

        if(is_fullscreen) {
            DWORD style = GetWindowLong(window, GWL_STYLE);
            SetWindowLong(window, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
            SendMessage(window, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }
    }

    inline ~ConsoleContext() {
        SetConsoleCursorInfo(handle, &original_cursor);
        if(is_fullscreen) {
            SendMessage(window, WM_SYSCOMMAND, SC_RESTORE, 0);
        }
    }
};

/* Logic for parsing and animating LAF files.
** Optimized for memory efficiency by pre-rendering frames.
*/
class LAFEngine {
public:
    inline explicit LAFEngine(const std::string& path) {
        std::ifstream file(path);
        if(!file) {
            throw std::runtime_error("File error: Could not open " + path);
        }

        std::stringstream ss;
        ss << file.rdbuf();
        std::string raw = ss.str();

        if(raw.size() < 12 || raw.substr(0, 3) != "LAF") {
            throw std::runtime_error("Header error: Missing LAF identifier.");
        }

        width = std::stoul(raw.substr(3, 3), nullptr, 16);
        height = std::stoul(raw.substr(6, 3), nullptr, 16);
        frec = std::stoul(raw.substr(9, 2), nullptr, 16);
        if(frec == 0)
            frec = 4;

        size_t data_start = raw.find('|');
        if(data_start == std::string::npos) {
            throw std::runtime_error("Format error: Missing data separator '|'.");
        }

        std::string flags = raw.substr(11, data_start - 11);
        bool reverse_flag = (flags.find('R') != std::string::npos);
        default_fullscreen = (flags.find('F') != std::string::npos);

        std::string content = raw.substr(data_start + 1);
        content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
        content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());

        size_t chars_per_frame = width * height;

        if(chars_per_frame == 0 || (content.length() % chars_per_frame) != 0) {
            throw std::runtime_error("Data error: Content does not match dimensions.");
        }

        size_t total_frames = content.length() / chars_per_frame;
        for(size_t i = 0; i < total_frames; ++i) {
            std::string frame_buffer;
            for(size_t r = 0; r < height; ++r) {
                frame_buffer += content.substr((i * height + r) * width, width) + "\n";
            }
            frames.push_back(frame_buffer);
        }

        if(reverse_flag) {
            for(int i = static_cast<int>(frames.size()) - 1; i >= 0; --i) {
                frames.push_back(frames[i]);
            }
        }
    }

    inline void play() {
        ConsoleContext context(default_fullscreen);
        uint32_t frame_delay = 1000 / frec;

        system("cls");

        while(!_kbhit()) {
            for(const auto& frame : frames) {
                SetConsoleCursorPosition(context.handle, { 0, 0 });
                WriteConsoleA(context.handle, frame.c_str(), static_cast<DWORD>(frame.length()), nullptr, nullptr);

                if(_kbhit()) return;
                Sleep(frame_delay);
            }
        }
    }

private:
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t frec{ 4 };
    bool default_fullscreen{ false };
    std::vector<std::string> frames;
};

#endif