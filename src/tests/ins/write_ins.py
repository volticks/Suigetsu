f = open("instructions.dat", "wb")
clr = b"\x0c"

mov_d3_op = 0xf
movhu_d0_abs16 = b"\x0f" + b"AB"
movbu_d0_abs16 = b"\x0e" + b"CC"
mov_d0_abs16 = b"\x0d" + b"FF"

add_imm8_a0 = b"\x20\x41"
add_imm8_d3 = b"\x2e\x41"

mov_imm16_a0 = b"\x21\xff\xff"
mov_imm16_d3 = b"\x2f\xff\xff"

insbuf = b""

## A very very primitive attempt to iterate over the instruction set lmao
## First, Sn
for i in range(0, 0xf0):
        for j in range(0, 0xff):
                insbuf += bytes([i, j])

## Now test some Dn
#for i in range(0, 0xff):
#        insbuf += bytes([0xf0, i])

f.write(clr + insbuf)

#f.write(clr + 
#        mov_d0_abs16 + 
#        movbu_d0_abs16 + 
#        movhu_d0_abs16 + 
#        clr + 
#        mov_imm16_a0 + 
#        mov_imm16_d3 +  
#        add_imm8_a0 + 
#        add_imm8_d3 )
f.close()
