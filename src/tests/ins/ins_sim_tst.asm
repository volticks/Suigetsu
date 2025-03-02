
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
  mov -1, d2
  #mov 0, mdr
  mov 0x1234, d0
  divu  d2, d0
  #divu d2, d0
  #mov mdr, d1
  mov d0, d2
  #add d1, d0
  mov psw, d1
  add d1, d2
  #mov d1, d2
  
  mov 0x814141, d0
  mov 0x414142, d1
  or d1, d0
  xor d1, d0
  not d0
  
  #asr 0x1f, d0
  mov 0x1f, d1
  lsr d1, d0
  asl d1, d0
  ror d0
  rol d0

  mov psw, d0
  mov d0, d2
  
  mov 0x1234, d2
  mov 0x1000, a0
  mov d2, (d0, a0)

  #add 0x80000000, d2
  #mov d0, (a0)
  inc d0
  mov (d0, a0), d2


  ## Lets try some cross page accesses 
  mov 0x1000, a0
  mov 0xffe, d0
  #mov 0x0, d0
  mov 0x12345678, d2
  mov d2, (d0, a0)
  #inc d0 ## Causes problems innit
  mov (d0, a0), d2



  #add 0x80000000, d2
  mov d2, a0
  mov d0, (a0)

