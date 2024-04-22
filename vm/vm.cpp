#include "../include/vm.h"

namespace monkey {
    void VM::Run() {
        while (currentFrame()->ip < static_cast<int>(currentFrame()->getInstructions().size() - 1)) {
            ++currentFrame()->ip;
            auto& ip = currentFrame()->ip;
            auto instructions = currentFrame()->getInstructions();
            auto op = instructions[ip];
            switch (op) {
                case OpConstant: {
                    auto const_index = ReadUint16(instructions, ip+1);
                    ip += 2;
                    push((*constants)[const_index]);
                    break;
                }
                case OpPop: {
                    pop();
                    break;
                }
                case OpAdd: 
                case OpSub:
                case OpMul:
                case OpDiv:
                    executeBinaryOperation(op);
                    break;
                case OpTrue: {
                    push(True);
                    break;
                }
                case OpFalse: {
                        push(False);
                    break;
                }
                case OpEqual:
                case OpNotEqual:
                case OpGreaterThan:
                    executeComparison(op);
                    break;
                case OpBang: {
                    executeBangOperator();
                    break;
                }
                case OpMinus: {
                        executeMinusOperator();
                    break;
                }
                case OpJump: {
                    auto pos = ReadUint16(instructions, ip+1);
                    ip = pos - 1;
                    // std::cerr << "Jump: " << pos << std::endl;  // debug
                    break;
                }
                case OpJumpNotTruthy: {
                    auto pos = ReadUint16(instructions, ip+1);
                    ip += 2;
                    auto condition = pop();
                    if (!isTruthy(condition)) {
                        ip = pos - 1;
                    }
                    // std::cerr << "JumpNotTruthy: " << pos << std::endl;  // debug
                    break;
                }
                case OpNull: {
                    push(null);
                    break;
                }
                case OpArray: {
                    auto num_elements = ReadUint16(instructions, ip+1);
                    ip += 2;
                    auto array = buildArray(sp-num_elements, sp);
                    sp -= num_elements;
                    push(array);
                    break;
                }
                case OpHash: {
                    auto num_elements = ReadUint16(instructions, ip+1) * 2;
                    ip += 2;
                    auto hashtable = buildHash(sp-num_elements, sp);
                    sp -= num_elements;
                    push(hashtable);
                    // std::cout << "Run: OpHash " << hashtable->inspect() << std::endl; // debug
                    break;
                }
                case OpIndex: {
                    auto index = pop();
                    auto left = pop();
                    executeIndexExpression(left, index);
                    break;
                }
                case OpCall: {
                    auto num_args = ReadUint8(instructions, ip+1);
                    ip += 1;
                    executeCall(num_args);
                    break;
                }
                case OpReturn: {
                    auto frame = popFrame();
                    sp = frame->basePointer - 1;
                    push(null);
                    break;
                }
                case OpReturnValue: {
                    auto return_value = pop();
                    auto frame = popFrame();
                    sp = frame->basePointer - 1;
                    push(return_value);
                    break;
                }
                case OpSetGlobal: {
                    auto global_index = ReadUint16(instructions, ip+1);
                    ip += 2;
                    (*globals)[global_index] = pop();
                    // std::cout << "Run: OpSetGlobal " << (*globals)[global_index]->inspect() << std::endl; // debug
                    break;
                }
                case OpGetGlobal: {
                    auto global_index = ReadUint16(instructions, ip+1);
                    ip += 2;
                    push((*globals)[global_index]);
                    // std::cout << "Run: OpGetGlobal " << (*globals)[global_index]->inspect() << std::endl; // debug
                    break;
                }
                case OpSetLocal: {
                    auto local_index = ReadUint8(instructions, ip+1);
                    ip += 1;
                    auto frame = currentFrame();
                    stack[frame->basePointer + local_index] = pop();
                    // std::cout << "Run: OpSetLocal " << stack[frame->basePointer + local_index]->inspect() << std::endl; // debug
                    break;
                }
                case OpGetLocal: {
                    auto local_index = ReadUint8(instructions, ip+1);
                    ip += 1;
                    auto frame = currentFrame();
                    push(stack[frame->basePointer + local_index]);
                    // std::cout << "Run: OpGetLocal " << stack[frame->basePointer + local_index]->inspect() << std::endl; // debug
                    break;
                }
                case OpGetBuiltin: {
                    auto builtin_index = ReadUint8(instructions, ip+1);
                    ip += 1;
                    auto builtin_def = builtins[builtin_index];
                    push(builtin_def.fn);
                    break;
                }
                case OpGetFree: {
                    auto free_index = ReadUint8(instructions, ip+1);
                    ip += 1;
                    auto current_cl = currentFrame()->cl;
                    push(current_cl->free[free_index]);
                    break;
                }
                case OpCurrentClosure: {
                    auto current_cl = currentFrame()->cl;
                    push(current_cl);
                    break;
                }
                case OpClosure: {
                    auto const_index = ReadUint16(instructions, ip+1);
                    ip += 2;
                    auto num_free = ReadUint8(instructions, ip+1);
                    ip += 1;
                    pushClosure(const_index, num_free);
                    break;
                }
                default:
                    throw RunningError{"unknown opcode"};
            }
        }
    }

