#!/usr/bin/zsh

## Run all programs in this dir

numtests=10
progs_sim=(./*_sim)
progs_ntv=(./*_ntv)

for i in {1..$numtests} 
do
  echo ($i)_sim
  echo ($i)_ntv
  echo -n "Simulated: "
  ../../../../../Suigetsu/build/Suigetsu ($i)_sim 0 70 4096 | grep -a BREAK -A20 | grep D0
  echo -n "Real:  "
  gdb ./($i)_ntv  -x ./gdbscript.gdb -batch -n | grep ret
done
