#pragma once

#include "./code.h"
#include "./compiler.h"
#include "./object.h"

namespace monkey {
    const int StackSize = 2048;
    const int GlobalsSize = 65536;
    const int MaxFrames = 1024;
    static std::shared_ptr<Boolea> True = std::make_shared<Boolea>(true);
    static std::shared_ptr<Boolea> False = std::make_shared<Boolea>(false);
    static std::shared_ptr<Null> null = std::make_shared<Null>();

    struct Frame {
        int ip;
        uint16_t basePointer;
        std::shared_ptr<Closure> cl;

        Frame() : ip(-1), basePointer(-1), cl(nullptr) {}
        Frame(std::shared_ptr<Closure> cl) : ip(-1), basePointer(-1), cl(cl) {}
        Frame(std::shared_ptr<Closure> cl, uint16_t basePointer) : ip(-1), basePointer(basePointer), cl(cl) {}

        Instructions getInstructions() {
            return cl->fn->instructions;
        }
    };

    class VM {
    public:
        VM() {
            sp = 0;
            framesIndex = 1;
            auto mainFn = std::make_shared<CompiledFunction>(Instructions());
            auto mainClosure = std::make_shared<Closure>(mainFn);
            auto mainFrame = std::make_shared<Frame>(mainClosure, 0);
            frames.push_back(mainFrame);
            frames.resize(MaxFrames);
            constants = std::make_shared<Constants>();
            globals = std::make_shared<Globals>(GlobalsSize);
            stack.resize(StackSize);
        }
        VM(std::shared_ptr<ByteCode> bc) : constants(bc->constants) {
            sp = 0;
            framesIndex = 1;
            auto mainFn = std::make_shared<CompiledFunction>(bc->instructions);
            auto mainClosure = std::make_shared<Closure>(mainFn);
            auto mainFrame = std::make_shared<Frame>(mainClosure, 0);
            frames.push_back(mainFrame);
            frames.resize(MaxFrames);
            globals = std::make_shared<Globals>(GlobalsSize);
            stack.resize(StackSize);
        }
        ~VM() = default;

        std::shared_ptr<VM> NewWithGlobalsStore(std::shared_ptr<ByteCode> bc, std::shared_ptr<Globals> s) {
            VM vm(bc);
            vm.globals = s;
            return std::make_shared<VM>(vm);
        }

        // std::shared_ptr<Globals> GetGlobals() { return globals; } //debug

        std::shared_ptr<Object> StackTop() {
            if (sp == 0) {
                return nullptr;
            }
            return stack[sp-1];
        }

        std::shared_ptr<Object> LastPoppedStackElem() {
            return stack[sp];
        }

        void Run();

        void executeBinaryOperation(Opcode op);

        void executeBinaryIntegerOperation(Opcode op, std::shared_ptr<Object> left, std::shared_ptr<Object> right);

        void executeComparison(Opcode op);

        void executeBinaryIntegerComparison(Opcode op, std::shared_ptr<Object> left, std::shared_ptr<Object> right);

        void executeBinaryStringComparison(Opcode op, std::shared_ptr<Object> left, std::shared_ptr<Object> right);

        void executeBinaryStringOperation(Opcode op, std::shared_ptr<Object> left, std::shared_ptr<Object> right);

        void executeBangOperator();

        void executeMinusOperator();

        void executeIndexExpression(std::shared_ptr<Object> left, std::shared_ptr<Object> index);

        void executeArrayIndex(std::shared_ptr<Object> array, std::shared_ptr<Object> index);

        void executeHashIndex(std::shared_ptr<Object> hash, std::shared_ptr<Object> index);

        void executeCall (int numArgs);

        void callFunction(std::shared_ptr<Closure> cl, int numArgs);

        void callBuiltin(std::shared_ptr<Builtin> fn, int numArgs);

        void pushClosure(int constIndex, int numFree);

        std::shared_ptr<Array> buildArray(int sp_start, int sp_end);

        std::shared_ptr<HashTable> buildHash(int sp_start, int sp_end);

        std::shared_ptr<Boolea> nativeBoolToBooleanObject(bool input);

        bool isTruthy(std::shared_ptr<Object> obj);

        void push(std::shared_ptr<Object> obj);

        std::shared_ptr<Object> pop();

        std::shared_ptr<Frame> currentFrame();

        void pushFrame(std::shared_ptr<Frame> frame);

        std::shared_ptr<Frame> popFrame();

    private:
        int sp; // Always points to the next value. Top of stack is stack[sp-1]
        std::shared_ptr<Constants> constants;
        Stack stack;
        std::shared_ptr<Globals> globals;
        int framesIndex;
        std::vector<std::shared_ptr<Frame>> frames;
    }; // class VM
} // namespace monkey