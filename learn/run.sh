
#!/bin/sh

nasm -f elf64 -o a.o $1
ld -o a a.o
rm a.o
./a

