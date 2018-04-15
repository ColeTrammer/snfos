#!/bin/sh
set -e
. ./config.sh

for PROJECT in $PROJECTS; do
  (cd $PROJECT && $MAKE clean)
done

# rm -rf sysroot 
rm -rf isodir
rm -rf snfos.iso
rm -rf dump debug kernel.dis logs.txt os.iso program.dis program1.dis program2.dis
