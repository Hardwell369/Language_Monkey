#pragma once

#include <string>
#include "./token.h"

namespace monkey {
    class Lexer{
    public:
        Lexer(std::string input) : input(input) {
            readPosition = 0;
            readChar();
        }

        Token nextToken();

    private:
        // helper functions
        // 读取字符, 并更新position和readPosition
        void readChar();

        // 跳过空白字符
        void skipWhitespace();

        char peekChar();

        // 读取完整的变量名
        std::string readIdentifier();

        // 读取完整的数字
        std::string readNumber();

        // 读取字符串
        std::string readString();

        // 判断是否为字符
        bool isLetter(char ch);

        // 判断是否为数字
        bool isDigit(char ch);

    private:
        std::string input;
        int position; // current position in input (points to current char)
        int readPosition; // current reading position in input (after current char)
        char ch; // current char under examination
    };
    
}; // namespace monkey