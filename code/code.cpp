#include "../include/code.h"

namespace monkey {
    std::string InstructionsToString(Instructions &ins) {
        std::stringstream result;
        for (int i = 0; i < ins.size(); ++i) {
            auto def = Lookup(ins[i]);
            if (def == nullptr) {
                throw CodeError{"Unknown opcode in 'InstructionsToString()'\n"};
            }
            auto operands = ReadOperands(def, ins, i+1);
            result << std::setw(4) << std::setfill('0') << i << " " 
                    << FmtInstruction(def, operands) << "\n";
            i += def->OperandWidths.size();
        }
        return result.str();
    }

    std::string FmtInstruction(std::shared_ptr<Defination> def, std::vector<uint16_t> operands) {
        std::stringstream result;
        auto operandCount = def->OperandWidths.size();
        if (operands.size() != operandCount) {
            result << "ERROR: operand len " << operands.size() << " mismatch defined " << operandCount << "\n";
            return result.str();
        }
        switch (operandCount) {
            case 0:
                result << def->Name;
                break;
            case 1:
                result << def->Name << " " << operands[0];
                break;
            default:
                result << "ERROR: unhandled operandCount for " << def->Name << "\n";
                break;
        }
        return result.str();
    }

    Instructions Make(Opcode op, std::vector<uint16_t> operands) {
        if (definations.find(op) == definations.end()) {
            return std::vector<byte>();
        }
        auto def = definations[op];
        Instructions ins;
        ins.emplace_back(op);
        width_t width;
        for (int i = 0; i < operands.size(); ++i) {
            auto operand = operands[i];
            width = def.OperandWidths[i];
            switch (width) {
                case 0:
                    break;
                case 1:
                    PutUint8(ins, operand);
                    break;
                case 2:
                    PutUint16(ins, operand);
                    break;
                default:
                    throw CodeError{"Unknown width " + std::to_string(width) + " in 'Make()'\n"}; 
            }
        }
        return ins;
    }

    std::vector<uint16_t> ReadOperands(std::shared_ptr<Defination> def, Instructions &ins, offset_t start) {
        if (def == nullptr) {
            return std::vector<uint16_t>();
        }
        std::vector<uint16_t> operands;
        offset_t offset = start;
        for (int i = 0; i < def->OperandWidths.size(); ++i) {
            width_t width = def->OperandWidths[i];
            switch (width) {
                case 2:
                    operands.emplace_back(ReadUint16(ins, offset));
                    break;
                case 1:
                    operands.emplace_back(ReadUint8(ins, offset));
                    break;
                default:
                    throw CodeError{"Unknown width " + std::to_string(width) + " in 'ReadOperands()'\n"}; 
            }
            offset += width;
        }
        return operands;
    }
} // namespace monkey