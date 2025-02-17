.section .text
.global _start
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
  
  add 0xddd, d2
  cmp 0xddd, d3
  add 0xddd, a2
  cmp 0xddd, a3
  
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


  ###### 0xF0
  
  mov a1, (a2)
  mov a0, (a1)
  movbu (a1), d1 
  movbu d3, (a1) 
  movhu (a1), d1 
  movhu d2, (a1) 
  
  bset d2, (a1)
  bclr d3, (a1)

  calls (a3)
  jmp (a1)
  rets
  rti
  trap
  
  ###### end 0xF0

  ###### 0xF1
  sub d1, d1
  sub a1, d2
  sub d2, a3
  sub a3, a0


  addc d1, d0

  add a1, d1
  add d1, a2
  add a1, a3

  subc d1, d3
  
  cmp a1, d3
  cmp d1, a2
  
  mov a1, d1
  mov d1, a2
  ###### end 0xF1


  ###### 0xF2
  and d1, d0
  or d1, d0
  xor d1, d0
  not d2

  mul d1, d2
  mulu d1, d2
  div d3, d3
  divu d3, d1

  rol d2
  ror d1
  
  asl d1, d1
  lsr d1, d3
  asr d1, d3
  
  ext d2

  mov mdr, d1
  mov psw, d2

  mov a1, sp
  mov d0, mdr
  mov d3, psw
  ###### end 0xF2


  ###### 0xF3
  mov (d0, a2), d2
  mov d1, (d2, a0)
  mov (d3, a3), a2
  mov a3, (d1, a1)
  ###### end 0xF3
  
  ###### 0xF4
  movbu (d0, a2), d2
  movbu d1, (d2, a0)
  movhu (d3, a3), d2
  movhu d3, (d1, a1)
  ###### end 0xF4


  ###### 0xFC
  mov (0x41414141, a1), d0 
  mov (0x41414141, a1), a2 
  movbu (0x41414141, a1), d0 
  movhu (0x41414141, a1), d2 

  mov d1, (0x41414141, a1)
  mov a3, (0x41414141, a1)
  movbu d2, (0x41414141, a1)
  movhu d3, (0x41414141, a1)

  mov a0, (0x42424242)
  mov a3, (0x42424242)
  mov a2, (0x42424242)
  mov d0, (0x42424242)
  mov d3, (0x42424242)
  mov d2, (0x42424242)

  mov a0, (0x42424242, sp)
  mov a3, (0x42424242, sp)
  mov a2, (0x42424242, sp)
  mov d0, (0x42424242, sp)
  mov d3, (0x42424242, sp)
  mov d2, (0x42424242, sp)

  mov (0x42424242), a0 
  mov (0x42424242), a3 
  movbu (0x42424242), d2 
  movbu (0x42424242), d0 
  movhu (0x42424242), d3 
  movhu (0x42424242), d2 
  
  mov   (0x42424242, sp), a0 
  mov   (0x42424242, sp), a3 
  movbu (0x42424242, sp), d2 
  movbu (0x42424242, sp), d0 
  movhu (0x42424242, sp), d3 
  movhu (0x42424242, sp), d2 

  add 0x43434343, d2
  add 0x43434343, a3
  mov 0x43434343, d2
  mov 0x43434343, a3
  cmp 0x43434343, d2
  cmp 0x43434343, a3
  sub 0x43434343, d2
  sub 0x43434343, a3

  and 0x43434343, d2
  or 0x43434343, d3
  xor 0x43434343, d2
  btst 0x43434343, d3
  add 0x43434343, sp
  calls 0x43454647
  ###### end 0xFC
  
  ###### 0xFE
  bset 0x41, (0x42434445)
  bclr 0x41, (0x42434445)
  btst 0x41, (0x42434445)

  bset 0x41, (0x4243)
  bclr 0x41, (0x4243)
  btst 0x41, (0x4243)
  ###### end 0xFE


  ###### 0x0
  clr d2 
  clr d1 
  clr d3 
  mov d1, (0x4142)
  mov d3, (0x4142)
  movbu d1, (0x4142)
  movbu d3, (0x4142)
  movhu d1, (0x4142)
  movhu d3, (0x4142)

  extb d1
  extbu d2

  exth d1
  exthu d2

  add 0x9, a1
  mov 0x9, a3

  add 0x9, d1
  mov 0x9, d3

  mov (0x3333), d0
  movbu (0x3333), d0
  movhu (0x3333), d0
  mov sp, a0

  inc d0
  inc d1
  inc a0
  inc a1

  mov d2, (0x44, sp)
  mov a2, (0x44, sp)

  inc4 a3
  asl2 d2
  mov (0x55, sp), d3
  mov (0x55, sp), a2

  mov d0, (a1)
  mov d1, (a2)

  mov (a2), d1
  mov (a3), d2

  mov d0, d1
  mov a0, a1
  cmp d0, d1
  cmp a0, a1

  mov 0x41, d1
  mov 0x41, a1
  cmp 0x41, d1
  cmp 0x41, a1

  blt 0x41
  bgt 0x41
  bge 0x41
  ble 0x41
  bcs 0x41
  bhi 0x41
  bcc 0x41
  bls 0x41
  beq 0x41
  bne 0x41
  bra 0x41

  nop
  jmp 0x4444
  call 0x4141,[],0
  
  ## Cannot use any an OR dn reg less than 2
  #movm (sp), [a0]
  movm [a2, a3, d2, d3],(sp)
  movm [a3, a2, d3, d2],(sp)
  movm [a3, a3, d3],(sp)

  llt 
  lgt
  lge
  lle
  lcs
  lhi
  lcc
  lls
  leq
  lne
  lra
  
  setlb
  

  jmp 0x41424344
  call 0x41424344,[],0
  retf [a3], 4
  ret [a2], 4

  add d0, d1
  add d1, d0
  add d1, d1
  ###### end 0x0
