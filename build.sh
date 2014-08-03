#!/bin/sh

make clean

make

nasm -f elf64 -o /tmp/germ.o -I asm/ asm/germ.asm

python ../disas/disas.py -s /tmp/germ.o
