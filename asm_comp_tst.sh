#!/usr/bin/zsh
~/Documents/uni/Suigetsu/compiler/cross/bin/mn10300-elf-as ./src/tests/ins/tst.asm && 
~/Documents/uni/Suigetsu/compiler/cross/bin/mn10300-elf-objdump -d ./a.out 
~/Documents/uni/Suigetsu/compiler/cross/bin/mn10300-elf-objcopy -O binary --only-section=.text a.out ./src/tests/ins/tst.text

