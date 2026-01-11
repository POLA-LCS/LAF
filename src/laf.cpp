#include <laf.hpp>
#include <iostream>
#include <clab.hpp>
#include <map>

void print_usage() {
    std::cout << "[USAGE]\n";
    std::cout << "    (--help | -h)           Displays this message\n";
    std::cout << "    <file.laf> [options]    Parses and plays a laf file\n";
    std::cout << "\noptions:\n";
    std::cout << "    +fps <value>   Change the FPS of the animation\n";
    std::cout << "    +<flag>        Add a flag overriding the default flag\n";
    std::cout << "    -<flag>        Remove a flag overriding the default flag\n";
    std::cout << "\nflag:\n";
    std::cout << "    stop    Stops when reaching the end\n";
    std::cout << "    full    Fullscreens the console window\n";
    std::cout << "    reve    Plays it in reverse when reached the end\n";
}

/* Application entry point.
** Usage: laf.exe <path_to_file.laf>
*/

const std::map<std::string, std::string> tag_to_flag = {
    {"Fps", "+fps"},
    {"File", "position argument file"},
};

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
            .start().abort().flag("help", "--").flag("h").action([](auto) {
                print_usage();
            }).end()
            .start("Fps").flag("fps", "+").consume(1).end()
            .start("FullAdd").flag("full", "+").end()
            .start("FullRem").flag("full", "-").end()
            .start("ReveAdd").flag("reve", "+").end()
            .start("ReveRem").flag("reve", "-").end()
            .start("StopAdd").flag("stop", "+").end()
            .start("StopRem").flag("stop", "-").end()
            .evaluate(argc, argv);

        if(eval.aborted()) return 0;

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

        if(eval.state("Fps")) {
            laf_engine.frec = std::stoul(eval.value("Fps"));
        }

        

        laf_engine.play();
    } catch(const clab::MissingArgument& e) {
        std::cerr << "[ERROR] Missing argument: " << tag_to_flag.at(std::string(e.what())) << '\n' << std::endl;
    } catch(const clab::MissingValue& e) {
        std::cerr << "[ERROR] Missing value for: " << tag_to_flag.at(std::string(e.what())) << '\n' << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}