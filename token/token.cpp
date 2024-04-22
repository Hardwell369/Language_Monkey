#include "../include/token.h"

namespace monkey {
    TokenType lookupIdent(std::string ident) {
        if (keywords.find(ident) != keywords.end()) {
            return keywords[ident];
        }
        return TokenType::IDENT;
    }
}