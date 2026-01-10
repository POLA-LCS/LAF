#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <fstream>
#include <conio.h>
#include <stdexcept>
#include <sstream>

using Frame = std::string;

std::string get_laf_content(const std::string& path) {
    std::ifstream file(path.c_str());
    if(!file.is_open())
        throw std::runtime_error("File not found.");
    return (std::stringstream() << file.rdbuf()).str();
}

size_t hex_to_uint(const std::string& str) {    
    size_t result = 0;
    for(const char& ch : str) {
        result *= 16;
        if(ch >= '0' && ch <= '9') result += (ch - '0');        else 
        if(ch >= 'A' && ch <= 'F') result += ((ch - 'A') + 10); else
        throw std::logic_error(("Invalid hex character: " + std::to_string(static_cast<int>(ch)) + "(" + ch + ")").c_str());
    }
    return result;
}

struct LAF {
    std::string content;
    size_t width, height, frec;
    bool fullscreen = false, reverse = false;

    LAF(std::string content) {
        if(content.substr(0, 3) != "LAF")
            throw std::logic_error("File is not a LAF format. (missing \"LAF\" tag in header).");

        width  = hex_to_uint(content.substr(3, 3));
        height = hex_to_uint(content.substr(6, 3));
        frec   = hex_to_uint(content.substr(9, 2));
        if(frec <= 0) frec = 4;
        for(size_t i = 11; i < content.length(); i++) {
            const char& ch = content[i];
            if(ch == '|') {
                this->content = content.substr(i + 1);
                break;
            }

            switch(ch) {
                case 'F':
                    fullscreen = true;
                    break;
                case 'R':
                    reverse = true;
                    break;
                default:
                    throw std::logic_error(
                        (("(Unknown header option: " + std::to_string(static_cast<int>(ch)) + "(" + ch + ")\n") + (
                            "Perhaps missing header termination (\"|\")")).c_str()
                    );
            }
        }
    }

    std::vector<Frame> get_frames() const {
        std::vector<Frame> frames;
        size_t row_count = content.length() / (width * height);

        if(content.length() != width * height * row_count) {
            throw std::logic_error(
                ("Content length (" + std::to_string(content.length()) + ") does not match frame dimensions (" + std::to_string(width) + ", " + std::to_string(height) + ")").c_str()
            );
        }

        for(size_t i = 0; i < row_count; ++i) {
            Frame frame;
            for(size_t row = 0; row < height; ++row) {
                size_t offset = (i * height + row) * width;
                frame += content.substr(offset, width) + '\n';
            }
            frames.push_back(frame);
        }

        if(reverse) {
            size_t size = frames.size();
            while(size --> 0) {
                frames.push_back(frames[size]);
            }
        }
        return frames;
    }
};

int main(int argc, char* argv[]) {
    try {
        if(argc == 1)
            throw std::logic_error("No input was provided.");
    
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        if(handle == INVALID_HANDLE_VALUE)
            throw std::logic_error("Failed to handle the console.");
    
        const std::string path(argv[1]);

        // The ENGINE !!!
        const LAF laf(get_laf_content(path));

        HWND wind = GetConsoleWindow();
        if(laf.fullscreen) {
            DWORD newstyle = GetWindowLong(wind, GWL_STYLE);
            SetWindowLong(wind, GWL_STYLE, newstyle & ~WS_OVERLAPPEDWINDOW);
            SendMessage(wind, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }
    
        system("cls");
        bool running = true;
        while(running) {
            size_t frame_rate = 1000 / ((laf.frec == 0) ? 1 : laf.frec);
            const std::vector<Frame>& frames = laf.get_frames();
            for(size_t i = 0; i < frames.size() && running; i++, running = !kbhit()) {
                const Frame& frame = frames[i];
                SetConsoleCursorPosition(handle, {0, 0});
                WriteConsole(handle, frame.c_str(), frame.length(), nullptr, nullptr);
                Sleep(frame_rate);
            }
        }
    
        if(laf.fullscreen)
            SendMessage(wind, WM_SYSCOMMAND, SC_RESTORE, 0);
        
    } catch(const std::logic_error& error) {
        std::cerr << "[LAF] " + std::string(error.what());
        return 1;
    }

    return 0;
}
