f = open("instructions.dat", "wb")
clr = b"\x0c"

mov_d3_op = 0xf
movhu_d0_abs16 = b"\x0f" + b"AB"
movbu_d0_abs16 = b"\x0e" + b"CC"
mov_d0_abs16 = b"\x0d" + b"FF"
f.write(clr + mov_d0_abs16 + movbu_d0_abs16 + movhu_d0_abs16)
f.close()
