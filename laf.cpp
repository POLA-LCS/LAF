#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <fstream>
#include <conio.h>

void fast_print(HANDLE handle, std::string str) {
    WriteConsole(handle, str.c_str(), str.length(), nullptr, nullptr);
}

std::string get_laf_content(const std::string& path) {
    std::ifstream file(path.c_str());
    if(!file.is_open()) {
        std::cerr << "[LAF] File not found." << std::endl;
        exit(1);
    }

    std::string content;
    for(std::string line; getline(file, line); content += line);
    return content;
}

size_t hex_to_uint(const std::string& str) {    
    size_t result = 0;
    for(const char& ch : str) {
        result *= 16;
        if(ch >= '0' && ch <= '9') {
            result += (ch - '0');
        } else if(ch >= 'A' && ch <= 'F') {
            result += ((ch - 'A') + 10);
        } else {
            std::cerr << "[LAF] Invalid hex character: " << (int)ch << '(' << ch << ')' << std::endl;
            exit(1);
        }
    }
    return result;
}

typedef std::string Frame;

struct Pai {
    std::string content;
    size_t width, height, frec;
    bool fullscreen = false, reverse = false;

    Pai(std::string content) {
        if(content.substr(0, 3) != "LAF") {
            std::cerr << "[LAF] File is not a LAF format. (missing LAF header)." << std::endl;
            exit(1);
        }

        width  = hex_to_uint(content.substr(3, 3));
        height = hex_to_uint(content.substr(6, 3));
        frec   = hex_to_uint(content.substr(9, 2));
        if(!frec) frec = 4;
        for(size_t i = 11; i < content.length(); i++) {
            char ch = content[i];
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
                    std::cerr << "[LAF] Unknown header option: " << (int)ch << '(' << ch << "\n";
                    std::cerr << "    Perhaps missing header termination (\"|\")" << std::endl;
                    exit(1);
            }
        }
    }

    std::vector<Frame> get_frames() const {
        std::vector<Frame> frames;
        size_t row_count = content.length() / (width * height);

        if(content.length() != width * height * row_count) {
            std::cerr << "[LAF] Content length does not match frame dimensions." << std::endl;
            exit(1);
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
    if(argc == 1) {
        std::cout << "[LAF] No input was provided." << std::endl;
        return 1;
    }

    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if(!handle) {
        std::cerr << "[LAF] Failed to handle the console." << std::endl;
        return 1;
    }

    std::string path = argv[1];
    std::string laf_content = get_laf_content(path);
    Pai image(laf_content);

    HWND wind = GetConsoleWindow();
    if(image.fullscreen) {
        DWORD newstyle = GetWindowLong(wind, GWL_STYLE);
        SetWindowLong(wind, GWL_STYLE, newstyle & ~WS_OVERLAPPEDWINDOW);
        SendMessage(wind, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
    }

    system("cls");
    while(true) {
        bool running = true;
        size_t frame_rate = 1000 / ((image.frec == 0) ? 1 : image.frec);
        for(const Frame& frame : image.get_frames()) {
            running = !kbhit();
            if(!running) break;
            SetConsoleCursorPosition(handle, {0, 0});
            fast_print(handle, frame);
            Sleep(frame_rate);
        }
        if(!running) break;
    }

    if(image.fullscreen) {
        SendMessage(wind, WM_SYSCOMMAND, SC_RESTORE, 0);
    }

    return 0;
}
