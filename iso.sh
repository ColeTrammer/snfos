#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
mkdir -p isodir/modules

cp sysroot/boot/snfos.kernel isodir/boot/snfos.kernel
cp kernel/arch/i386/stage2_eltorito isodir/boot/grub
cat > isodir/boot/grub/menu.lst << EOF
default=0
timeout=0

title os
kernel /boot/snfos.kernel

module /modules/program1.bin
EOF

cd ./kernel/modules
i686-elf-as -o ../../sysroot/usr/lib/crt0.o ../../libc/arch/i386/crt0.S
i686-elf-as -o ../../sysroot/usr/lib/crti.o ../../libc/arch/i386/crti.S
i686-elf-as -o ../../sysroot/usr/lib/crtn.o ../../libc/arch/i386/crtn.S
i686-elf-gcc -o program1.o program1.c -O2 -T link.ld
i686-elf-objcopy -O binary program1.o ../../isodir/modules/program1.bin
cd ../..

genisoimage -R                              \
			-b boot/grub/stage2_eltorito    \
			-no-emul-boot                   \
			-boot-load-size 4               \
			-A os                           \
			-input-charset utf8             \
			-quiet                          \
			-boot-info-table                \
			-o snfos.iso                     \
			isodir
