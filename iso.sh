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

i686-elf-as --32 -o kernel/modules/program.o kernel/modules/program.S
i686-elf-ld -Ttext 0 --oformat binary -o isodir/modules/program kernel/modules/program.o
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
