#!/usr/bin/zsh
~/Documents/uni/Suigetsu/compiler/cross/bin/mn10300-elf-as ./src/tests/ins/ins_sim_tst.asm -o ./src/tests/ins/ins_sim_tst.out && 
~/Documents/uni/Suigetsu/compiler/cross/bin/mn10300-elf-objdump -d ./src/tests/ins/ins_sim_tst.out 
~/Documents/uni/Suigetsu/compiler/cross/bin/mn10300-elf-objcopy -O binary --only-section=.text ./src/tests/ins/ins_sim_tst.out  ./src/tests/ins/ins_sim.text

