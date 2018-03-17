#!/bin/sh
set -e

objdump -D kernel/modules/program.o > program.dis