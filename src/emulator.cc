#include "emulator.h"
#include "instruction.h"
#include "mmu.h"
#include "registers.h"
#include <bit>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <ostream>

// TODO: Since we adding jumps and stuff we need to re-evaluate how we do this
// shit also since end conditions wont be the same among other things.
// TODO: This doesnt rlly need to take the insns anymo.
bool Emulator::emu_loop(const Instructions &insns, virt_addr start) {

  // const Instructions::const_iterator begin = insns.begin();
  // const Instructions::const_iterator end = insns.end();
  // Instructions::const_iterator it = begin;
  bool ret = true;

  Instruction curr_ins;
  virt_addr curr = start;
  page_entry cached = mmu.get_pd().get_pte_from_vaddr(start);
  // Latter page in case a split is needed when jumping
  page_entry cached_next;
  // Naming convention is a lil confusing here, basically the former part of the
  // split - if thats not the current page.
  page_entry cached_next_prev;
  reg_type pc = 0, oldpc = 0;
  inst_data *real_ins = (inst_data *)((cached.page_addr << page_shift));
  inst_data *real_ins_cpy = real_ins;

  inst_data decode_spillover[max_ins * 2] = {0};
  addr new_addr = 0;
  bool bad_next = false;
  bool bad = false;
  // If we do go oob, need to move to next page specified
  bool use_next = false;

  regs.set_pc(start);

  while (true) {
    // TODO: Maybe some kinda page caching to make this less expensive?
    oldpc = pc;
    pc = regs.get_pc();
    if (pc < curr || pc >> page_shift != oldpc >> page_shift || pc + max_ins > (curr + page_size) /*||
        (pc & page_mask) != (oldpc & page_mask)*/) {
      // Outside current cached page, need to re-check new page.
      try {
        cached_next = mmu.get_pd().get_pte_from_vaddr(pc + max_ins);
        if (!this->mmu.is_rx(cached_next)) {
          bad = true;
          throw PageException("Cant exec page: ", pc + max_ins);
        }

        // If the first part of the split isnt on the current page, check the
        // page it IS on.
        if (pc >= curr + page_size || pc <= (curr & ~(page_size - 1))) {
          cached_next_prev = mmu.get_pd().get_pte_from_vaddr(pc);

          new_addr = cached_next_prev.page_addr;

          // We gud,
          real_ins = (inst_data *)(new_addr << 12) + (pc & (page_size - 1));
        }
      } catch (PageException &pex) {
        // Not present
        bad_next = true;

        std::cerr << "Emulator::emu_loop threw: " << pex.what() << std::endl;

        // In this case we straight up just cannot execute at this address.
        if (bad) {
          std::cerr << "Emulator::emu_loop page not executable, bailing"
                    << std::endl;
        }
        break;
      }
      // Not readable or executable, bail if we access.
      if (!this->mmu.is_rx(cached) || !this->mmu.is_rx(cached_next_prev))
        bad_next = true;

      // TODO: Get this finished.
      uint32_t end_dist = ((pc + page_size) & ~(page_size - 1)) - pc;
      if (!bad_next && end_dist < max_ins) {
        // This handles cases where an instruction carries over to the next
        // page, so we just end up copying the other part of what is/could be
        // part of the instruction to a buffer and using that for the rest of
        // this cycle.
        // TODO: Mention this in the report.
        uint32_t read1 = end_dist;
        uint32_t read2 = (pc + max_ins) - (pc + read1);
        std::memcpy(decode_spillover, real_ins, read1);
        std::memcpy(decode_spillover + read1,
                    (inst_data *)(cached_next.page_addr << 12), read2);
        // Save to enable reassigning if we didnt go oob.
        real_ins_cpy = real_ins;
        // This is temporary, will be reassigned.
        real_ins = decode_spillover;
      }
    } else {
      // Within same page, re-assign real_ins so we align
      // This is some freaky freaky ptr shenanigans
      real_ins = (inst_data *)((uint64_t)real_ins & ~(uint64_t)(page_size - 1));
      real_ins += pc & (page_size - 1);
    }

    // decoder.decode_inst(it.base(), end.base(), curr_ins);
    decoder.decode_inst(real_ins, real_ins + max_ins, curr_ins);

    if (curr_ins.op == 0xff) {
      std::cout << "Emulator::emu_loop Found break/trap, leaving" << std::endl;
      break;
    }

    // std::cout << "Emulator::emu_loop decoded, sz: " << (int)curr_ins.sz
    //           << " OP (enum): " << std::hex << (int)curr_ins.op << std::endl;

    bool oob = pc + curr_ins.sz > (curr + page_size);

    // If we didnt go oob we need to restore this immediately so we can resume
    // where we left off
    if (!oob && real_ins == decode_spillover) {
      real_ins = real_ins_cpy;
    }

    //  Instruction decoding would go OOB...
    if (!bad_next && oob) {
      cached = cached_next;
      // Also need to remember to reassign the ptr we r using innit
      uint32_t off = (pc + curr_ins.sz) & (page_size - 1);
      // uint32_t off = pc & (page_size - 1);
      //  curr = (pc + curr_ins.sz) & ~(page_size - 1);
      real_ins = (inst_data *)(cached.page_addr << 12) + off;
    }
    // ... AND we cant go oob (bad next page)
    else if (curr_ins.sz == 0 || oob) {
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

    // Only re-set pc if we dont set it outrselves via an instruction
    if (pc == regs.get_pc()) {
      regs.set_pc(pc + curr_ins.sz);
      real_ins += curr_ins.sz;
    }

    curr = (pc + curr_ins.sz) & ~(page_size - 1);
    // std::cout << "Emulator::emu_loop, new PC: " << registers.get_pc()
    //           << std::endl;
    // it += curr_ins.sz;
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

bool is_an(ArgKind kind) { return kind >= ArgKind::A0 && kind <= ArgKind::A3; }

bool is_abs(ArgKind kind) {
  return kind >= ArgKind::abs16 && kind <= ArgKind::abs32;
}

bool is_mem_reg(ArgKind kind) {
  return kind >= ArgKind::MA0 && kind <= ArgKind::MA3;
}

bool is_disp(ArgKind kind) {
  return kind >= ArgKind::d8 && kind <= ArgKind::d32;
}

// Sign extend a number of size nbits
reg_type s_ext(reg_type i, uint32_t nbits) {

  if (nbits == 32)
    return i;

  uint32_t s_mask = 2 << (nbits - 2);
  return i | (0 - (i & s_mask));
}

MMU &Emulator::get_mmu() { return this->mmu; }
Reg &Emulator::get_regs() { return this->regs; }

// TODO: Add ability to get memory?
// TODO: This will straight up NOT work for multiple diff kinds of args, so pay
// that in mind. This needs fixing.
reg_type Emulator::get_val(ArgKind src, const Instruction &ins) {
  reg_type s;
  // TODO: extend for abs where appropriate?
  if (is_imm(src) || is_abs(src) || is_disp(src) || src == ArgKind::regs) {
    uint8_t sz = get_arg_sz(src) * 8;
    s = *(reg_type *)ins.args;
    s = s_ext(s, sz);
  } else {
    s = regs.get(src);
  }
  return s;
}
reg_type Emulator::get_val(uint32_t kindno, const Instruction &ins) {
  assert(kindno < sizeof(ins.kinds) / sizeof(ins.kinds[0]));
  ArgKind src = ins.kinds[kindno];
  reg_type s;

  if (is_reg(src) || is_mem_reg(src)) {
    s = regs.get(src);
    return s;
  }

  s = ins.get_arg(kindno);

  return s;
}

// Finds actual address from arguments and returns
reg_type Emulator::get_addr_from_regs(const Instruction &ins, bool arg) {
  // arg: true: use dest, false: use src
  uint32_t kind_start = arg;
  // Wont be more than 2 i dont think
  ArgKind kind1 = ins.kinds[kind_start];
  ArgKind kind2 = ins.kinds[kind_start + 1];
  // Now find the actual values of the kinds, and add em up
  reg_type k1 = get_val(kind1, ins);
  reg_type k2 = get_val(kind2, ins);
  reg_type addr = k1 + k2;

  return addr;
}

// Gets an address from the registers on the appropriate side, then returns the
// address and outputs which side to indicate the operation (r/w).
reg_type Emulator::get_val_mem(const Instruction &ins, Op *operation) {
  // TODO: Finish
  uint32_t op_loc;
  reg_type addr;

  // Reading == 0, Writing == 1
  op_loc = !(is_abs(ins.kinds[0]) || is_mem_reg(ins.kinds[0]) ||
             is_disp(ins.kinds[0]) ||
             is_an(ins.kinds[1]) && ins.kinds[2] != ArgKind::NONE ||
             ins.kinds[0] == ArgKind::SP && ins.kinds[1] == ArgKind::regs);

  // Simple, just get the addr from registers and read contents.
  if (is_abs(ins.kinds[op_loc]) || is_mem_reg(ins.kinds[op_loc]) ||
      ins.kinds[op_loc] == ArgKind::SP) {
    addr = get_val(ins.kinds[op_loc], ins);
  }
  // A bit harder, need to read multiple registers and add base + displacement
  // to get addr, then we can read.
  else if (is_disp(ins.kinds[op_loc]) ||
           is_an(ins.kinds[op_loc + 1]) /*&& ins.kinds[2] != ArgKind::NONE*/) {
    addr = get_addr_from_regs(ins, op_loc);
  }

  if (operation != NULL)
    *operation = (Op)op_loc;
  return addr;
}
template <typename T> bool Emulator::handle_mov(const Instruction &ins) {
  // TODO: Test
  ArgKind dst = ins.kinds[1];
  ArgKind src = ins.kinds[0];
  // TODO: May have more args

  // 2 registers or imm -> reg
  if (is_reg(dst) && (is_reg(src) || is_imm(src)) &&
      ins.kinds[2] == ArgKind::NONE) {

    // TODO: Is this actually true doe
    if (dst == src) {
      std::cerr << "Emulator::handle_mov registers are identical, should be "
                   "impossible. Aborting."
                << std::endl;
      abort();
    }
    // TODO: Maybe special handling for some register types....
    // PSW -> Dn 0 extends upper 16 bits
    // Dn -> PSW ignores upper 16 bits

    T val;
    // Sz (in bits)
    uint8_t sz = get_arg_sz(src) * 8;
    if (is_imm(src)) {
      // Get imm val from args
      // TODO: needs to change based on size of imm. Gotta ensure that we sign
      // ext here as imm needs to be (dun).
      val = *(T *)ins.args;
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
  // TODO: Pretty sure the above has already caught any and all non-memory
  // operations
  // - if not, blegh.

  Op op;
  reg_type addr = get_val_mem(ins, &op);
  T val = 0;

  // We know this is populated, so should be using it.
  dst = ins.kinds[2] == ArgKind::NONE ? ins.kinds[1] : ins.kinds[2];

  // TODO: Test these thangs
  // Writing to something
  if (op) {
    val = regs.get(src);
    mmu.write(addr, val);
  }
  // Reading
  else {
    val = mmu.read<reg_type>(addr);
    regs.set(dst, val);
  }

  return true;
}

bool Emulator::handle_movm(const Instruction &ins) {

  Op op;
  // Will be SP
  reg_type addr = get_val_mem(ins, &op);
  const uint32_t kidx = (op + 1) % 2;
  // Should fingies crossed be our registers.
  const reg_type reg = get_val(ins.kinds[kidx], ins);

  uint32_t sz;

  int idx = 0;
  int toadd = -4;
  ArgKind kinds[11] = {(ArgKind)0};

  regs.dump_regs();

  if (reg & MovmBits::other) {

    std::cout << "Emulator::handle_movm Other found, doing funny business"
              << std::endl;

    // Dummy
    // addr += 4;
    // Move a bunch of registers to/from SP
    sz = (11 * 4);

    const ArgKind other_kinds[] = {ArgKind::LAR, ArgKind::LIR, ArgKind::MDR,
                                   ArgKind::A1,  ArgKind::A0,  ArgKind::D1,
                                   ArgKind::D0};

    std::memcpy((void *)kinds, other_kinds, sizeof(other_kinds));

    idx = sizeof(other_kinds) / sizeof(other_kinds[0]);
  }
  std::cout << "Emulator::handle_movm normal registers" << std::endl;
  // This is marginally less ugly.
  if (reg & MovmBits::a3) {
    kinds[idx++] = ArgKind::A3;
  }
  if (reg & MovmBits::a2) {
    kinds[idx++] = ArgKind::A2;
  }
  if (reg & MovmBits::d3) {
    kinds[idx++] = ArgKind::D3;
  }
  if (reg & MovmBits::d2) {
    kinds[idx++] = ArgKind::D2;
  }
  idx--;

  if (op == R) {
    idx = 0;
    toadd = 4;
  }

  // Other uses a kinda "dummy" area before we start.
  if (reg & MovmBits::other || (op == W && !(reg & MovmBits::other)))
    addr += toadd;

  for (; idx < (sizeof(kinds) / sizeof(kinds[0])) && idx >= 0;) {
    if (kinds[idx] == ArgKind::NONE)
      break;

    if (op == W) {
      mmu.write(addr, regs.get(kinds[idx]));
      idx--;
    } else {
      regs.set(kinds[idx], mmu.read<reg_type>(addr));
      idx++;
    }
    addr += toadd;
  }

  if (op == W && !(reg & MovmBits::other))
    addr -= toadd;

  regs.set(ArgKind::SP, addr);

  regs.dump_regs();
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
    // bool cnd = ((uint64_t)s + (uint64_t)(d * sign) + cf_maybe) >
    // UINT32_MAX;
    // bool cnd = ((reg_type_s)d > 0 && (reg_type_s)s > 0) && ((uint64_t)d +
    // (uint64_t)(s * sign) + cf_maybe) > UINT32_MAX;
    bool cnd = (reg_type_s)res < (reg_type_s)d &&
               (reg_type_s)res < (reg_type_s)s && (reg_type_s)res > 0;
    // Mask off old bits, shouldnt carry over to the next addition.
    psw &= ~0b1111;
    psw |= (PswBits::V * cnd);
    cnd = res == 0;
    psw |= (PswBits::Z * cnd);
    cnd = (int)res < 0;
    psw |= (PswBits::N * cnd);
    // Now, how to do the carry.
    // cnd = (s & CARRY_BIT || d & CARRY_BIT);
    cnd = (res < d || res < s);
    psw |= (PswBits::C * cnd);

    regs.set(ArgKind::PSW, psw);
    regs.set(dst, res);
  }

  return true;
}
bool Emulator::handle_mul(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  uint64_t res;
  // Registers are the only operands possible
  reg_type s = regs.get(src);
  reg_type d = regs.get(dst);
  reg_type psw = regs.get(ArgKind::PSW);

  if (ins.op == MULU)
    res = (unsigned long)d * (unsigned long)s;
  if (ins.op == MUL)
    // Disgusting but necessary - need to first get the signed 32-bit values,
    // then extend them to 64 so we can do a full 64 bit mul
    res = ((long)((int)d) * (long)((int)s));

  bool cnd = (res & 0xffffffff) == 0;
  // Only effects the N and Z from what i can see
  // TODO: Unsure if this is correct but we shall see, produces correct asm
  // result anyhow.
  psw &= ~0b1111;
  psw |= (PswBits::Z * cnd);
  cnd = ((int)(res & 0xffffffff)) < 0;
  psw |= (PswBits::N * cnd);

  // std::cout << "MULU: Res: " << std::hex << res << std::endl;
  // std::cout << "MULU: Res, high: " << std::hex
  //           << ((res & 0xffffffff00000000) >> 32) << std::endl;

  regs.set(ArgKind::MDR, (res & 0xffffffff00000000) >> 32);
  regs.set(dst, res & 0xffffffff);
  regs.set(ArgKind::PSW, psw);
  return true;
}

bool Emulator::handle_div(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  int res = 0;
  // Registers are the only operands possible
  reg_type_s s = regs.get(src);
  reg_type d = regs.get(dst);
  reg_type mdr = regs.get(ArgKind::MDR);
  reg_type psw = regs.get(ArgKind::PSW);

  uint64_t combined = d + ((uint64_t)mdr << 32);
  std::cout << "DIV: combined: " << std::hex << combined << std::endl;
  // TODO: handle div by zero
  if (ins.op == DIVU && s != 0) {
    res = combined / s;
    mdr = combined % s;
  }
  if (ins.op == DIV && s != 0) {
    res = (int)((long)combined / (long)s);
    mdr = (long)((long)combined % (long)s);
  }

  psw &= ~0b1111;
  if (s == 0) {
    psw |= PswBits::V;
    mdr = 0;
    res = 0xff;
  } else {
    // Normal operation
    // TODO: unsure about this, manual says we should be checking if the
    // DIVISOR is negative but that produces weird results and is inconsistent
    // with the way this flag has been used previously.
    bool cnd = (int)(res & 0xffffffff) < 0;
    psw |= (PswBits::N * cnd);
    cnd = res == 0;
    psw |= (PswBits::Z * cnd);
  }

  regs.set(ArgKind::MDR, mdr);
  regs.set(dst, res);
  regs.set(ArgKind::PSW, psw);
  return true;
}

bool Emulator::handle_inc(const Instruction &ins) {
  ArgKind dst = ins.kinds[0];

  reg_type res;
  // Registers are the only operands possible
  reg_type d = regs.get(dst);
  reg_type psw = regs.get(ArgKind::PSW);

  int toadd = 1;
  if (ins.op == INC4)
    toadd *= 4;
  d += toadd;

  if (is_dn(dst)) {
    // Do PSW stuff only on Dn registers
    // TODO: This seems correct but could do with some more validation later
    // maybe.
    bool cnd = (int)d < 0;
    psw &= ~0b1111;
    psw |= (PswBits::N * cnd);
    cnd = (d == 0);
    psw |= (PswBits::Z * cnd);
    psw |= (PswBits::C * cnd);
    cnd = (d - 1 == 0x7fffffff);
    // TODO: Unsure
    psw |= (PswBits::V * cnd);
    regs.set(ArgKind::PSW, psw);
  }

  regs.set(dst, d);

  return true;
}

bool Emulator::handle_cmp(const Instruction &ins) {

  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  // Registers are the only operands possible
  reg_type s;
  reg_type d = regs.get(dst);
  reg_type psw = regs.get(ArgKind::PSW);

  // If we use imm, need to sign extend.
  // TODO: Put this in a function???
  if (is_imm(src)) {
    uint8_t sz = get_arg_sz(src) * 8;
    s = *(reg_type *)ins.args;
    s = s_ext(s, sz);
  } else {
    s = regs.get(src);
  }

  uint32_t res = d - s;
  std::cout << "Emulator::handle_cmp res -> " << std::hex << res << std::endl;
  // If we overflow and ARENT negative.
  bool cnd = (reg_type_s)res > (reg_type_s)d && (reg_type_s)res > 0;
  // Mask off old bits, shouldnt carry over to the next addition.
  psw &= ~0b1111;
  psw |= (PswBits::V * cnd);
  cnd = res == 0;
  psw |= (PswBits::Z * cnd);
  cnd = (int)res < 0;
  psw |= (PswBits::N * cnd);
  // Now, how to do the carry - i *think* this is the correct way for this.
  // cnd = (s & CARRY_BIT && !(d & CARRY_BIT));
  cnd = res > d || res > s;
  psw |= (PswBits::C * cnd);

  regs.set(ArgKind::PSW, psw);

  return true;
}

bool Emulator::handle_and(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];
  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type d;
  if (dst != ArgKind::PSW)
    d = regs.get(dst);
  else
    d = psw;

  reg_type res = s & d;

  psw &= ~0b1111;
  if (dst == ArgKind::PSW) {
    // Sets from first 4 bits
    psw = res & 0b1111;
    res = psw;
  } else {
    bool cnd = (int)res < 0;
    psw |= (PswBits::N * cnd);
    cnd = res == 0;
    psw |= (PswBits::Z * cnd);
    regs.set(ArgKind::PSW, psw);
  }
  regs.set(dst, res);

  return true;
}

// Largely same as AND, maybe encapsulate bitwise logic differently?
// TODO: Test all bitwise ops
bool Emulator::handle_or(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];
  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type d;
  if (dst != ArgKind::PSW)
    d = regs.get(dst);
  else
    d = psw;

  reg_type res = s | d;

  psw &= ~0b1111;
  if (dst == ArgKind::PSW) {
    // Sets from first 4 bits
    psw = res & 0b1111;
    res = psw;
  } else {
    bool cnd = (int)res < 0;
    psw |= (PswBits::N * cnd);
    cnd = res == 0;
    psw |= (PswBits::Z * cnd);
    regs.set(ArgKind::PSW, psw);
  }
  regs.set(dst, res);

  return true;
}

bool Emulator::handle_xor(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];
  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type d;
  if (dst != ArgKind::PSW)
    d = regs.get(dst);
  else
    d = psw;

  reg_type res = s ^ d;

  psw &= ~0b1111;
  bool cnd = (int)res < 0;
  psw |= (PswBits::N * cnd);
  cnd = res == 0;
  psw |= (PswBits::Z * cnd);
  regs.set(ArgKind::PSW, psw);
  regs.set(dst, res);

  return true;
}

bool Emulator::handle_not(const Instruction &ins) {

  ArgKind src = ins.kinds[0];
  reg_type s = regs.get(src);

  s = ~s;

  regs.set(src, s);
  // Must also set flags
  reg_type psw = regs.get(ArgKind::PSW);
  psw &= ~0b1111;
  bool cnd = (int)s < 0;
  psw |= (PswBits::N * cnd);
  cnd = s == 0;
  psw |= (PswBits::Z * cnd);

  regs.set(ArgKind::PSW, psw);

  return true;
}

// BTST kinda just does what and does but doesnt store the result other than
// setting the flags i think?
bool Emulator::handle_btst(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);

  reg_type d;
  if (!is_dn(dst)) {
    // TODO: memory stuff
    d = get_val_mem(ins, NULL);
  } else {
    d = get_val(dst, ins);
  }

  reg_type_s res = s & d;

  bool cnd = res < 0;
  psw &= ~0b1111;
  psw |= (PswBits::N * cnd);
  cnd = res == 0;
  psw |= (PswBits::Z * cnd);
  regs.set(ArgKind::PSW, psw);

  return true;
}

