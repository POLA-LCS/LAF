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

#include <filesystem>
namespace fs = std::filesystem;

constexpr uint32_t LAF_START = 0;
constexpr uint32_t LAF_SIZE = 3;
constexpr uint32_t WIDTH_START = 3;
constexpr uint32_t WIDTH_SIZE = 3;
constexpr uint32_t HEIGHT_START = 6;
constexpr uint32_t HEIGHT_SIZE = 3;
constexpr uint32_t FREC_START = 9;
constexpr uint32_t FREC_SIZE = 2;
constexpr uint32_t FREC_DEFAULT = 4;
constexpr uint32_t HEADER_DATA_END = FREC_START + FREC_SIZE;

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

    inline void clearScreen() {
        COORD topLeft = { 0, 0 };
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        DWORD written;
        DWORD cells;

        if (!GetConsoleScreenBufferInfo(handle, &csbi)) return;

        cells = csbi.dwSize.X * csbi.dwSize.Y;

        FillConsoleOutputCharacter(handle, (TCHAR)' ', cells, topLeft, &written);
        FillConsoleOutputAttribute(handle, csbi.wAttributes, cells, topLeft, &written);
        SetConsoleCursorPosition(handle, topLeft);
    }
};

/* Logic for parsing and animating LAF files.
** Optimized for memory efficiency by pre-rendering frames.
*/
struct LAFEngine {
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t frec{ 4 };
    bool fullscreen_flag{ false };
    bool reverse_flag{ false };
    bool stop_flag{ false };
    std::vector<std::string> frames;

    inline explicit LAFEngine(const fs::path& path) {
        const std::string path_str = path.string();
        if(!fs::exists(path)) {
            throw std::runtime_error("path: doesn't exists. " + path_str);
        }

        if(fs::is_directory(path)) {
            throw std::runtime_error("path: is a directory. " + path_str);
        }

        std::ifstream file(path);
        if(!file) {
            throw std::runtime_error("file: Could not open. " + path_str);
        }

        std::string raw_content = (std::stringstream() << file.rdbuf()).str();

        if(raw_content.size() < HEADER_DATA_END + 1 || raw_content.substr(LAF_START, LAF_SIZE) != "LAF") {
            throw std::runtime_error("Header error: Missing LAF identifier.");
        }

        width  = std::stoul(raw_content.substr(WIDTH_START, WIDTH_SIZE  ), nullptr, 16);
        height = std::stoul(raw_content.substr(HEIGHT_START, HEIGHT_SIZE), nullptr, 16);
        frec   = std::stoul(raw_content.substr(FREC_START, FREC_SIZE    ), nullptr, 16);

        size_t data_start = raw_content.find('|');
        if(data_start == std::string::npos) {
            throw std::runtime_error("format: Missing end of header separator '|'.");
        }

        std::string flags = raw_content.substr(HEADER_DATA_END, data_start - HEADER_DATA_END);
        reverse_flag    = (flags.find('R') != std::string::npos);
        fullscreen_flag = (flags.find('F') != std::string::npos);
        stop_flag       = (flags.find('S') != std::string::npos);

        std::string content = raw_content.substr(data_start + 1);
        content.erase(std::remove(content.begin(), content.end(), '\n'), content.end());
        content.erase(std::remove(content.begin(), content.end(), '\r'), content.end());

        const size_t chars_per_frame = width * height;

        if(chars_per_frame == 0) {
            throw std::runtime_error("header: dimentions must be positive.");
        }

        if(content.length() % chars_per_frame != 0) {
            throw std::runtime_error("data: Content doesn't match dimentions.");
        }

        size_t total_frames = content.length() / chars_per_frame;
        for(size_t i = 0; i < total_frames; ++i) {
            std::string frame_buffer;
            for(size_t r = 0; r < height; ++r) {
                frame_buffer += content.substr((i * height + r) * width, width) + "\n";
            }
            frames.push_back(frame_buffer);
        }
    }

    inline void play() {
        if(reverse_flag) {
            for(int i = static_cast<int>(frames.size()) - 1; i >= 0; --i) {
                frames.push_back(frames[i]);
            }
        }

        if(!frec) frec = FREC_DEFAULT;

        ConsoleContext context(fullscreen_flag);
        uint32_t frame_delay = 1000 / frec;

        context.clearScreen();

        bool interrupted = false;
        while(!interrupted) {
            for(const std::string& frame : frames) {
                SetConsoleCursorPosition(context.handle, { 0, 0 });
                WriteConsoleA(context.handle, frame.c_str(), static_cast<DWORD>(frame.length()), nullptr, nullptr);

                if(_kbhit()) {
                    interrupted = true;
                    break;
                }
                Sleep(frame_delay);
            }
            if(stop_flag) break;
        }
    }
};

#endif