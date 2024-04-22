#include "../include/symbol.h"

namespace monkey {
    Symbol SymbolTable::Define(const std::string &name) {
        Symbol symbol(name, numDefinitions);
        if (outer == nullptr) {
            symbol.scope = GlobalScope;
        } else {
            symbol.scope = LocalScope;
        }
        store[name] = symbol;
        ++numDefinitions;
        return symbol;
    }

    Symbol SymbolTable::DefineFree(Symbol original_symbol) {
        freesymbols.push_back(original_symbol);
        auto symbol = Symbol(original_symbol.name, FreeScope, freesymbols.size() - 1);
        store[symbol.name] = symbol;
        return symbol;
    }

    bool SymbolTable::Resolve(const std::string &name, Symbol& symbol) {
        auto it = store.find(name);
        if (it != store.end()) {
            symbol = it->second; 
            return  true;
        }
        if (outer != nullptr) {
            auto flag = outer->Resolve(name, symbol);
            if (!flag) {
                return false;
            }
            if (symbol.scope == GlobalScope || symbol.scope == BuiltinScope) {
                return true;
            }
            auto free = DefineFree(symbol);
            symbol = free;
            return true;
        }
        return false;
    }

    Symbol SymbolTable::DefineBuiltin(int index, const std::string &name) {
        Symbol symbol(name, BuiltinScope, index);
        store[name] = symbol;
        return symbol;
    }

    Symbol SymbolTable::DefineFunctionName(const std::string &name) {
        Symbol symbol(name, FunctionScope, 0);
        store[name] = symbol;
        return symbol;
    }


    std::shared_ptr<SymbolTable> NewEnclosedSymbolTable(std::shared_ptr<SymbolTable> outer) {
        std::shared_ptr<SymbolTable> inner = std::make_shared<SymbolTable>();
        inner->SetOuter(outer);
        return inner;
    }
} // namespace monkey