    void VM::executeBinaryOperation(Opcode op) {
        auto right = pop();
        auto left = pop();
        if (left->type() == "INTEGER" && right->type() == "INTEGER") {
            executeBinaryIntegerOperation(op, left, right);
            return;
        }
        if (left->type() == "STRING" && right->type() == "STRING") {
            executeBinaryStringOperation(op, left, right);
            return;
        }
        throw RunningError{"unsupported types for binary operation " + left->type() + " and " + right->type()};
    }

    void VM::executeBinaryIntegerOperation(Opcode op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        auto left_val = std::dynamic_pointer_cast<Integer>(left)->value;
        auto right_val = std::dynamic_pointer_cast<Integer>(right)->value;
        std::shared_ptr<Integer> result;
        switch (op) {
            case OpAdd:
                result = std::make_shared<Integer>(left_val + right_val);
                break;
            case OpSub:
                result = std::make_shared<Integer>(left_val - right_val);
                break;
            case OpMul:
                result = std::make_shared<Integer>(left_val * right_val);
                break;
            case OpDiv:
                result = std::make_shared<Integer>(left_val / right_val);
                break;
            default:
                throw RunningError{"unknown integer operation"};
        }
        push(result);
    }

    void VM::executeBinaryStringOperation(Opcode op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        if (op != OpAdd) {
            throw RunningError{"unknown string operation"};
        }
        auto left_val = std::dynamic_pointer_cast<Strin>(left)->value;
        auto right_val = std::dynamic_pointer_cast<Strin>(right)->value;
        auto result = std::make_shared<Strin>(left_val + right_val);
        push(result);
    }

    void VM::executeComparison(Opcode op) {
        auto right = pop();
        auto left = pop();
        if (left->type() == "INTEGER" && right->type() == "INTEGER") {
            executeBinaryIntegerComparison(op, left, right);
            return;
        }
        if (left->type() == "STRING" && right->type() == "STRING") {
            executeBinaryStringComparison(op, left, right);
            return;
        }
        switch (op) {
            case OpEqual:
                push(nativeBoolToBooleanObject(left == right));
                break;
            case OpNotEqual:
                push(nativeBoolToBooleanObject(left != right));
                break;
            default:
                throw RunningError{"unsupported types for binary operation " + left->type() + " and " + right->type()};
        }
    }

    void VM::executeBinaryIntegerComparison(Opcode op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        auto left_val = std::dynamic_pointer_cast<Integer>(left)->value;
        auto right_val = std::dynamic_pointer_cast<Integer>(right)->value;
        std::shared_ptr<Boolea> result;
        switch (op) {
            case OpEqual:
                result = nativeBoolToBooleanObject(left_val == right_val);
                break;
            case OpNotEqual:
                result = nativeBoolToBooleanObject(left_val != right_val);
                break;
            case OpGreaterThan:
                result = nativeBoolToBooleanObject(left_val > right_val);
                break;
            default:
                throw RunningError{"unknown integer comparison operation"};
        }
        push(result);
    }

