#include "emulator.h"
#include "instruction.h"
#include "registers.h"
#include <cstdint>
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
    regs.set_pc(regs.get_pc() + curr_ins.sz);
    // std::cout << "Emulator::emu_loop, new PC: " << registers.get_pc()
    //           << std::endl;
    it += curr_ins.sz;
  }

  return true;
}

bool is_reg(ArgKind kind) {
  return kind > ArgKind::NONE && kind <= ArgKind::LAR;
}

bool is_imm(ArgKind kind) {
  return kind >= ArgKind::imm8 && kind <= ArgKind::imm48;
}

// TODO: May have to expand this to consider memory regs aswell.
bool is_dn(ArgKind kind) { return kind >= ArgKind::D0 && kind <= ArgKind::D3; }

// Sign extend a number of size nbits
reg_type s_ext(reg_type i, uint32_t nbits) {

  if (nbits == 32)
    return i;

  uint32_t s_mask = 2 << (nbits - 2);
  return i | (0 - (i & s_mask));
}

bool Emulator::handle_mov(const Instruction &ins) {
  // TODO: Test
  ArgKind dst = ins.kinds[1];
  ArgKind src = ins.kinds[0];
  // TODO: May have more args

  // 2 registers or imm -> reg
  if (is_reg(dst) && (is_reg(src) || is_imm(src))) {

    // Is this actually true doe
    if (dst == src) {
      std::cerr << "Emulator::handle_mov registers are identical, should be "
                   "impossible. Aborting."
                << std::endl;
      abort();
    }
    // TODO: Maybe special handling for some register types....
    // PSW -> Dn 0 extends upper 16 bits
    // Dn -> PSW ignores upper 16 bits

    reg_type val;
    // Sz (in bits)
    uint8_t sz = get_arg_sz(src) * 8;
    if (is_imm(src)) {
      // Get imm val from args
      // TODO: needs to change based on size of imm. Gotta ensure that we sign
      // ext here as imm needs to be (dun).
      val = *(reg_type *)ins.args;
    } else {
      val = regs.get(src);
    }

    // Only sign extend for Dn
    if (is_dn(dst) &&
        is_imm(src) /*src != ArgKind::PSW && src != ArgKind::MDR*/)
      val = s_ext(val, sz);

    regs.set(dst, val);
    return true;
  }

  // TODO: Handle memory stuff.

  return true;
}

bool Emulator::handle_ext(const Instruction &ins) {

  // EXT only has one arg, always dn.
  ArgKind src = ins.kinds[0];
  // Handle all Ext? variants here
  switch (ins.op) {
  case EXT: {
    // Need to sign extend Dn reg to 64bits and store top 32bits in MDR.
    // No changes to Dn
    // This is simple enough where i can just use default sign ext stuff idc.
    int val = regs.get(src);
    long int ext = val;
    regs.set(ArgKind::MDR, (ext & 0xffffffff00000000) >> 32);
    break;
  }
  case EXTB: {
    signed char val = regs.get(src);
    int ext = val;
    regs.set(src, ext);
    break;
  }
  case EXTBU: {
    // Just zero ext, ez innit
    regs.set(src, regs.get(src) & 0x000000ff);
    break;
  }
  case EXTH: {
    short val = regs.get(src);
    int ext = val;
    regs.set(src, ext);
    break;
  }
  case EXTHU: {
    regs.set(src, regs.get(src) & 0x0000ffff);
    break;
  }
  }
  return true;
}

// Can also handle sub instructions
// TODO: Maybe a little more testing of sub when you can?
bool Emulator::handle_add(const Instruction &ins, int sign = 1) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  // TODO: Extend to include imm
  if ((is_reg(src) || is_imm(src)) && is_reg(dst)) {

    reg_type s;
    reg_type cf_maybe = 0;

    // If we use imm, need to sign extend.
    if (is_imm(src)) {
      uint8_t sz = get_arg_sz(src) * 8;
      s = *(reg_type *)ins.args;
      s = s_ext(s, sz);
    } else {
      s = regs.get(src);
    }

    reg_type d = regs.get(dst);

    reg_type psw = regs.get(ArgKind::PSW);

    // Need to also add the carry flag if we have addc
    if (ins.op == ADDC || ins.op == SUBC) {
      std::cout << insn_to_str((InsnType)ins.op) << " -> adding carry flag"
                << std::endl;
      // TODO: Not sure this is the way we r meant to do this lol.
      cf_maybe = ((psw & PswBits::C) >> 2) * sign;
    }
    // reg_type res = s + (d * sign) + cf_maybe;
    reg_type res = d + (s * sign) + cf_maybe;

    // Now do PSW shenanigans
    // Handle overflow.
    // 8 byte so we can detect ovf
    // bool cnd = ((uint64_t)s + (uint64_t)(d * sign) + cf_maybe) > UINT32_MAX;
    bool cnd = ((uint64_t)d + (uint64_t)(s * sign) + cf_maybe) > UINT32_MAX;
    // Mask off old bits, shouldnt carry over to the next addition.
    psw &= ~0b1111;
    psw |= (PswBits::V * cnd);
    cnd = res == 0;
    psw |= (PswBits::Z * cnd);
    cnd = (int)res < 0;
    psw |= (PswBits::N * cnd);
    // Now, how to do the carry.
    cnd = (s & CARRY_BIT && d & CARRY_BIT);
    psw |= (PswBits::C * cnd);

    regs.set(ArgKind::PSW, psw);
    regs.set(dst, res);
  }

  return true;
}
// TODO: Figure out tf we meant to be doing if it mulu idk what todo
bool Emulator::handle_mul(const Instruction &ins) {
  // TODO: Test a bunch
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  uint64_t res;
  // Registers are the only operands possible
  reg_type s = regs.get(src);
  reg_type d = regs.get(dst);
  reg_type psw = regs.get(ArgKind::PSW);

  res = s * d;

  bool cnd = (res & 0xffffffff) == 0;
  // Only effects the N and Z from what i can see
  // TODO: Test PSW stuff here
  psw &= ~0b11;
  psw |= (PswBits::Z * cnd);
  cnd = (int)(res & 0xffffffff) < 0;
  psw |= (PswBits::N * cnd);

  regs.set(ArgKind::MDR, res >> 32);
  regs.set(dst, res & 0xffffffff);
  regs.set(ArgKind::PSW, psw);
  return true;
}
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
  case EXTB:
  case EXTBU:
  case EXTH:
  case EXTHU:
  case EXT:
    // Handle EXT instruction
    // Think this is done???
    handle_ext(ins);
    break;
  case CLR: {
    // Handle CLR instruction
    // Doesnt rlly need a handler lol
    regs.set(ins.kinds[0], 0);
    // Must also set flags
    reg_type psw = regs.get(ArgKind::PSW);
    psw |= PswBits::Z;
    psw &= ~PswBits::N;
    psw &= ~PswBits::C;
    psw &= ~PswBits::V;
    regs.set(ArgKind::PSW, psw);
    break;
  }
  case ADDC:
  case ADD:
    handle_add(ins);
    // Handle ADD instruction
    break;
  case INC:
    // Handle INC instruction
    break;
  case INC4:
    // Handle INC4 instruction
    break;
  case SUBC:
  case SUB:
    // Handle SUB instruction
    handle_add(ins, -1);
    break;
  case MULU:
  case MUL:
    // Handle MUL instruction
    handle_mul(ins);
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
