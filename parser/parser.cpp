#include "../include/parser.h"

namespace monkey{
// token 操作
void Parser::nextToken(){
    curToken = peekToken;
    peekToken = lexer->nextToken();
}

bool Parser::curTokenIs(TokenType t){
    return curToken.getType() == t;
}

bool Parser::peekTokenIs(TokenType t){
    return peekToken.getType() == t;
}

bool Parser::expectPeek(TokenType t){
    if (peekTokenIs(t)){
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

// 报错
void Parser::peekError(TokenType t){
    std::string msg = "expected next token to be " + TokenTypeString[t] + ", got " + peekToken.getTypeString() + " instead";
    errors.emplace_back(msg);
}

void Parser::noPrefixParseFnError(TokenType t){
    std::string msg = "no prefix parse function for " + TokenTypeString[t] + " found";
    errors.emplace_back(msg);
}

// 优先级辅助函数
prec Parser::peekPrecedence() {
    auto it = precedences.find(peekToken.getType());
    if (it != precedences.end()){
        return it->second;
    }
    return prec::LOWEST;
}

prec Parser::curPrecedence() {
    auto it = precedences.find(curToken.getType());
    if (it != precedences.end()){
        return it->second;
    }
    return prec::LOWEST;
}

// 解析函数
// 解析主程序
std::shared_ptr<Program> Parser::parseProgram(){
    std::shared_ptr<Program> program = std::make_shared<Program>();
    while (curToken.getType() != TokenType::EOF){
        std::shared_ptr<Statement> stmt = parseStatement();
        if (stmt != nullptr){
            program->statements.push_back(stmt);
        }
        nextToken();
    }
    return program;
}
// 解析语句
std::shared_ptr<Statement> Parser::parseStatement(){
    switch(curToken.getType()){
        case TokenType::LET:
            return parseLetStatement();
        case TokenType::RETURN:
            return parseReturnStatement();
        default:
            return parseExpressionStatement();
    }
}

// 解析 let 语句
std::shared_ptr<LetStatement> Parser::parseLetStatement(){
    std::shared_ptr<LetStatement> stmt = std::make_shared<LetStatement>(curToken);
    if (!expectPeek(TokenType::IDENT)){
        return nullptr;
    }
    stmt->name = std::make_shared<Identifier>(curToken, curToken.getLiteral());
    if (!expectPeek(TokenType::ASSIGN)){
        return nullptr;
    }
    nextToken();
    stmt->value = parseExpression(prec::LOWEST);
    if (std::dynamic_pointer_cast<FunctionLiteral>(stmt->value) != nullptr){
        auto func = std::dynamic_pointer_cast<FunctionLiteral>(stmt->value);
        func->name = stmt->name->value;
    }
    if (peekTokenIs(TokenType::SEMICOLON)){
        nextToken();
    }
    return stmt;
}

// 解析 return 语句
std::shared_ptr<ReturnStatement> Parser::parseReturnStatement(){
    std::shared_ptr<ReturnStatement> stmt = std::make_shared<ReturnStatement>(curToken);
    nextToken();
    stmt->returnValue = parseExpression(prec::LOWEST);
    if (peekTokenIs(TokenType::SEMICOLON)){
        nextToken();
    }
    return stmt;
}

// 解析表达式语句
std::shared_ptr<ExpressionStatement> Parser::parseExpressionStatement(){
    std::shared_ptr<ExpressionStatement> stmt = std::make_shared<ExpressionStatement>(curToken);
    stmt->expression = parseExpression(prec::LOWEST);
    if (peekTokenIs(TokenType::SEMICOLON)){
        nextToken();
    }
    return stmt;
}

// 解析表达式
std::shared_ptr<Expression> Parser::parseExpression(prec precedence){
    auto prefix = prefixParseFns[curToken.getType()];
    if (prefix == nullptr){
        noPrefixParseFnError(curToken.getType());
        return nullptr;
    }
    std::shared_ptr<Expression> leftExp = (this->*prefix)();
    while (!peekTokenIs(TokenType::SEMICOLON) && precedence < peekPrecedence()){
        auto infix = infixParseFns[peekToken.getType()];
        if (infix == nullptr){
            return leftExp;
        }
        nextToken();
        leftExp = (this->*infix)(leftExp);
    }
    return leftExp;
}

// 解析标识符
std::shared_ptr<Expression> Parser::parseIdentifier(){
    return std::make_shared<Identifier>(curToken, curToken.getLiteral());
}

// 解析整型字面量
std::shared_ptr<Expression> Parser::parseIntegerLiteral(){
    std::shared_ptr<IntegerLiteral> lit = std::make_shared<IntegerLiteral>(curToken);
    int value = std::stoi(curToken.getLiteral());
    if (value == 0 && (curToken.getLiteral() != "0")) {
        std::string msg = "could not parse " + curToken.getLiteral() + " as integer";
        errors.emplace_back(msg);
    }
    lit->value = value;
    return lit;
}

// 解析字符串字面量
std::shared_ptr<Expression> Parser::parseStringLiteral(){
    return std::make_shared<StringLiteral>(curToken, curToken.getLiteral());
}

// 解析数组字面量
std::shared_ptr<Expression> Parser::parseArrayLiteral(){
    std::shared_ptr<ArrayLiteral> array = std::make_shared<ArrayLiteral>(curToken);
    array->elements = parseExpressionList(TokenType::RBRACKET);
    return array;
}

// 解析表达式列表
std::vector<std::shared_ptr<Expression>> Parser::parseExpressionList(TokenType end){
    std::vector<std::shared_ptr<Expression>> list;
    if (peekTokenIs(end)){
        nextToken();
        return list;
    }
    nextToken();
    list.push_back(parseExpression(prec::LOWEST));
    while (peekTokenIs(TokenType::COMMA)){
        nextToken();
        nextToken();
        list.push_back(parseExpression(prec::LOWEST));
    }
    if (!expectPeek(end)){
        return std::vector<std::shared_ptr<Expression>>();
    }
    return list;
}

// 解析索引表达式
std::shared_ptr<Expression> Parser::parseIndexExpression(std::shared_ptr<Expression> left){
    std::shared_ptr<IndexExpression> exp = std::make_shared<IndexExpression>(curToken, left);
    nextToken();
    exp->index = parseExpression(prec::LOWEST);
    if (!expectPeek(TokenType::RBRACKET)){
        return nullptr;
    }
    return exp;
}

// 解析 hash 字面量
std::shared_ptr<Expression> Parser::parseHashLiteral(){
    std::shared_ptr<HashLiteral> hash = std::make_shared<HashLiteral>(curToken);
    while (!peekTokenIs(TokenType::RBRACE)){
        nextToken();
        std::shared_ptr<Expression> key = parseExpression(prec::LOWEST);
        if (!expectPeek(TokenType::COLON)){
            return nullptr;
        }
        nextToken();
        std::shared_ptr<Expression> value = parseExpression(prec::LOWEST);
        hash->pairs[key] = value;
        if (!peekTokenIs(TokenType::RBRACE) && !expectPeek(TokenType::COMMA)){
            return nullptr;
        }
    }
    if (!expectPeek(TokenType::RBRACE)){
        return nullptr;
    }
    return hash;
}

// 解析前缀表达式
std::shared_ptr<Expression> Parser::parsePrefixExpression(){
    std::shared_ptr<PrefixExpression> exp = std::make_shared<PrefixExpression>(curToken, curToken.getLiteral());
    nextToken();
    exp->right = parseExpression(prec::PREFIX);
    return exp;
}

// 解析中缀表达式
std::shared_ptr<Expression> Parser::parseInfixExpression(std::shared_ptr<Expression> left){
    std::shared_ptr<InfixExpression> exp = std::make_shared<InfixExpression>(curToken, curToken.getLiteral(), left);
    prec precedence = curPrecedence();
    nextToken();
    exp->right = parseExpression(precedence);
    return exp;
}

// 解析布尔值
std::shared_ptr<Expression> Parser::parseBoolean(){
    return std::make_shared<Boolean>(curToken, curTokenIs(TokenType::TRUE));
}

// 解析分组表达式
std::shared_ptr<Expression> Parser::parseGroupedExpression(){
    nextToken();
    std::shared_ptr<Expression> exp = parseExpression(prec::LOWEST);
    if (!expectPeek(TokenType::RPAREN)){
        return nullptr;
    }
    return exp;
}

// 解析 if 表达式
std::shared_ptr<Expression> Parser::parseIfExpression(){
    std::shared_ptr<IfExpression> exp = std::make_shared<IfExpression>(curToken);
    if (!expectPeek(TokenType::LPAREN)){
        return nullptr;
    }
    nextToken();
    exp->condition = parseExpression(prec::LOWEST);
    if (!expectPeek(TokenType::RPAREN)){
        return nullptr;
    }
    if (!expectPeek(TokenType::LBRACE)){
        return nullptr;
    }
    exp->consequence = parseBlockStatement();
    if (peekTokenIs(TokenType::ELSE)){
        nextToken();
        if (!expectPeek(TokenType::LBRACE)){
            return nullptr;
        }
        exp->alternative = parseBlockStatement();
    }
    return exp;
}

// 解析块语句
std::shared_ptr<BlockStatement> Parser::parseBlockStatement(){
    std::shared_ptr<BlockStatement> block = std::make_shared<BlockStatement>(curToken);
    nextToken();
    while (!curTokenIs(TokenType::RBRACE) && !curTokenIs(TokenType::EOF)){
        std::shared_ptr<Statement> stmt = parseStatement();
        if (stmt != nullptr){
            block->statements.push_back(stmt);
        }
        nextToken();
    }
    return block;
}

// 解析函数字面量
std::shared_ptr<Expression> Parser::parseFunctionLiteral(){
    std::shared_ptr<FunctionLiteral> lit = std::make_shared<FunctionLiteral>(curToken);
    if (!expectPeek(TokenType::LPAREN)){
        return nullptr;
    }
    lit->parameters = parseFunctionParameters();
    if (!expectPeek(TokenType::LBRACE)){
        return nullptr;
    }
    lit->body = parseBlockStatement();
    return lit;
}

// 解析函数形式参数
std::vector<std::shared_ptr<Identifier>> Parser::parseFunctionParameters() {
    std::vector<std::shared_ptr<Identifier>> identifiers;
    if (peekTokenIs(TokenType::RPAREN)){
        nextToken();
        return identifiers;
    }
    nextToken();
    std::shared_ptr<Identifier> ident = std::make_shared<Identifier>(curToken, curToken.getLiteral());
    identifiers.push_back(ident);
    while (peekTokenIs(TokenType::COMMA)){
        nextToken();
        nextToken();
        ident = std::make_shared<Identifier>(curToken, curToken.getLiteral());
        identifiers.push_back(ident);
    }
    if (!expectPeek(TokenType::RPAREN)){
        return std::vector<std::shared_ptr<Identifier>>();
    }
    return identifiers;
}

// 解析函数调用
std::shared_ptr<Expression> Parser::parseCallExpression(std::shared_ptr<Expression> function){
    std::shared_ptr<CallExpression> exp = std::make_shared<CallExpression>(curToken, function);
    exp->arguments = parseCallArguments();
    return exp;
}

// 解析函数调用实参
std::vector<std::shared_ptr<Expression>> Parser::parseCallArguments(){
    return parseExpressionList(TokenType::RPAREN);
}

std::string Parser::getErrors(){
    std::string error_out;
    int i = 0;
    for (auto &e : errors) {
        error_out += std::to_string(++i) + "." + e + "\n";
    }
    return error_out;
}

} // namespace monkey