bool Emulator::handle_bset(const Instruction &ins) {
  // TODO: All of this needs memory support
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];
  reg_type s = get_val(src, ins);
  reg_type d = get_val_mem(ins, NULL);
  reg_type psw = regs.get(ArgKind::PSW);

  reg_type res = s & d;

  bool cnd = (res == 0);
  psw &= ~0b1111;
  psw |= (PswBits::Z * cnd);
  regs.set(ArgKind::PSW, psw);
  mmu.write(d, (s | d) & 0xff);
  return true;
}

bool Emulator::handle_bclr(const Instruction &ins) {
  // TODO: All of this needs memory support
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];
  reg_type s = get_val(src, ins);
  reg_type d = get_val_mem(ins, NULL);
  reg_type psw = regs.get(ArgKind::PSW);

  reg_type res = s & d;

  bool cnd = (res == 0);
  psw &= ~0b1111;
  psw |= (PswBits::Z * cnd);
  regs.set(ArgKind::PSW, psw);

  s = s ^ 0xffffffff;
  mmu.write(d, (s & d) & 0xff);
  return true;
}

bool Emulator::handle_asr(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type d;
  reg_type_s res;

  if (dst == ArgKind::NONE) {
    s = 1;
    dst = src;
  } else {
    d = get_val(dst, ins);
  }

  reg_type shift = s & 0x1f;

  psw &= ~0b1111;
  if (shift) {
    res = (reg_type_s)d >> shift;
    psw |= (PswBits::C * (d & 1));
    regs.set(dst, res);
  } else {
    res = d;
  }

  bool cnd = res < 0;
  psw |= (PswBits::N * cnd);
  cnd = res == 0;
  psw |= (PswBits::Z * cnd);
  regs.set(ArgKind::PSW, psw);

  return true;
}

