#!/usr/bin/zsh

## Run all programs in this dir

progs=(./00???)

for p ($progs)
do
  ../../../../build/Suigetsu $p 0 70 4096 | grep -a BREAK -A20 | grep D0
done

