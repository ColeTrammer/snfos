#!/bin/sh
set -e

objdump -D modules/program1.o > program1.dis
objdump -d isodir/boot/snfos.kernel > kernel.dis