    void VM::executeBinaryStringComparison(Opcode op, std::shared_ptr<Object> left, std::shared_ptr<Object> right) {
        if (op != OpEqual && op != OpNotEqual) {
            throw RunningError{"unknown string comparison operation"};
        }
        auto left_val = std::dynamic_pointer_cast<Strin>(left)->value;
        auto right_val = std::dynamic_pointer_cast<Strin>(right)->value;
        std::shared_ptr<Boolea> result;
        if (op == OpEqual) {
            result = nativeBoolToBooleanObject(left_val == right_val);
        } else {
            result = nativeBoolToBooleanObject(left_val != right_val);
        }
        push(result);
    }

    void VM::executeBangOperator() {
        auto operand = pop();
        if (operand == True) {
            push(False);
        } else if (operand == False || operand == null) {
            push(True);
        } else if (operand == nullptr) {
            throw RunningError{"operand is null"};
        } else {
            push(False);
        }
    }

    void VM::executeMinusOperator() {
        auto operand = pop();
        if (operand->type() != "INTEGER") {
            throw RunningError{"unsupported type for negation"};
        }
        auto value = std::dynamic_pointer_cast<Integer>(operand)->value;
        push(std::make_shared<Integer>(0-value));
    }

    void VM::executeIndexExpression(std::shared_ptr<Object> left, std::shared_ptr<Object> index) {
        if (left->type() == "ARRAY" && index->type() == "INTEGER") {
            executeArrayIndex(left, index);
            return;
        }
        if (left->type() == "HASH_TABLE") {
            executeHashIndex(left, index);
            return;
        }
        throw RunningError{"index operator not supported: " + left->type()};
    }

    void VM::executeArrayIndex(std::shared_ptr<Object> array, std::shared_ptr<Object> index) {
        auto arr = std::dynamic_pointer_cast<Array>(array);
        auto idx = std::dynamic_pointer_cast<Integer>(index)->value;
        if (idx < 0 || idx >= arr->elements.size()) {
            push(null);
        } else {
            push(arr->elements[idx]);
        }
    }

    void VM::executeHashIndex(std::shared_ptr<Object> hash, std::shared_ptr<Object> index) {
        auto h = std::dynamic_pointer_cast<HashTable>(hash);
        if (index->type() != "STRING" && index->type() != "INTEGER" && index->type() != "BOOLEAN") {
            throw RunningError{"unusable as hash key: " + index->type()};
        }
        // std::cerr << "executeHashIndex: " << index->inspect() << std::endl; // debug
        // std::cerr << "executeHashTable: " << h->inspect() << std::endl; // debug
        if (std::dynamic_pointer_cast<Strin>(index)) {
            auto k = std::dynamic_pointer_cast<Strin>(index);
            auto key = k->hashKey();
            if (h->find(key)){
                push(h->get(key)->value);
            } else {
                push(null);
            }
        } else if (std::dynamic_pointer_cast<Integer>(index)) {
            auto k = std::dynamic_pointer_cast<Integer>(index);
            auto key = k->hashKey();
            if (h->find(key)) {
                push(h->get(key)->value);
            } else {
                push(null);
            }
        } else if (std::dynamic_pointer_cast<Boolea>(index)) {
            auto k = std::dynamic_pointer_cast<Boolea>(index);
            auto key = k->hashKey();
            if (h->find(key)) {
                push(h->get(key)->value);
            } else {
                push(null);
            }
        }
    }

    void VM::executeCall (int numArgs) {
        auto calledFn = stack[sp-1-numArgs];
        auto type = calledFn->type();
        if (type == "CLOSURE") {
            callFunction(std::dynamic_pointer_cast<Closure>(calledFn), numArgs);
            return;
        }
        if (type == "BUILTIN") {
            callBuiltin(std::dynamic_pointer_cast<Builtin>(calledFn), numArgs);
            return;
        }
        throw RunningError{"calling non-function or non-builtin"};
    }

    void VM::callFunction(std::shared_ptr<Closure> cl, int numArgs) {
        auto fn = cl->fn;
        if (numArgs != fn->numParameters) {
            throw RunningError{"wrong number of arguments, want=" + std::to_string(fn->numParameters) + ", got=" + std::to_string(numArgs)};
        }
        auto frame = std::make_shared<Frame>(cl, sp-numArgs);
        pushFrame(frame);
        sp = frame->basePointer + fn->numLocals;
    }

