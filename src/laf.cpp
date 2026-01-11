#include <laf.hpp>
#include <iostream>

void print_usage() {
    std::cout << "[LAF v2.0.0] Usage\n";
    std::cout << "    laf.exe <file.laf> [options]    Parses and plays a laf file\n";
    std::cout << "[options]\n";
    std::cout << "    +<flag>    Add a flag overriding the default flag\n";
    std::cout << "    -<flag>    Remove a flag overriding the default flag\n";
    std::cout << "<flag>\n";
    std::cout << "    stop       Stops when reaching the end\n";
    std::cout << "    full       Fullscreens the console window\n";
    std::cout << "    reverse    Plays it in reverse when reached the end\n";
}

/* Application entry point.
** Usage: laf.exe <path_to_file.laf>
*/
int main(int argc, char* argv[]) {
    if(argc < 2) {
        print_usage();
        return 1;
    }

    try {
        LAFEngine laf_engine(argv[1]);
        laf_engine.play();
    } catch(const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}