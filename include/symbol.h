# pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace monkey {
    using SymbolScope = std::string;
    const SymbolScope GlobalScope = "GLOBAL";
    const SymbolScope LocalScope = "LOCAL";
    const SymbolScope BuiltinScope = "BUILTIN";
    const SymbolScope FreeScope = "FREE";
    const SymbolScope FunctionScope = "FUNCTION";

    struct Symbol {
        std::string name;
        SymbolScope scope;
        int index;

        Symbol() = default;
        Symbol(std::string name, int index) : name(name), index(index) {}
        Symbol(std::string name, SymbolScope scope, int index) : name(name), scope(scope), index(index) {}
        Symbol(const Symbol& s) {
            name = s.name;
            scope = s.scope;
            index = s.index;
        }
        Symbol& operator=(const Symbol& s) {
            name = s.name;
            scope = s.scope;
            index = s.index;
            return *this;
        }
    }; // struct Symbol

    class SymbolTable {
    public:
        SymbolTable() = default;

        SymbolTable(const SymbolTable& s) {
            store = s.store;
            numDefinitions = s.numDefinitions;
        }
        ~SymbolTable() = default;

        SymbolTable& operator=(const SymbolTable& s) {
            store = s.store;
            numDefinitions = s.numDefinitions;
            return *this;
        }

        Symbol Define(const std::string &name);
        Symbol DefineFree(Symbol symbol);
        bool Resolve(const std::string &name, Symbol& symbol);
        Symbol DefineBuiltin(int index, const std::string &name);
        Symbol DefineFunctionName(const std::string &name);
        int GetNumDefinitions() const { return numDefinitions; }
        void SetOuter(std::shared_ptr<SymbolTable> outer) { 
            this->outer = outer;
        }
        void SetFreeSymbols(std::vector<Symbol> free) { 
            this->freesymbols = free;
        }
        std::shared_ptr<SymbolTable> GetOuter() const { return outer; }
        std::vector<Symbol> GetFreeSymbols() const { return freesymbols; }
        std::map<std::string, Symbol> GetStore() const { return store; } // debug

    private:
        std::map<std::string, Symbol> store;
        int numDefinitions = 0;  // the number of definitions in the symbol table
        std::shared_ptr<SymbolTable> outer = nullptr;
        std::vector<Symbol> freesymbols;
    }; // class SymbolTable

    std::shared_ptr<SymbolTable> NewEnclosedSymbolTable(std::shared_ptr<SymbolTable> outer);
} // namespace monkey