// For all intents and purposes just an unsigned shift
bool Emulator::handle_lsr(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type d;
  reg_type_s res;

  if (dst == ArgKind::NONE) {
    s = 1;
    dst = src;
  } else {
    d = get_val(dst, ins);
  }

  reg_type shift = s & 0x1f;

  psw &= ~0b1111;
  if (shift) {
    res = d >> shift;
    psw |= (PswBits::C * (d & 1));
    // res &= ~0x80000000;
    regs.set(dst, res);
  } else {
    res = d;
  }

  bool cnd = res < 0;
  psw |= (PswBits::N * cnd);
  cnd = res == 0;
  psw |= (PswBits::Z * cnd);
  regs.set(ArgKind::PSW, psw);

  return true;
}

// Also handles asl2
bool Emulator::handle_asl(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  ArgKind dst = ins.kinds[1];

  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type d;
  reg_type_s res;

  if (dst == ArgKind::NONE) {
    s = 1;
    dst = src;
    d = get_val(src, ins);
  } else {
    d = get_val(dst, ins);
  }

  reg_type shift;

  if (ins.op == ASL2)
    shift = 2;
  else
    shift = s & 0x1f;

  psw &= ~0b1111;
  if (shift) {
    res = (reg_type_s)d << shift;
    psw |= (PswBits::C * (d & 1));
    regs.set(dst, res);
  } else {
    res = d;
  }

  bool cnd = res < 0;
  psw |= (PswBits::N * cnd);
  cnd = res == 0;
  psw |= (PswBits::Z * cnd);
  regs.set(ArgKind::PSW, psw);

  return true;
}

