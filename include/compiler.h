#pragma once

#include "./ast.h"
#include "./code.h"
#include "./symbol.h"
#include "./builtins.h"

namespace monkey {
    struct ByteCode {
        Instructions instructions;
        std::shared_ptr<Constants> constants;

        ByteCode(Instructions ins, std::shared_ptr<Constants> cons) : instructions(ins), constants(cons) {}
    }; // struct ByteCode

    struct EmittedInstruction {
        Opcode op;
        int position;
    }; // struct EmittedInstruction

    struct CompilerScope {
        Instructions instructions;
        EmittedInstruction lastInstruction;
        EmittedInstruction previousInstruction;
    };
    

    class Compiler {
    public:
        Compiler() {
            scopeIndex = 0;
            scopes.emplace_back(CompilerScope());
            constants = std::make_shared<Constants>();
            symbolTable = std::make_shared<SymbolTable>();
            for (size_t i = 0; i < builtins.size(); ++i) {
                symbolTable->DefineBuiltin(i, builtins[1].name);
            }
        } 
        Compiler(std::shared_ptr<SymbolTable> symTable){
            scopeIndex = 0;
            scopes.emplace_back(CompilerScope());
            constants = std::make_shared<Constants>();
            symbolTable = symTable;
        }

        ~Compiler() = default;

        std::shared_ptr<Compiler> NewWithState(std::shared_ptr<SymbolTable> symTable, std::shared_ptr<std::vector<std::shared_ptr<Object>>> constants) {
            scopeIndex = 0;
            scopes.emplace_back(CompilerScope());
            constants = constants;
            symbolTable = symTable;
            return std::make_shared<Compiler>(*this);
        }

        // std::shared_ptr<SymbolTable> GetSymbolTable() { return symbolTable; } // debug

        void Compile(std::shared_ptr<Node> node);

        Instructions currentInstructions() {
            return scopes[scopeIndex].instructions;
        }

        void enterScope() {
            CompilerScope scope;
            scopes.emplace_back(scope);
            ++scopeIndex;
            symbolTable = NewEnclosedSymbolTable(symbolTable);
        }

        Instructions leaveScope() {
            auto instructions = currentInstructions();
            scopes.pop_back();
            --scopeIndex;
            symbolTable = symbolTable->GetOuter();
            return instructions;
        }

        std::shared_ptr<ByteCode> Bytecode() {
            ByteCode byte_code(currentInstructions(), constants);
            return std::make_shared<ByteCode>(byte_code);
        }

        int addConstant(std::shared_ptr<Object> obj) {
            constants->emplace_back(obj);
            return constants->size() - 1;
        }

        int emit(Opcode op, std::vector<int> operands = {}) {
            std::vector<uint16_t> uint16_operands;
            for (auto operand : operands) {
                uint16_operands.emplace_back(static_cast<uint16_t>(operand));
            }
            auto ins = Make(op, uint16_operands);
            auto pos = addInstruction(ins);
            setLastInstruction(op, pos);
            return pos;
        }

        int addInstruction(Instructions ins) {
            auto instructions = currentInstructions();
            auto pos_new_instruction = instructions.size();
            for (auto i : ins) {
                instructions.emplace_back(i);
            }
            scopes[scopeIndex].instructions = instructions;
            return pos_new_instruction;
        }

        void setLastInstruction(Opcode op, int pos) {
            auto previous = scopes[scopeIndex].lastInstruction;
            auto last = EmittedInstruction{op, pos};
            scopes[scopeIndex].previousInstruction = previous;
            scopes[scopeIndex].lastInstruction = last;
        }

        void removeLastInstruction() {
            auto instructions = currentInstructions();
            auto previous = scopes[scopeIndex].previousInstruction;
            auto last = scopes[scopeIndex].lastInstruction;
            instructions.reserve(last.position);
            scopes[scopeIndex].instructions = instructions;
            scopes[scopeIndex].lastInstruction = previous;
        }

        void replaceInstruction(int pos, Instructions newInstruction) {
            auto instructions = currentInstructions();
            for (auto i : newInstruction) {
                instructions[pos++] = i;
            }
            scopes[scopeIndex].instructions = instructions;
        }

        void changeOperand(int opPos, int operand) {
            auto uint16_operand = static_cast<uint16_t>(operand);
            auto op = static_cast<Opcode>(currentInstructions()[opPos]);
            auto newInstruction = Make(op, {uint16_operand});
            replaceInstruction(opPos, newInstruction);
        }

        void replaceLastPopWithReturn() {
            auto lastPos = scopes[scopeIndex].lastInstruction.position;
            replaceInstruction(lastPos, Make(OpReturnValue, {}));
            scopes[scopeIndex].lastInstruction.op = OpReturnValue;
        }

        void loadSymbol(const Symbol& symbol) {
            auto scope = symbol.scope;
            if (scope == GlobalScope) {
                emit(OpGetGlobal, {symbol.index});
            } else if (scope == LocalScope) {
                emit(OpGetLocal, {symbol.index});
            } else if (scope == BuiltinScope) {
                emit(OpGetBuiltin, {symbol.index});
            } else if (scope == FreeScope) {
                emit(OpGetFree, {symbol.index});
            } else if (scope == FunctionScope) {
                emit(OpCurrentClosure);
            }
        }

    private:
        int scopeIndex;
        std::vector<CompilerScope> scopes;
        std::shared_ptr<Constants> constants;
        std::shared_ptr<SymbolTable> symbolTable;
    }; // class Compiler
} // namespace monkey