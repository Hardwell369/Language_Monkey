#pragma once 

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "./ast.h"
#include "./lexer.h"
#include "./token.h"

namespace monkey{
    enum prec{
        LOWEST = 0,
        EQUALS,         // ==
        LESSGREATER,    // > or <
        SUM,            // +
        PRODUCT,        // *
        PREFIX,         // -X or !X
        CALL,           // myFunction(X)
        INDEX           // array[index]
    };

    static std::map<TokenType, prec> precedences = {
        {TokenType::EQ, prec::EQUALS},
        {TokenType::NOT_EQ, prec::EQUALS},
        {TokenType::LT, prec::LESSGREATER},
        {TokenType::GT, prec::LESSGREATER},
        {TokenType::PLUS, prec::SUM},
        {TokenType::MINUS, prec::SUM},
        {TokenType::SLASH, prec::PRODUCT},
        {TokenType::ASTERISK, prec::PRODUCT},
        {TokenType::LPAREN, prec::CALL},
        {TokenType::LBRACKET, prec::INDEX}
    };

    class Parser{
    public:
        // 前缀解析函数
        typedef std::shared_ptr<Expression> (Parser::*prefixParseFn)();
        // 中缀解析函数
        typedef std::shared_ptr<Expression> (Parser::*infixParseFn)(std::shared_ptr<Expression>);

        std::map<TokenType, prefixParseFn> prefixParseFns;
        std::map<TokenType, infixParseFn> infixParseFns;

        Parser(std::shared_ptr<Lexer> l) : lexer(l) {
            // 注册前缀解析函数
            registerPrefix(TokenType::IDENT, &Parser::parseIdentifier);
            registerPrefix(TokenType::INT, &Parser::parseIntegerLiteral);
            registerPrefix(TokenType::STRING, &Parser::parseStringLiteral);
            registerPrefix(TokenType::BANG, &Parser::parsePrefixExpression);
            registerPrefix(TokenType::MINUS, &Parser::parsePrefixExpression);
            registerPrefix(TokenType::TRUE, &Parser::parseBoolean);
            registerPrefix(TokenType::FALSE, &Parser::parseBoolean);
            registerPrefix(TokenType::LPAREN, &Parser::parseGroupedExpression);
            registerPrefix(TokenType::LBRACKET, &Parser::parseArrayLiteral);
            registerPrefix(TokenType::IF, &Parser::parseIfExpression);
            registerPrefix(TokenType::FUNCTION, &Parser::parseFunctionLiteral);
            registerPrefix(TokenType::LBRACE, &Parser::parseHashLiteral);

            // 注册中缀解析函数
            registerInfix(TokenType::PLUS, &Parser::parseInfixExpression);
            registerInfix(TokenType::MINUS, &Parser::parseInfixExpression);
            registerInfix(TokenType::SLASH, &Parser::parseInfixExpression);
            registerInfix(TokenType::ASTERISK, &Parser::parseInfixExpression);
            registerInfix(TokenType::EQ, &Parser::parseInfixExpression);
            registerInfix(TokenType::NOT_EQ, &Parser::parseInfixExpression);
            registerInfix(TokenType::LT, &Parser::parseInfixExpression);
            registerInfix(TokenType::GT, &Parser::parseInfixExpression);
            registerInfix(TokenType::LPAREN, &Parser::parseCallExpression);
            registerInfix(TokenType::LBRACKET, &Parser::parseIndexExpression);

            // 读取两个token，设置curToken和peekToken
            nextToken();
            nextToken();
        }

        // 注册前缀解析函数
        void registerPrefix(TokenType tokenType, prefixParseFn fn) {
            prefixParseFns[tokenType] = fn;
        }

        // 注册中缀解析函数
        void registerInfix(TokenType tokenType, infixParseFn fn) {
            infixParseFns[tokenType] = fn;
        }

    private:
        // token 操作
        void nextToken();

        bool curTokenIs(TokenType t);

        bool peekTokenIs(TokenType t);

        bool expectPeek(TokenType t);

        // 报错
        void peekError(TokenType t);

        void noPrefixParseFnError(TokenType t);

        // 优先级辅助函数
        prec peekPrecedence();

        prec curPrecedence();

    public:
        // 解析函数
        // 解析主程序
        std::shared_ptr<Program> parseProgram();

        // 解析语句
        std::shared_ptr<Statement> parseStatement();

        // 解析 let 语句
        std::shared_ptr<LetStatement> parseLetStatement();

        // 解析 return 语句
        std::shared_ptr<ReturnStatement> parseReturnStatement();

        // 解析表达式语句
        std::shared_ptr<ExpressionStatement> parseExpressionStatement();

        // 解析表达式
        std::shared_ptr<Expression> parseExpression(prec precedence);

        // 解析标识符
        std::shared_ptr<Expression> parseIdentifier();

        // 解析整型字面量
        std::shared_ptr<Expression> parseIntegerLiteral();

        // 解析字符串字面量
        std::shared_ptr<Expression> parseStringLiteral();

        // 解析数组字面量
        std::shared_ptr<Expression> parseArrayLiteral();

        // 解析表达式列表
        std::vector<std::shared_ptr<Expression>> parseExpressionList(TokenType end);

        // 解析索引表达式
        std::shared_ptr<Expression> parseIndexExpression(std::shared_ptr<Expression> left);

        // 解析 hash 字面量
        std::shared_ptr<Expression> parseHashLiteral();

        // 解析前缀表达式
        std::shared_ptr<Expression> parsePrefixExpression();

        // 解析中缀表达式
        std::shared_ptr<Expression> parseInfixExpression(std::shared_ptr<Expression> left);

        // 解析布尔值
        std::shared_ptr<Expression> parseBoolean();

        // 解析分组表达式
        std::shared_ptr<Expression> parseGroupedExpression();

        // 解析 if 表达式
        std::shared_ptr<Expression> parseIfExpression();

        // 解析块语句
        std::shared_ptr<BlockStatement> parseBlockStatement();

        // 解析函数字面量
        std::shared_ptr<Expression> parseFunctionLiteral();

        // 解析函数形式参数
        std::vector<std::shared_ptr<Identifier>> parseFunctionParameters();

        // 解析函数调用
        std::shared_ptr<Expression> parseCallExpression(std::shared_ptr<Expression> function);

        // 解析函数调用实参
        std::vector<std::shared_ptr<Expression>> parseCallArguments();

        std::string getErrors();

    private:
        std::shared_ptr<Lexer> lexer;
        std::vector<std::string> errors;
        Token curToken;
        Token peekToken;
    }; // class Parser


} // namespace monkey