bool Emulator::handle_ror(const Instruction &ins) {
  ArgKind src = ins.kinds[0];

  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type_s res;

  res = std::rotr(s, 1);

  psw &= ~0b1111;
  bool cnd = res < 0;
  psw |= (PswBits::N * cnd);
  cnd = res == 0;
  psw |= (PswBits::Z * cnd);

  regs.set(src, res);
  regs.set(ArgKind::PSW, psw);

  return true;
}

bool Emulator::handle_rol(const Instruction &ins) {
  ArgKind src = ins.kinds[0];

  reg_type s = get_val(src, ins);
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type_s res;

  res = std::rotl(s, 1);

  psw &= ~0b1111;
  bool cnd = res < 0;
  psw |= (PswBits::N * cnd);
  cnd = res == 0;
  psw |= (PswBits::Z * cnd);

  regs.set(src, res);
  regs.set(ArgKind::PSW, psw);

  return true;
}

// TODO: Test
bool Emulator::handle_bcc(const Instruction &ins) {
  bool cnd;
  reg_type psw = regs.get(ArgKind::PSW);

  bool z = psw & PswBits::Z;
  bool n = psw & PswBits::N;
  bool c = psw & PswBits::C;
  bool v = psw & PswBits::V;

  switch (ins.op) {
  case BEQ:
    cnd = z;
    break;
  case BNE:
    cnd = !z;
    break;
  case BGT:
    cnd = !(z || (n ^ v));
    break;
  case BGE:
    cnd = !(n ^ v);
    break;
  case BLE:
    cnd = z || (n ^ v);
    break;
  case BLT:
    cnd = n ^ v;
    break;
  case BHI:
    cnd = !(c || z);
    break;
  case BCC:
    cnd = !c;
    break;
  case BLS:
    // TODO: Maybe use single or
    cnd = c || z;
    break;
  case BCS:
    cnd = c;
    break;
  case BVC:
    cnd = !v;
    break;
  case BVS:
    cnd = v;
    break;
  case BNC:
    cnd = !n;
    break;
  case BNS:
    cnd = n;
    break;
  case BRA:
    cnd = true;
    break;
  }

  if (!cnd)
    return false;
  // Its just a dmp in this case
  handle_jmp(ins);
  return true;
}
// TODO: Test
bool Emulator::handle_lcc(const Instruction &ins) {
  bool cnd;
  reg_type psw = regs.get(ArgKind::PSW);
  reg_type lar = regs.get(ArgKind::LAR);

  bool z = psw & PswBits::Z;
  bool n = psw & PswBits::N;
  bool c = psw & PswBits::C;
  bool v = psw & PswBits::V;

  switch (ins.op) {
  case LEQ:
    cnd = z;
    break;
  case LNE:
    cnd = !z;
    break;
  case LGT:
    cnd = !z || !(n ^ v);
    break;
  case LGE:
    cnd = !(n ^ v);
    break;
  case LLE:
    cnd = z || (n ^ v);
    break;
  case LLT:
    cnd = n ^ v;
    break;
  case LHI:
    cnd = !c || !z;
    break;
  case LCC:
    cnd = !c;
    break;
  case LLS:
    // TODO: Maybe use single or
    cnd = c || z;
    break;
  case LCS:
    cnd = c;
    break;
  case LRA:
    cnd = true;
    break;
  }

  if (cnd)
    lar -= 4;
  else
    lar += 1;

  regs.set_pc(lar);
  // Its just a dmp in this case
  return true;
}

