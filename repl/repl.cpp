#include "../include/repl.h"

namespace monkey{
    void printParserErrors(std::ofstream& output, std::string errors) {
        output << MONKEY_FACE << "\n";
        output << "Woops! We ran into some monkey business here!\n";
        output << "parser errors:\n";
        output << errors;
    }

    void printParserErrors(std::ostream& output, std::string errors) {
        output << "\n" <<MONKEY_FACE << "\n";
        output << "Woops! We ran into some monkey business here!\n";
        output << "parser errors:\n";
        output << "\033[31m" << errors << "\033[0m";
    }

    // repl
    /**
     * @return true if error
    */
    bool start_run(std::ifstream& input, std::ostream& output) {
        std::string line;
        std::string program;
        SymbolTable symbolTable;    // global symbol table
        std::shared_ptr<SymbolTable> symbolTablePtr = std::make_shared<SymbolTable>(symbolTable);
        registeBuiltinFunctions(symbolTablePtr);
        Compiler compiler(symbolTablePtr);

        while (getline(input, line)) {
            if (line[0] == '#') {
                continue;
            }
            program += line;
            program += "\n";
        }
        
        std::shared_ptr<Lexer> lexer = std::make_shared<Lexer>(program);
        std::shared_ptr<Parser> parser = std::make_shared<Parser>(lexer);
        
        auto program_ast = parser->parseProgram();
        if (parser->getErrors().size() != 0) {
            printParserErrors(output, parser->getErrors());
            return true;
        }

        try {
            compiler.Compile(program_ast);
        } catch (std::exception& e) {
            output << MONKEY_FACE << "\n";
            output << "Woops! We ran into some monkey business here!\n";
            output << "\033[31m" << e.what() << "\033[0m";
            return true;
        }

        output << WELCOME << "\n" << std::endl;
        VM vm(compiler.Bytecode());
        try {
            vm.Run();
        } catch (std::exception& e) {
            output << MONKEY_FACE << "\n";
            output << "Woops! We ran into some monkey business here!\n";
            output << "\033[31m" << e.what() << "\033[0m";
            return true;
        }
        // auto lastPopped = vm.LastPoppedStackElem()->inspect();
        // if (lastPopped != "null") {
        //     output << PROMPT << lastPopped << '\n';
        // }
        return false;
    }

    void start_cmd(std::istream& in, std::ostream& out) {
        Constants constants;    // global constants
        std::shared_ptr<Constants> constantsPtr = std::make_shared<Constants>(constants);
        Globals globals;    // global variables
        std::shared_ptr<Globals> globalsPtr = std::make_shared<Globals>(globals);
        globalsPtr->reserve(GlobalsSize);
        SymbolTable symbolTable;    // global symbol table
        std::shared_ptr<SymbolTable> symbolTablePtr = std::make_shared<SymbolTable>(symbolTable);

        registeBuiltinFunctions(symbolTablePtr);

        doPreAction(symbolTablePtr, constantsPtr, globalsPtr);

        Compiler compiler(symbolTablePtr);
        VM vm;
        while (true) {
            auto commands = getMultiLineInput(in);
            if (commands == "exit;" || commands == "quit;") {
                break;
            }
            out << PROMPT;
            auto lexer = std::make_shared<Lexer>(commands);
            auto parser = std::make_shared<Parser>(lexer);

            auto program = parser->parseProgram();
            if (parser->getErrors().size() != 0) {
                printParserErrors(out, parser->getErrors());
                std::exit(EXIT_FAILURE);
            }

            auto comp = compiler.NewWithState(symbolTablePtr, constantsPtr);
            // check_symbolTable(*symbolTablePtr); // debug
            try{
                comp->Compile(program);
            } catch (std::exception& e) {
                out << "\n" << MONKEY_FACE << "\n";
                out << "Woops! We ran into some monkey business here!\n";
                out<< "\033[31m" << e.what() << "\033[0m";
                std::exit(EXIT_FAILURE);
            }
            auto code = comp->Bytecode();

            // debug
            // for (auto& ins : code->instructions) {
            //     std::cerr << "Compile op: " << std::to_string(ins) << std::endl;
            // }

            auto machine = vm.NewWithGlobalsStore(code, globalsPtr);
            // check_globals(globals); // debug
            try {
                machine->Run();
            } catch (std::exception& e) {
                out << "\n" << MONKEY_FACE << "\n";
                out << "Woops! We ran into some monkey business here!\n";
                out << "\033[31m" << e.what() << "\033[0m";
                std::exit(EXIT_FAILURE);
        }
            auto lastPopped = machine->LastPoppedStackElem();
            if (lastPopped->inspect() == "null") {
                continue;
            }
            out << lastPopped->inspect() << "\n";
        }
    }

    void doPreAction(std::shared_ptr<SymbolTable> symbolTablePtr, 
                    std::shared_ptr<Constants> constantsPtr, 
                    std::shared_ptr<Globals> globalsPtr) {
        std::string pre_action = "let err=\"ERROR\";";
        auto commands = pre_action;
        auto lexer = std::make_shared<Lexer>(commands);
        auto parser = std::make_shared<Parser>(lexer);
        Compiler compiler(symbolTablePtr);
        VM vm;

        auto program = parser->parseProgram();
        if (parser->getErrors().size() != 0) {
            std::cerr << "\033[31mpre_action parser error\033[0m" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        auto comp = compiler.NewWithState(symbolTablePtr, constantsPtr);
        try{
            comp->Compile(program);
        } catch (std::exception& e) {
            std::cerr << "\n" << MONKEY_FACE << "\n";
            std::cerr << "Woops! We ran into some monkey business here!\n";
            std::cerr << "\033[31m" << e.what() << "\033[0m";
            std::exit(EXIT_FAILURE);
        }

        auto code = comp->Bytecode();
        auto machine = vm.NewWithGlobalsStore(code, globalsPtr);
        try {
            machine->Run();
        } catch (std::exception& e) {
            std::cerr << "\n" << MONKEY_FACE << "\n";
            std::cerr << "Woops! We ran into some monkey business here!\n";
            std::cerr << "\033[31m" << e.what() << "\033[0m";
            std::exit(EXIT_FAILURE);
        }
    }

    void registeBuiltinFunctions(std::shared_ptr<SymbolTable> symbolTablePtr) {
        for (int i = 0; i < builtins.size(); ++i) {
            auto builtin = builtins[i];
            symbolTablePtr->DefineBuiltin(i, builtin.name);
        }
    }

    std::string getMultiLineInput(std::istream& in) {
        std::string line, commands;
        while (std::getline(in, line)) {
            commands += line + " ";
            if (!line.empty() && line.back() == ';') {
                break;
            }
        }
        // std::cerr << "commands: " << commands << std::endl;
        commands.pop_back();
        return commands;
    }

    void check_symbolTable(const SymbolTable& s) {
        auto a = s.GetStore();
        for (auto& i : a) {
            std::cout << i.first << " " << i.second.name << " " << i.second.scope << " " << i.second.index << ", ";
        }
        std::cout << std::endl;
    }

    void check_globals(const std::vector<std::shared_ptr<Object>>& globals) {
        if (globals.empty()) {
            std::cout << "globals is empty" << std::endl;
            return;
        }
        for (auto& i : globals) {
            std::cout << i->inspect() << ", ";
        }
        std::cout << std::endl;
    }

}; // namespace monkey