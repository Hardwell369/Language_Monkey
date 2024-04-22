#include "../include/compiler.h"

namespace monkey {
    void Compiler::Compile(std::shared_ptr<Node> node) {
            if (std::dynamic_pointer_cast<Program>(node)) {
                auto program = std::dynamic_pointer_cast<Program>(node);
                for (auto stmt : program->statements) {
                    Compile(stmt);
                }
            } else if (std::dynamic_pointer_cast<LetStatement>(node)) {
                auto let_stmt = std::dynamic_pointer_cast<LetStatement>(node);
                auto symbol = symbolTable->Define(let_stmt->name->value);
                Compile(let_stmt->value);
                if (symbol.scope == GlobalScope) {
                    // std::cout << "Compile: OpSetGlobal " << symbol.name << "\n";  // debug
                    emit(OpSetGlobal, {symbol.index});
                } else {
                    // std::cout << "Compile: OpSetLocal " << symbol.name << "\n";  // debug
                    emit(OpSetLocal, {symbol.index});
                }
            } else if (std::dynamic_pointer_cast<BlockStatement>(node)) {
                auto block_stmt = std::dynamic_pointer_cast<BlockStatement>(node);
                for (auto stmt : block_stmt->statements) {
                    Compile(stmt);
                }
            } else if (std::dynamic_pointer_cast<ReturnStatement>(node)) {
                auto return_stmt = std::dynamic_pointer_cast<ReturnStatement>(node);
                Compile(return_stmt->returnValue);
                emit(OpReturnValue);
            } else if (std::dynamic_pointer_cast<ExpressionStatement>(node)) {
                auto expr_stmt = std::dynamic_pointer_cast<ExpressionStatement>(node);
                Compile(expr_stmt->expression);
                // std::cout << "Compile: OpPop\n";  // debug
                emit(OpPop);
            } else if (std::dynamic_pointer_cast<PrefixExpression>(node)) {
                auto prefix_expr = std::dynamic_pointer_cast<PrefixExpression>(node);
                Compile(prefix_expr->right);
                if (prefix_expr->op == "!") {
                    // std::cout << "Compile: OpBang\n";  // debug
                    emit(OpBang);
                } else if (prefix_expr->op == "-") {
                    // std::cout << "Compile: OpMinus\n";  // debug
                    emit(OpMinus);
                } else {
                    throw CompileError{"unknown operator " + prefix_expr->op};
                }
            } else if (std::dynamic_pointer_cast<InfixExpression>(node)) {
                auto infix_expr = std::dynamic_pointer_cast<InfixExpression>(node);
                if (infix_expr->op == "<") {
                    Compile(infix_expr->right);
                    Compile(infix_expr->left);
                    // std::cout << "Compile: OpGreaterThan\n";  // debug
                    emit(OpGreaterThan);
                    return;
                }
                Compile(infix_expr->left);
                Compile(infix_expr->right);
                // classify the operator
                if (infix_expr->op == "+") {
                    // std::cout << "Compile: OpAdd\n";  // debug
                    emit(OpAdd);
                } else if (infix_expr->op == "-") {
                    // std::cout << "Compile: OpSub\n";  // debug
                    emit(OpSub);
                } else if (infix_expr->op == "*") {
                    // std::cout << "Compile: OpMul\n";  // debug
                    emit(OpMul);
                } else if (infix_expr->op == "/") {
                    // std::cout << "Compile: OpDiv\n";  // debug
                    emit(OpDiv);
                } else if (infix_expr->op == "==") {
                    // std::cout << "Compile: OpEqual\n";  // debug
                    emit(OpEqual);
                } else if (infix_expr->op == "!=") {
                    // std::cout << "Compile: OpNotEqual\n";  // debug
                    emit(OpNotEqual);
                } else if (infix_expr->op == ">") {
                    // std::cout << "Compile: OpGreaterThan\n";  // debug
                    emit(OpGreaterThan);
                } else {
                    throw CompileError{"unknown operator " + infix_expr->op};
                }
            } else if (std::dynamic_pointer_cast<IfExpression>(node)) {
                auto if_expr = std::dynamic_pointer_cast<IfExpression>(node);
                Compile(if_expr->condition);
                // emit OpJumpNotTruthy with a dummy value
                auto jumpNotTruthyPos = emit(OpJumpNotTruthy, {9999});
                Compile(if_expr->consequence);

                auto lastInstruction = scopes[scopeIndex].lastInstruction;
                if (lastInstruction.op == OpPop) {
                    removeLastInstruction();
                }
                auto jumpPos = emit(OpJump, {9999});
                auto afterConsequencePos = currentInstructions().size();
                // std::cerr << "afterConsequencePos: " << afterConsequencePos << std::endl;  // debug
                changeOperand(jumpNotTruthyPos, afterConsequencePos);
                if (if_expr->alternative == nullptr) {
                    emit(OpNull);
                } else {
                    Compile(if_expr->alternative);
                    if (lastInstruction.op == OpPop) {
                        removeLastInstruction();
                    }
                }
                auto afterAlternativePos = currentInstructions().size();
                changeOperand(jumpPos, afterAlternativePos);
                // std::cerr << "afterAlternativePos: " << afterAlternativePos << std::endl;  // debug
            } else if (std::dynamic_pointer_cast<Identifier>(node)) {
                auto ident = std::dynamic_pointer_cast<Identifier>(node);
                Symbol symbol;
                if (!symbolTable->Resolve(ident->value, symbol)) {
                    throw CompileError{"undefined variable " + ident->value};
                }
                loadSymbol(symbol);
            } else if (std::dynamic_pointer_cast<IntegerLiteral>(node)) {
                auto int_lit = std::dynamic_pointer_cast<IntegerLiteral>(node);
                auto integer = std::make_shared<Integer>(int_lit->value);
                // std::cout << "Compile: OpConstant " << integer->inspect() << "\n";  // debug
                emit(OpConstant, {addConstant(integer)});
            } else if (std::dynamic_pointer_cast<StringLiteral>(node)) {
                auto str_lit = std::dynamic_pointer_cast<StringLiteral>(node);
                auto str = std::make_shared<Strin>(str_lit->value);
                // std::cout << "Compile: OpConstant " << str->inspect() << "\n";  // debug
                emit(OpConstant, {addConstant(str)});
            } else if (std::dynamic_pointer_cast<Boolean>(node)) {
                auto boolean = std::dynamic_pointer_cast<Boolean>(node);
                if (boolean->value) {
                    emit(OpTrue);
                } else {
                    emit(OpFalse);
                }
            } else if (std::dynamic_pointer_cast<ArrayLiteral>(node)) {
                auto array = std::dynamic_pointer_cast<ArrayLiteral>(node);
                uint16_t len = 0;
                for (auto elem : array->elements) {
                    Compile(elem);
                    ++len;
                }
                // std::cout << "Compile: OpArray " << array->elements.size() << "\n";  // debug
                emit(OpArray, {len});
            } else if (std::dynamic_pointer_cast<HashLiteral>(node)) {
                auto hash = std::dynamic_pointer_cast<HashLiteral>(node);
                uint16_t len = 0;
                for (auto pair : hash->pairs) {
                    Compile(pair.first);
                    Compile(pair.second);
                    ++len;
                }
                // std::cout << "Compile: OpHash " << hash->pairs.size() << "\n";  // debug
                emit(OpHash, {len});
            } else if (std::dynamic_pointer_cast<IndexExpression>(node)) {
                auto index_expr = std::dynamic_pointer_cast<IndexExpression>(node);
                Compile(index_expr->left);
                Compile(index_expr->index);
                // std::cout << "Compile: OpIndex\n";  // debug
                emit(OpIndex);
            } else if (std::dynamic_pointer_cast<FunctionLiteral>(node)) {
                auto func = std::dynamic_pointer_cast<FunctionLiteral>(node);
                enterScope();
                if (func->name != "") {
                    // std::cerr << "DefineFunctionName: " << func->name << std::endl;  // debug
                    symbolTable->DefineFunctionName(func->name);
                }
                for (auto& param : func->parameters) {
                    symbolTable->Define(param->value);
                }
                Compile(func->body);
                if (scopes[scopeIndex].lastInstruction.op == OpPop) {
                    replaceLastPopWithReturn();
                }
                if (scopes[scopeIndex].lastInstruction.op != OpReturnValue) {
                    emit(OpReturn);
                }
                auto freeSymbols = symbolTable->GetFreeSymbols();
                auto numLocals = symbolTable->GetNumDefinitions();
                auto numParams = func->parameters.size();
                auto instructions = leaveScope();
                for (auto free : freeSymbols) {
                    loadSymbol(free);
                }
                auto compiledFn = std::make_shared<CompiledFunction>(instructions, numLocals, numParams);
                auto fnIndex = addConstant(compiledFn);
                emit(OpClosure, {fnIndex, static_cast<int>(freeSymbols.size())});
            } else if (std::dynamic_pointer_cast<CallExpression>(node)) {
                auto call_expr = std::dynamic_pointer_cast<CallExpression>(node);
                Compile(call_expr->function);
                for (auto& arg : call_expr->arguments) {
                    Compile(arg);
                }
                emit(OpCall, {static_cast<uint16_t>(call_expr->arguments.size())});
            } else {
                throw CompileError{"unknown node type " + node->String()};
            }
        }
} // namespace monkey