// TODO: Test
bool Emulator::handle_setlb(const Instruction &ins) {

  reg_type pc = regs.get_pc();
  regs.set(ArgKind::LIR, mmu.read<reg_type>(pc + 1));
  regs.set(ArgKind::LAR, pc + 5);

  return true;
}

bool Emulator::handle_jmp(const Instruction &ins) {

  reg_type addr = get_val(ins.kinds[0], ins);
  regs.set_pc(addr + regs.get_pc());

  return true;
}

bool Emulator::handle_call(const Instruction &ins) {

  reg_type_s callee = get_val(0, ins);
  callee = s_ext(callee, get_arg_sz(ins.kinds[0]) * 8);

  reg_type reg = get_val(1, ins);
  reg_type sp_off = get_val(2, ins);
  reg_type sp = regs.get(ArgKind::SP);
  int idx = 0;

  std::cout << "Emulator::handle_call callee -> " << std::hex << callee
            << std::endl;
  std::cout << "Emulator::handle_call regs -> " << std::hex << reg << std::endl;
  std::cout << "Emulator::handle_call sp_off -> " << std::hex << sp_off
            << std::endl;

  ArgKind kinds[11] = {(ArgKind)0};

  if (reg & MovmBits::other) {

    std::cout << "Emulator::handle_call Other found, doing funny business"
              << std::endl;

    // Dummy
    // addr += 4;
    // Move a bunch of registers to/from SP

    const ArgKind other_kinds[] = {ArgKind::LAR, ArgKind::LIR, ArgKind::MDR,
                                   ArgKind::A1,  ArgKind::A0,  ArgKind::D1,
                                   ArgKind::D0};

    std::memcpy((void *)kinds, other_kinds, sizeof(other_kinds));

    idx = sizeof(other_kinds) / sizeof(other_kinds[0]);
  }
  std::cout << "Emulator::handle_call normal registers" << std::endl;
  // This is marginally less ugly.
  if (reg & MovmBits::a3) {
    kinds[idx++] = ArgKind::A3;
  }
  if (reg & MovmBits::a2) {
    kinds[idx++] = ArgKind::A2;
  }
  if (reg & MovmBits::d3) {
    kinds[idx++] = ArgKind::D3;
  }
  if (reg & MovmBits::d2) {
    kinds[idx++] = ArgKind::D2;
  }
  idx--;
  // TODO: Now actually store the registers

  uint32_t sp_cp = sp;

  std::cout << "Emulator::handle_call SP before:" << std::endl;
  mmu.log_many(sp - 48, 10);

  reg_type_s pc = regs.get_pc();
  reg_type saved_pc = pc + ins.sz;
  mmu.write(sp_cp, saved_pc);
  for (; idx >= 0; idx--) {
    sp_cp -= 4;
    mmu.write(sp_cp, regs.get(kinds[idx]));
  }

  std::cout << "Emulator::handle_call SP after:" << std::endl;
  mmu.log_many(sp - 48, 22);
  // TODO: Also set SP-imm8(zero_ext)->SP, PC+?->MDR, PC+d32->PC

  regs.set(ArgKind::SP, sp - sp_off);
  regs.set(ArgKind::MDR, saved_pc);
  regs.set(ArgKind::PC, pc + callee);

  return true;
}

