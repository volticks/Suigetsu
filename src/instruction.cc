#include "instruction.h"

const char *insn_to_str(InsnType type) {
  switch (type) {
  case MOV:
    return "MOV";
  case MOVBU:
    return "MOVBU";
  case MOVHU:
    return "MOVHU";
  case MOVM:
    return "MOVM";
  case EXT:
    return "EXT";
  case EXTB:
    return "EXTB";
  case EXTBU:
    return "EXTBU";
  case EXTH:
    return "EXTH";
  case EXTHU:
    return "EXTHU";
  case CLR:
    return "CLR";
  case ADD:
    return "ADD";
  case ADDC:
    return "ADDC";
  case INC:
    return "INC";
  case INC4:
    return "INC4";
  case SUB:
    return "SUB";
  case SUBC:
    return "SUBC";
  case MUL:
    return "MUL";
  case MULU:
    return "MULU";
  case DIV:
    return "DIV";
  case DIVU:
    return "DIVU";
  case CMP:
    return "CMP";
  case OR:
    return "OR";
  case AND:
    return "AND";
  case NOT:
    return "NOT";
  case XOR:
    return "XOR";
  case BTST:
    return "BTST";
  case BSET:
    return "BSET";
  case BCLR:
    return "BCLR";
  case ASR:
    return "ASR";
  case LSR:
    return "LSR";
  case ASL:
    return "ASL";
  case ASL2:
    return "ASL2";
  case ROR:
    return "ROR";
  case ROL:
    return "ROL";
  case Bcc:
    return "Bcc";
  case Lcc:
    return "Lcc";
  case JMP:
    return "JMP";
  case JSR_1:
    return "JSR_1";
  case CALL:
    return "CALL";
  case CALLS:
    return "CALLS";
  case TRAP:
    return "TRAP";
  case RET:
    return "RET";
  case RETF:
    return "RETF";
  case RETS:
    return "RETS";
  case RTS_1:
    return "RTS_1";
  case RTI:
    return "RTI";
  case NOP:
    return "NOP";
  case UDFnn:
    return "UDFnn";
  case UDFUnn:
    return "UDFUnn";
  case NONE:
    return "NONE";
  case CLR_OR_MOV_S0:
    return "CLR_OR_MOV_S0";
  // Branch variants
  case BLT:
    return "BLT";
  case BGT:
    return "BGT";
  case BGE:
    return "BGE";
  case BLE:
    return "BLE";
  case BCS:
    return "BCS";
  case BHI:
    return "BHI";
  case BCC:
    return "BCC";
  case BLS:
    return "BLS";
  case BEQ:
    return "BEQ";
  case BNE:
    return "BNE";
  case BRA:
    return "BRA";
  // Loop instruction execution instructions
  case LLT:
    return "LLT";
  case LGT:
    return "LGT";
  case LGE:
    return "LGE";
  case LLE:
    return "LLE";
  case LCS:
    return "LCS";
  case LHI:
    return "LHI";
  case LCC:
    return "LCC";
  case LLS:
    return "LLS";
  case LEQ:
    return "LEQ";
  case LNE:
    return "LNE";
  case LRA:
    return "LRA";
  case SETLB:
    return "SETLB";
  case BVC:
    return "BVC";
  case BVS:
    return "BVS";
  case BNC:
    return "BNC";
  case BNS:
    return "BNS";
  default:
    return "UNKNOWN";
  }
}

const char *arg_kind_to_str(ArgKind arg) {
  switch (arg) {
  case ArgKind::D0:
    return "D0";
  case ArgKind::D1:
    return "D1";
  case ArgKind::D2:
    return "D2";
  case ArgKind::D3:
    return "D3";
  case ArgKind::A0:
    return "A0";
  case ArgKind::A1:
    return "A1";
  case ArgKind::A2:
    return "A2";
  case ArgKind::A3:
    return "A3";
  case ArgKind::PC:
    return "PC";
  case ArgKind::SP:
    return "SP";
  case ArgKind::MDR:
    return "MDR";
  case ArgKind::PSW:
    return "PSW";
  case ArgKind::LIR:
    return "LIR";
  case ArgKind::LAR:
    return "LAR";
  case ArgKind::imm8:
    return "imm8";
  case ArgKind::imm16:
    return "imm16";
  case ArgKind::imm32:
    return "imm32";
  case ArgKind::imm40:
    return "imm40";
  case ArgKind::imm48:
    return "imm48";
  case ArgKind::regs:
    return "regs";
  case ArgKind::abs16:
    return "abs16";
  case ArgKind::abs32:
    return "abs32";
  case ArgKind::d8:
    return "d8";
  case ArgKind::d16:
    return "d16";
  case ArgKind::d32:
    return "d32";
  case ArgKind::MA0:
    return "MA0";
  case ArgKind::MA1:
    return "MA1";
  case ArgKind::MA2:
    return "MA2";
  case ArgKind::MA3:
    return "MA3";
  case ArgKind::MD0:
    return "MD0";
  case ArgKind::MD1:
    return "MD1";
  case ArgKind::MD2:
    return "MD2";
  case ArgKind::MD3:
    return "MD3";
  default:
    return "UNKNOWN";
  }
}

// Is our argument data?
bool arg_isdata(ArgKind kind) {
  return kind >= ArgKind::imm8 && kind <= ArgKind::d32 || kind == ArgKind::regs;
}

uint32_t get_arg_sz(ArgKind kind) {
  uint32_t sz = 0;
  if (!arg_isdata(kind))
    return sz;
  switch (kind) {
  case ArgKind::regs:
  case ArgKind::imm8:
  case ArgKind::d8:
    sz = 1;
    break;
  case ArgKind::imm16:
  case ArgKind::d16:
  case ArgKind::abs16:
    sz = 2;
    break;
  case ArgKind::imm32:
  case ArgKind::d32:
  case ArgKind::abs32:
    sz = 4;
    break;
  case ArgKind::imm40:
    sz = 5;
    break;
  case ArgKind::imm48:
    sz = 6;
    break;
  }
  return sz;
};
