#pragma once

#include <exception>
#include <string>

namespace monkey {
    struct Errors : public std::exception {
        std::string msg;
        Errors(std::string m) : msg(m) {}
        virtual ~Errors() = default;
        const char* what() const noexcept override {
            return msg.c_str();
        }
        
    };

    struct RunningError : public Errors {
        RunningError(std::string m) : Errors("Running Error: " + m + "\n") {}
    };

    struct CompileError : public Errors {
        CompileError(std::string m) : Errors("Compile Error: " + m + "\n") {}
    };

    struct CodeError : public Errors {
        CodeError(std::string m) : Errors("Code Error: " + m + "\n") {}
    };

    
} // namespace monkey