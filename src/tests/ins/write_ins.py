f = open("instructions.dat", "wb")
clr = b"\x0c"

mov_d3_op = 0xf
movhu_d0_abs16 = b"\x0f" + b"AB"
movbu_d0_abs16 = b"\x0e" + b"CC"
mov_d0_abs16 = b"\x0d" + b"FF"

add_imm8_a0 = b"\x20\xff"
add_imm8_d3 = b"\x2e\xff"

mov_imm16_a0 = b"\x21\xff\xff"
mov_imm16_d3 = b"\x2f\xff\xff"


f.write(clr + 
        mov_d0_abs16 + 
        movbu_d0_abs16 + 
        movhu_d0_abs16 + 
        clr + 
        mov_imm16_a0 + 
        mov_imm16_d3 +  
        add_imm8_a0 + 
        add_imm8_d3 )
f.close()
