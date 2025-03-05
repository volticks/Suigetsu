
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
  #mov (2, a0), d2
  #mov (0x414, a0), d2
  #mov (0x4141, a0), d2
  ##mov (0x41413000, a0), d2
  ##mov (0x41414000), d2
  #mov (2, sp), d2
  #
  ##mov (0x41413000, a0), a2
  ##mov (0x41414000, sp), a2
  #mov 0, a0
  #mov (0x41, a0), sp

  mov d2, (a0)
  mov d2, (2, a0)
  mov d2, (0x414, a0)
  #mov d2, (0x41413000, a0)
  #mov d2, (0x41414000)
  mov d2, (2, sp)
  #mov d2, (0x41413000, sp)
  mov sp, (2, a0)


  nop
  nop
  nop
  nop
  mov (2, sp), d2
  
  #mov (0x41413000, a0), a2
  #mov (0x41414000, sp), a2
  mov 0, a0
  mov (0x41, a0), sp

  
  movbu (a0), d1
  movbu (0), d2

  movhu (a0), d1
  movhu (0), d2
  mov 128, d1
  mov d1, a1
  #movbu (0x8, a1), d2
  .byte 0xf8, 0x49, 0x80
  #movb (0), d2 ## Idk if this actually exists -- it doesnt.
  #mov (0), d2
  
  #movm (sp), [other]
  #movm [other], (sp)
  ### Nothing below a2 allowed
  #movm [a2, a3, d2, d3, other], (sp)
  #movm [a2, a3, d2, d3], (sp)
  #movm [a2, a3, d2], (sp)
  #movm [a2, a3], (sp)
  #movm [a2], (sp)

  add 0x80000000, d2
  mov d2, a0
  mov d0, (a0)
  mov 0x3333, d0

