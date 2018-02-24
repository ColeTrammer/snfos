#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
mkdir -p isodir/modules

cp sysroot/boot/myos.kernel isodir/boot/myos.kernel
cp kernel/arch/i386/stage2_eltorito isodir/boot/grub
cat > isodir/boot/grub/menu.lst << EOF
default=0
timeout=0

title os
kernel /boot/myos.kernel

module /modules/program
EOF

cd ./kernel/modules
i686-elf-as -o start.o start.S
i686-elf-gcc -o program.o program.c -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs
i686-elf-ld -T link.ld -o ../../isodir/modules/program start.o program.o
cd ..
cd ..
# i686-elf-ld -T kernel/modules/link.ld -o isodir/modules/program kernel/modules/program.o
# nasm -f bin kernel/modules/program.S -o isodir/modules/program

genisoimage -R                              \
			-b boot/grub/stage2_eltorito    \
			-no-emul-boot                   \
			-boot-load-size 4               \
			-A os                           \
			-input-charset utf8             \
			-quiet                          \
			-boot-info-table                \
			-o myos.iso                     \
			isodir
