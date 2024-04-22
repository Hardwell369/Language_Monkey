#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <memory>
#include <iomanip>

#include "./define.h"
#include "./errors.h"

namespace monkey{
    // define op code
    const Opcode OpConstant = 0;
    const Opcode OpPop = 1;
    const Opcode OpAdd = 2;
    const Opcode OpSub = 3;
    const Opcode OpMul = 4;
    const Opcode OpDiv = 5;
    const Opcode OpTrue = 6;
    const Opcode OpFalse = 7;
    const Opcode OpEqual = 8;
    const Opcode OpNotEqual = 9;
    const Opcode OpGreaterThan = 10;
    const Opcode OpMinus = 11;
    const Opcode OpBang = 12;
    const Opcode OpJumpNotTruthy = 13;
    const Opcode OpJump = 14;
    const Opcode OpNull = 15;
    const Opcode OpSetGlobal = 16;
    const Opcode OpGetGlobal = 17;
    const Opcode OpGetLocal = 18;
    const Opcode OpSetLocal = 19;
    const Opcode OpArray = 20;
    const Opcode OpHash = 21;
    const Opcode OpIndex = 22;
    const Opcode OpCall = 23;
    const Opcode OpReturnValue = 24;
    const Opcode OpReturn = 25;
    const Opcode OpGetBuiltin = 26;
    const Opcode OpClosure = 27;
    const Opcode OpGetFree = 28;
    const Opcode OpCurrentClosure = 29;

    // helper function
    struct Defination {
        std::string Name;
        std::vector<width_t> OperandWidths;
    };

    static std::map<Opcode, Defination> definations = {
        {OpConstant, {"OpConstant", {2}}},
        {OpPop, {"OpPop", {}}},
        {OpAdd, {"OpAdd", {}}},
        {OpSub, {"OpSub", {}}},
        {OpMul, {"OpMul", {}}},
        {OpDiv, {"OpDiv", {}}},
        {OpTrue, {"OpTrue", {}}},
        {OpFalse, {"OpFalse", {}}},
        {OpEqual, {"OpEqual", {}}},
        {OpNotEqual, {"OpNotEqual", {}}},
        {OpGreaterThan, {"OpGreaterThan", {}}},
        {OpMinus, {"OpMinus", {}}},
        {OpBang, {"OpBang", {}}},
        {OpJumpNotTruthy, {"OpJumpNotTruthy", {2}}},
        {OpJump, {"OpJump", {2}}},
        {OpNull, {"OpNull", {}}},
        {OpSetGlobal, {"OpSetGlobal", {2}}},
        {OpGetGlobal, {"OpGetGlobal", {2}}},
        {OpGetLocal, {"OpGetLocal", {1}}},
        {OpSetLocal, {"OpSetLocal", {1}}},
        {OpArray, {"OpArray", {2}}},
        {OpHash, {"OpHash", {2}}},
        {OpIndex, {"OpIndex", {}}},
        {OpCall, {"OpCall", {1}}},
        {OpReturnValue, {"OpReturnValue", {}}},
        {OpReturn, {"OpReturn", {}}},
        {OpGetBuiltin, {"OpGetBuiltin", {1}}},
        {OpClosure, {"OpClosure", {2, 1}}},
        {OpGetFree, {"OpGetFree", {1}}},
        {OpCurrentClosure, {"OpCurrentClosure", {}}},
    };

    inline
    std::shared_ptr<Defination> Lookup(byte op) {
        if (definations.find(op) == definations.end()) {
            // std::cerr << "Unknown opcode: " << op << std::endl;
            return nullptr;
        }
        return std::make_shared<Defination>(definations[op]);
    }

    std::string InstructionsToString(Instructions &ins);

    std::string FmtInstruction(std::shared_ptr<Defination> def, std::vector<uint16_t> operands);

    // make instruction
    std::vector<byte> Make(Opcode op, std::vector<uint16_t> operands);

    // read instruction
    std::vector<uint16_t> ReadOperands(std::shared_ptr<Defination> def, Instructions &ins, offset_t init_offset);

    inline
    void PutUint16(Instructions &ins, uint16_t val) {
        // big endian
        ins.emplace_back((val >> 8) & 0xff);
        ins.emplace_back(val & 0xff);
    }

    inline
    void PutUint8(Instructions &ins, uint16_t val) {
        ins.emplace_back(static_cast<uint8_t>(val));
    }

    inline
    uint16_t ReadUint8(Instructions &ins, offset_t offset) {
        return static_cast<uint16_t>(ins[offset]);
    }

    inline
    uint16_t ReadUint16(Instructions &ins, offset_t offset) {
        return uint16_t(ins[offset]) << 8 | uint16_t(ins[offset + 1]);
    }

}