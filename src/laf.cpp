#include <laf.hpp>
#include <iostream>
#include <clab.hpp>

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

void changeBooleanConfings(const std::string& name, const bool& flag, const bool& counter_flag, bool& value) {
    if(flag) {
        value = true;
        if(counter_flag)
            throw std::runtime_error(name + " override is redundant");
    } else if(counter_flag) {
        value = false;
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        print_usage();
        return 1;
    }

    try {
        clab::Evaluation eval = clab::CLAB("Path")
            .start("File").consume(1).required().end()
            .start("FullAdd").flag("full", "+").end()
            .start("FullRem").flag("full", "-").end()
            .start("ReveAdd").flag("reverse", "+").end()
            .start("ReveRem").flag("reverse", "-").end()
            .start("StopAdd").flag("stop", "+").end()
            .start("StopRem").flag("stop", "-").end()
            .evaluate(argc, argv);

        LAFEngine laf_engine(eval.value("File"));

        bool FullAdd = eval.state("FullAdd");
        bool FullRem = eval.state("FullRem");
        bool ReveAdd = eval.state("ReveAdd");
        bool ReveRem = eval.state("ReveRem");
        bool StopAdd = eval.state("StopAdd");
        bool StopRem = eval.state("StopRem");

        changeBooleanConfings("fullscreen", FullAdd, FullRem, laf_engine.fullscreen_flag);
        changeBooleanConfings("reverse", ReveAdd, ReveRem, laf_engine.reverse_flag);
        changeBooleanConfings("stop", StopAdd, StopRem, laf_engine.stop_flag);

        laf_engine.play();
    } catch(const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}