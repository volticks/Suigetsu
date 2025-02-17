#include "emulator.h"
#include "registers.h"
#include <iostream>
#include <ostream>

bool Emulator::emu_loop(const Instructions &insns) {

  const Instructions::const_iterator begin = insns.begin();
  const Instructions::const_iterator end = insns.end();
  Instructions::const_iterator it = begin;
  bool ret = true;

  Instruction curr_ins;
  while (it < end) {
    decoder.decode_inst(it.base(), end.base(), curr_ins);

    // std::cout << "Emulator::emu_loop decoded, sz: " << (int)curr_ins.sz
    //           << " OP (enum): " << std::hex << (int)curr_ins.op << std::endl;
    //  Instruction decoding would go OOB
    if (curr_ins.sz == 0) {
      std::cerr << "Emulator::emu_loop decoding went OOB OR failed, exiting."
                << std::endl;
      break;
    }
    // Decoding failed due to invalid ins stop execution - disabled for now to
    // enable better deocing testing.
    if (curr_ins.op == NONE) {
      std::cerr << "Emulator::emu_loop invalid instruction, exiting."
                << std::endl;
      ret = false;
      break;
    }

    execute_insn(curr_ins);

    // TODO: Maybe combine these 2 together, dunno how much sense it makes to
    // have pc and iter be 2 seperate things
    registers.set_pc(registers.get_pc() + curr_ins.sz);
    // std::cout << "Emulator::emu_loop, new PC: " << registers.get_pc()
    //           << std::endl;
    it += curr_ins.sz;
  }

  return true;
}

bool Emulator::handle_mov(const Instruction &ins) {}

bool Emulator::execute_insn(const Instruction &ins) {
  // TODO: actual stuff here
  // std::cout << "Emulator::execute_insn doing stuff..." << std::endl;
  //
  switch (ins.op) {
  case MOV:
    // Handle MOV instruction
    handle_mov(ins);
    break;
  case MOVBU:
    // Handle MOVBU instruction
    break;
  case MOVHU:
    // Handle MOVHU instruction
    break;
  case MOVM:
    // Handle MOVM instruction
    break;
  case EXT:
    // Handle EXT instruction
    break;
  case EXTB:
    // Handle EXTB instruction
    break;
  case EXTBU:
    // Handle EXTBU instruction
    break;
  case EXTH:
    // Handle EXTH instruction
    break;
  case EXTHU:
    // Handle EXTHU instruction
    break;
  case CLR:
    // Handle CLR instruction
    break;
  case ADD:
    // Handle ADD instruction
    break;
  case ADDC:
    // Handle ADDC instruction
    break;
  case INC:
    // Handle INC instruction
    break;
  case INC4:
    // Handle INC4 instruction
    break;
  case SUB:
    // Handle SUB instruction
    break;
  case SUBC:
    // Handle SUBC instruction
    break;
  case MUL:
    // Handle MUL instruction
    break;
  case MULU:
    // Handle MULU instruction
    break;
  case DIV:
    // Handle DIV instruction
    break;
  case DIVU:
    // Handle DIVU instruction
    break;
  case CMP:
    // Handle CMP instruction
    break;
  case OR:
    // Handle OR instruction
    break;
  case AND:
    // Handle AND instruction
    break;
  case NOT:
    // Handle NOT instruction
    break;
  case XOR:
    // Handle XOR instruction
    break;
  // Bit testing operations
  case BSET:
    // Handle BSET instruction
    break;
  case BCLR:
    // Handle BCLR instruction
    break;
  case BTST:
    // Handle BTST instruction
    break;
  // Shift and rotate operations
  case ASR_2:
    // Handle ASR_2 instruction
    break;
  case LSR_2:
    // Handle LSR_2 instruction
    break;
  case ASL:
    // Handle ASL instruction
    break;
  case ASL2:
    // Handle ASL2 instruction
    break;
  case ASR:
    // Handle ASR instruction
    break;
  case LSR:
    // Handle LSR instruction
    break;
  case ROR:
    // Handle ROR instruction
    break;
  case ROL:
    // Handle ROL instruction
    break;
  // Control flow operations
  case Bcc:
    // Handle Bcc instruction
    break;
  case Lcc:
    // Handle Lcc instruction
    break;
  case JMP:
    // Handle JMP instruction
    break;
  case JSR_1:
    // Handle JSR_1 instruction
    break;
  case CALL:
    // Handle CALL instruction
    break;
  case CALLS:
    // Handle CALLS instruction
    break;
  case TRAP:
    // Handle TRAP instruction
    break;
  // Return operations
  case RET:
    // Handle RET instruction
    break;
  case RETF:
    // Handle RETF instruction
    break;
  case RETS:
    // Handle RETS instruction
    break;
  case RTS_1:
    // Handle RTS_1 instruction
    break;
  case RTI:
    // Handle RTI instruction
    break;
  // Miscellaneous operations
  case NOP:
    // Handle NOP instruction
    break;
  case UDFnn:
    // Handle UDFnn instruction
    break;
  case UDFUnn:
    // Handle UDFUnn instruction
    break;
  case NONE:
    // Handle NONE instruction
    break;
  case CLR_OR_MOV_S0:
    // Handle CLR_OR_MOV_S0 instruction
    break;
  // Branch variants
  case BLT:
    // Handle BLT instruction
    break;
  case BGT:
    // Handle BGT instruction
    break;
  case BGE:
    // Handle BGE instruction
    break;
  case BLE:
    // Handle BLE instruction
    break;
  case BCS:
    // Handle BCS instruction
    break;
  case BHI:
    // Handle BHI instruction
    break;
  case BCC:
    // Handle BCC instruction
    break;
  case BLS:
    // Handle BLS instruction
    break;
  case BEQ:
    // Handle BEQ instruction
    break;
  case BNE:
    // Handle BNE instruction
    break;
  case BRA:
    // Handle BRA instruction
    break;
  // Loop execution instructions
  case LLT:
    // Handle LLT instruction
    break;
  case LGT:
    // Handle LGT instruction
    break;
  case LGE:
    // Handle LGE instruction
    break;
  case LLE:
    // Handle LLE instruction
    break;
  case LCS:
    // Handle LCS instruction
    break;
  case LHI:
    // Handle LHI instruction
    break;
  case LCC:
    // Handle LCC instruction
    break;
  case LLS:
    // Handle LLS instruction
    break;
  case LEQ:
    // Handle LEQ instruction
    break;
  case LNE:
    // Handle LNE instruction
    break;
  case LRA:
    // Handle LRA instruction
    break;
  case SETLB:
    // Handle SETLB instruction
    break;
  // Additional branch instructions
  case BVC:
    // Handle BVC instruction
    break;
  case BVS:
    // Handle BVS instruction
    break;
  case BNC:
    // Handle BNC instruction
    break;
  case BNS:
    // Handle BNS instruction
    break;
  default:
    // Handle unknown instruction
    break;
  }
  return true;
}
