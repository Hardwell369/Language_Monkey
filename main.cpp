#include <iostream>
#include <fstream>
#include <string>

#include "./utils/timer.h"
#include "./include/repl.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./monkey [run] or [cmd]" << std::endl;
        return 1;
    }
    std::string arg(argv[1]);
    if (arg == "run") {
        Timer timer;
        std::ifstream input("input.txt");
        std::ostream output(std::cout.rdbuf());
        bool output_need_print;
        output_need_print = monkey::start_run(input, output);
        input.close();
        std::cout << "\033[32m" << "Elapsed time: " << timer.elapsed() << "s" << "\033[0m" << std::endl;
    } else if (arg == "cmd") {
        std::istream input(std::cin.rdbuf());
        std::ostream output(std::cout.rdbuf());
        monkey::start_cmd(input, output);
    } else {
        std::cerr << "Usage: ./monkey [run] or [cmd]" << std::endl;
        return 1;
    }

    return 0;
}