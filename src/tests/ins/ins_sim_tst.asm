
.section .text
.global _start
_start:
  mov 1, d2
  mov 0xffffffff, d1
  mov 0x80000000, d0
  add d1, d0
  ## Stall until we update the CF flag, (on the real CPU this is how we would have to do it anyway)
  nop
  addc d0, d2
  #mov d2, a0
  #mov d0, (a0)
  
  ## Testing subc
  sub d1, d2
  subc d1, d2
  
  #mov 0x80, d2
  mov 100, d0
  mulu d0, d2
  add 0x80000000, d2

  mov d2, a0
  mov d0, (a0)