bool Emulator::handle_calls(const Instruction &ins) {
  ArgKind src = ins.kinds[0];
  reg_type callee = get_val(0, ins);
  reg_type pc = regs.get(ArgKind::PC);
  reg_type sp = regs.get(ArgKind::SP);

  regs.set(ArgKind::MDR, pc + ins.sz);
  mmu.write(sp, pc + ins.sz);

  // Use displacement
  if (!is_reg(src) && !is_mem_reg(src)) {
    callee = pc + callee;
  }
  regs.set_pc(callee);

  return true;
}

bool Emulator::handle_ret(const Instruction &ins) {

  reg_type retaddr;
  reg_type sp = regs.get(ArgKind::SP);
  reg_type pc = regs.get_pc();
  int idx = 0;

  // Rets doesnt restore any registers
  if (ins.op == RETS) {
    regs.set_pc(mmu.read<reg_type>(sp));
    return true;
  }

  reg_type reg = get_val(0, ins);
  reg_type sp_off = get_val(1, ins);

  // Now handle register shenanigans

  std::cout << "Emulator::handle_ret regs -> " << std::hex << reg << std::endl;
  std::cout << "Emulator::handle_ret sp_off -> " << std::hex << sp_off
            << std::endl;

  ArgKind kinds[11] = {(ArgKind)0};

  if (reg & MovmBits::other) {

    std::cout << "Emulator::handle_ret Other found, doing funny business"
              << std::endl;

    // Dummy
    // addr += 4;
    // Move a bunch of registers to/from SP

    const ArgKind other_kinds[] = {ArgKind::LAR, ArgKind::LIR, ArgKind::MDR,
                                   ArgKind::A1,  ArgKind::A0,  ArgKind::D1,
                                   ArgKind::D0};

    std::memcpy((void *)kinds, other_kinds, sizeof(other_kinds));

    idx = sizeof(other_kinds) / sizeof(other_kinds[0]);
  }
  std::cout << "Emulator::handle_ret normal registers" << std::endl;
  // This is marginally less ugly.
  if (reg & MovmBits::a3) {
    kinds[idx++] = ArgKind::A3;
  }
  if (reg & MovmBits::a2) {
    kinds[idx++] = ArgKind::A2;
  }
  if (reg & MovmBits::d3) {
    kinds[idx++] = ArgKind::D3;
  }
  if (reg & MovmBits::d2) {
    kinds[idx++] = ArgKind::D2;
  }
  idx--;
  sp += sp_off;
  regs.set(ArgKind::SP, sp);

  if (ins.op == RET)
    retaddr = mmu.read<reg_type>(sp);
  else                                // RETF
    retaddr = regs.get(ArgKind::MDR); // No idea why they chose to use MDR

  regs.set_pc(retaddr);

  std::cout << "Emulator::handle_ret SP area ( " << ((idx - 2) * 4)
            << "):" << std::endl;
  mmu.log_many(sp - ((idx - 2) * 4), 11);

  // for (int disp = -44; idx >= 0 && disp <= -4; idx--, disp += 4) {
  for (int disp = 4; idx >= 0 /* && disp <= ((idx - 2) * 4)*/;
       idx--, disp += 4) {
    regs.set(kinds[idx], mmu.read<reg_type>(sp - disp));
  }

  return true;
}

