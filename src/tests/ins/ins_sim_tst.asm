
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
  
  #### Uncomment fo negative mdr
  mov 100, d0
  mul  d0, d2

  #mov mdr, d2

  
  #mov psw, d2
  #mov 1, d0
  #mov 2, d2 
  div  d2, d0
  divu d2, d0
  mov d0, d2
  #add d1, d0
  mov psw, d1
  add d1, d2
  mov mdr, d1
  add d1, d2
  #mov psw, d2
  #add 0x80000000, d2
  
  mov 0x7fffffff, d2
  inc d2
  inc4 a0

  
  mov psw, d2
  add 0x80000000, d2
  
  mov 0x80000000, d1
  cmp d1, d2 
  and 0x80000000, d2
  
  and 0xffff, psw
   
  mov psw, d2
  add 0x80000000, d2
  #mov psw, d2
  #add 0x80000000, d2
  mov d2, a0
  mov d0, (a0)
