#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "./include.h"

namespace monkey{
    static const std::string PROMPT = "\033[32m>> \033[0m";

    static const std::string WELCOME = R"(                         __                          
 /'\_/`\                /\ \                         
/\      \    ___     ___\ \ \/'\      __   __  __    
\ \ \__\ \  / __`\ /' _ `\ \ , <    /'__`\/\ \/\ \   
 \ \ \_/\ \/\ \L\ \/\ \/\ \ \ \\`\ /\  __/\ \ \_\ \  
  \ \_\\ \_\ \____/\ \_\ \_\ \_\ \_\ \____\\/`____ \ 
   \/_/ \/_/\/___/  \/_/\/_/\/_/\/_/\/____/ `/___/> \
                                               /\___/
                                               \/__/ )";

    static const std::string MONKEY_FACE = R"(            __,__
   .--.  .-"     "-.  .--.
  / .. \/  .-. .-.  \/ .. \
 | |  '|  /   Y   \  |'  | |
 | \   \  \ x | x /  /   / |
  \ '- ,\.-"""""""-./, -' /
   ''-' /_   ^ ^   _\ '-''
       |  \._   _./  |
       \   \ 'v' /   /
        '._ '-=-' _.'
           '-----')";


    void printParserErrors(std::ofstream& output, std::string errors);

    // repl
    bool start_run(std::ifstream& input, std::ostream& output);

    void start_cmd(std::istream& in, std::ostream& out);

    void registeBuiltinFunctions(std::shared_ptr<SymbolTable> symbolTablePtr);

    void doPreAction(std::shared_ptr<SymbolTable> symbolTablePtr, 
                    std::shared_ptr<Constants> constantsPtr, 
                    std::shared_ptr<Globals> globalsPtr);

    std::string getMultiLineInput(std::istream& in);

    void check_symbolTable(const SymbolTable& s);

    void check_globals(const std::vector<std::shared_ptr<Object>>& globals);

}; // namespace monkey