    void VM::callBuiltin(std::shared_ptr<Builtin> fn, int numArgs) {
        std::vector<std::shared_ptr<Object>> args;
        for (int i = sp-numArgs; i < sp; ++i) {
            args.emplace_back(stack[i]);
        }
        auto result = fn->fn(args);
        sp -= numArgs + 1;
        if (result) {
            push(result);
        } else {
            push(null);
        }
    }

    void VM::pushClosure(int const_index, int num_free) {
        auto constant = (*constants)[const_index];
        if (!std::dynamic_pointer_cast<CompiledFunction>(constant)) {
            throw RunningError{"not a function: " + constant->type()};
        } 
        auto compiled_fn = std::dynamic_pointer_cast<CompiledFunction>(constant);
        std::vector<std::shared_ptr<Object>> free;
        for (int i = 0; i < num_free; ++i) {
            free.emplace_back(stack[sp-num_free+i]);
        }
        sp -= num_free;
        auto cl = std::make_shared<Closure>(compiled_fn, free);
        push(cl);
    }

    std::shared_ptr<Array> VM::buildArray(int sp_start, int sp_end) {
        std::vector<std::shared_ptr<Object>> elements;
        for (int i = sp_start; i < sp_end; ++i) {
            elements.emplace_back(stack[i]);
        }
        return std::make_shared<Array>(elements);
    }

    std::shared_ptr<HashTable> VM::buildHash(int sp_start, int sp_end) {
        std::map<std::shared_ptr<HashKey>, std::shared_ptr<HashPair>> pairs;
        for (int i = sp_start; i < sp_end; i += 2) {
            auto key = stack[i];
            auto value = stack[i+1];
            auto pair = std::make_shared<HashPair>(key, value);
            if (key->type() != "STRING" & key->type() != "INTEGER" & key->type() != "BOOLEAN") {
                throw RunningError{"unusable as hash key: " + key->type()};
            }
            if (std::dynamic_pointer_cast<Strin>(key)) {
                auto k = std::dynamic_pointer_cast<Strin>(key);
                pairs[k->hashKey()] = pair;
            } else if (std::dynamic_pointer_cast<Integer>(key)) {
                auto k = std::dynamic_pointer_cast<Integer>(key);
                pairs[k->hashKey()] = pair;
            } else if (std::dynamic_pointer_cast<Boolea>(key)) {
                auto k = std::dynamic_pointer_cast<Boolea>(key);
                pairs[k->hashKey()] = pair;
            }
        }
        return std::make_shared<HashTable>(pairs);
    }

    std::shared_ptr<Boolea> VM::nativeBoolToBooleanObject(bool input) {
        if (input) {
            return True;
        }
        return False;
    }

    bool VM::isTruthy(std::shared_ptr<Object> obj) {
        if (std::dynamic_pointer_cast<Boolea>(obj)) {
            auto b = std::dynamic_pointer_cast<Boolea>(obj);
            return b->value;
        }
        if (std::dynamic_pointer_cast<Null>(obj)) {
            return false;
        }
        return true;
    }

    void VM::push(std::shared_ptr<Object> obj) {
        if (sp >= StackSize) {
            throw RunningError{"stack overflow"};
        }
        stack[sp++] = obj;
    }

    std::shared_ptr<Object> VM::pop() {
        if (sp == 0) {
            return nullptr;
        }
        auto obj = stack[--sp];
        return obj;
    }

    std::shared_ptr<Frame> VM::currentFrame() {
        return frames[framesIndex-1];
    }

    void VM::pushFrame(std::shared_ptr<Frame> frame) {
        if (framesIndex >= MaxFrames) {
            throw RunningError{"frames overflow"};
        }
        frames[framesIndex++] = frame;
    }

    std::shared_ptr<Frame> VM::popFrame() {
        if (framesIndex == 0) {
            return nullptr;
        }
        return frames[--framesIndex];
    }
} // namespace monkey