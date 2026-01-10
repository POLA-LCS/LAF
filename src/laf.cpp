#include <laf.hpp>
#include <iostream>

void print_usage() {
    std::cout << "[LAF v2.0.0] Usage\n";
    std::cout << "    laf.exe <file.laf>    Parses and plays a laf file\n";
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