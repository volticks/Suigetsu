#!/bin/zsh 

if (($# < 1)); then
  echo "Need testcase id."
  return
fi

tcase=($1)

echo $tcase
fname=(tcase_real_$tcase)
echo $fname

csmith --concise --ptr-size 4 --no-global-variables --checksum  --no-safe-math --output ${fname}.c
sed -i "s/csmith.h/my_stdint.h/g;/platform_/d;/printf/d;/strcmp/d;s/return 0/return returned/g;" ${fname}.c
../../../../Suigetsu/compiler/cross/bin/mn10300-elf-gcc -Wno-incompatible-pointer-types  -nostdlib -static ${fname}.c -o ${fname}
