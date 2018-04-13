#!/bin/sh
set -e

objdump -D kernel/modules/program1.o > program1.dis
objdump -D kernel/modules/program2.o > program2.dis
objdump -d isodir/boot/snfos.kernel > kernel.dis