// TODO: Test
bool Emulator::handle_rti(const Instruction &ins) {
  reg_type sp = regs.get(ArgKind::SP);
  regs.set(ArgKind::PSW, mmu.read<uint16_t>(sp));
  regs.set_pc(mmu.read<reg_type>(sp + 4));
  regs.set(ArgKind::SP, sp + 8);
  return true;
}

// TODO: Could also add some additional stuff here so it looks like its
// performing a syscall.
bool Emulator::handle_trap(const Instruction &ins) {
  reg_type pc = regs.get_pc();
  mmu.write(regs.get(ArgKind::SP), pc + 2);
  // Why this? It was in the manual.
  regs.set_pc(0x40000010);
  return true;
}
bool Emulator::handle_nop(const Instruction &ins) { return true; }

bool Emulator::execute_insn(const Instruction &ins) {
  // std::cout << "Emulator::execute_insn doing stuff..." << std::endl;
  //
  switch (ins.op) {
  case MOV:
    // Handle MOV instruction
    handle_mov<reg_type>(ins);
    break;
  case MOVBU:
    // Handle MOVBU instruction
    handle_mov<uint8_t>(ins);
    break;
  case MOVHU:
    // Handle MOVHU instruction
    handle_mov<uint16_t>(ins);
    break;
  case MOVM:
    // Handle MOVM instruction
    handle_movm(ins);
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
  case INC4:
  case INC:
    // Handle INC instruction
    handle_inc(ins);
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
  case DIVU:
  case DIV:
    // Handle DIV instruction
    handle_div(ins);
    break;
  case CMP:
    // Handle CMP instruction
    handle_cmp(ins);
    break;
  case OR:
    // Handle OR instruction
    handle_or(ins);
    break;
  case AND:
    // Handle AND instruction
    handle_and(ins);
    break;
  case NOT:
    // Handle NOT instruction
    handle_not(ins);
    break;
  case XOR:
    // Handle XOR instruction
    handle_xor(ins);
    break;
  // Bit testing operations
  case BSET:
    // Handle BSET instruction
    handle_bset(ins);
    break;
  case BCLR:
    // Handle BCLR instruction
    handle_bclr(ins);
    break;
  case BTST:
    // Handle BTST instruction
    handle_btst(ins);
    break;
  // Shift and rotate operations
  case ASL2:
  case ASL:
    // Handle ASL instruction
    handle_asl(ins);
    break;
  case ASR_2:
  case ASR:
    // Handle ASR instruction
    handle_asr(ins);
    break;
  case LSR_2:
  case LSR:
    // Handle LSR instruction
    handle_lsr(ins);
    break;
  case ROR:
    // Handle ROR instruction
    handle_ror(ins);
    break;
  case ROL:
    // Handle ROL instruction
    handle_rol(ins);
    break;
  // Control flow operations
  case Bcc:
    // Handle Bcc instruction
    handle_bcc(ins);
    break;
  case Lcc:
    // Handle Lcc instruction
    handle_lcc(ins);
    break;
  case JMP:
    // Handle JMP instruction
    handle_jmp(ins);
    break;
  case JSR_1:
    // Handle JSR_1 instruction
    break;
  case CALL:
    // Handle CALL instruction
    handle_call(ins);
    break;
  case CALLS:
    // Handle CALLS instruction
    handle_calls(ins);
    break;
  case TRAP:
    // Handle TRAP instruction
    handle_trap(ins);
    break;
  // Return operations
  case RET:
    // Handle RET instruction
  case RETF:
    // Handle RETF instruction
  case RETS:
    // Handle RETS instruction
    handle_ret(ins);
    break;
  case RTI:
    // Handle RTI instruction
    handle_rti(ins);
    break;
  // Miscellaneous operations
  case NOP:
    // Handle NOP instruction
    handle_nop(ins);
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
  case BGT:
    // Handle BGT instruction
  case BGE:
    // Handle BGE instruction
  case BLE:
    // Handle BLE instruction
  case BCS:
    // Handle BCS instruction
  case BHI:
    // Handle BHI instruction
  case BCC:
    // Handle BCC instruction
  case BLS:
    // Handle BLS instruction
  case BEQ:
    // Handle BEQ instruction
  case BNE:
    // Handle BNE instruction
  case BVC:
    // Handle BVC instruction
  case BVS:
    // Handle BVS instruction
  case BNC:
    // Handle BNC instruction
  case BNS:
    // Handle BNS instruction
  case BRA:
    // Handle BRA instruction
    handle_bcc(ins);
    break;
  // Loop execution instructions
  case LLT:
    // Handle LLT instruction
  case LGT:
    // Handle LGT instruction
  case LGE:
    // Handle LGE instruction
  case LLE:
    // Handle LLE instruction
  case LCS:
    // Handle LCS instruction
  case LHI:
    // Handle LHI instruction
  case LCC:
    // Handle LCC instruction
    handle_lcc(ins);
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
    handle_setlb(ins);
    break;
  default:
    // Handle unknown instruction
    break;
  }
  return true;
}
