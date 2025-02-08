        .global _start

        .text
_start:
  mov 1234, a1
  mov 1234, a0
  clr d1

  ## 0xF8
  mov (0x41, a0), d2
  mov d0, (0x41, a0)
  movbu (0x41, a0), d2
  movbu d1, (0x41, a0)
  movhu (0x41, a0), d2
  movhu d2, (0x41, a0)
  
  movbu d3, (0x22, sp)
  movbu (0x21, sp), d2
  and 0x44, d2
  asr 0x43, d0
  
  ## PC in this case is implicit.
  BVC 0x33
  bvs 0x33
  bnc 0x33
  bns 0x33

  btst 0x99, d1

  add 0x9, sp
  mov (0x77, a0), sp
  mov sp, (0x77, a0)

  ## 0xFA
  mov (0x4141, a1), d2
  mov d3, (0x4141, a0)
  mov (0x4141, a1), a0
  mov a0, (0x4141, a0)
  movbu (0x4141, a1), d2
  movbu d3, (0x4141, a0)
  movhu (0x4141, a1), d2
  movhu d3, (0x4141, a0)

  mov a3, (0xffff)

  mov a0, (0x5555, sp)
  movbu d1, (0x5555, sp)
  movhu d2, (0x5555, sp)
  mov a3, (0x5555, sp)

  mov (0xffff), a2
  
  mov (0x5555, sp), a2
  mov (0x5555, sp), d2
  movbu (0x5555, sp), d2
  movhu (0x5555, sp), d2
  
  add 0xdddd, d2
  cmp 0xdddd, d3
  add 0xdddd, a2
  cmp 0xdddd, a3
  
  ## Looks like these wide bitwise ops are only supported on the dn regs
  and 0xdddd, d2
  or 0xdddd, d3
  xor 0xdddd, d2
  btst 0xdddd, d3

  bset 0x40, (0x41, a1)
  bclr 0x40, (0x41, a2)
  btst 0x40, (0x41, a3)
  
  and 0x6666, psw
  or 0x6666, psw
  add 0x6666, sp
  ## implicitly pc
  #calls 0x6666, pc
  calls 0x6666
  ###### end 0xFA
