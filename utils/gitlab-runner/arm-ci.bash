#!/bin/bash

pkill -f "/opt/qemu/bin/qemu-system-arm -M arm-generic-fdt-7series -serial mon:stdio -display none -dtb /home/air/workspace/dtb/system.dtb -s -kernel ./executable/AIRAPP.exe -S"
pkill qemu-system-arm

/opt/qemu/bin/qemu-system-arm -M arm-generic-fdt-7series -serial mon:stdio -display none -dtb /home/air/workspace/dtb/system.dtb -s -kernel ./executable/AIRAPP.exe -S &
PID=$!
arm-rtems5-gdb -x $UTILS/gdb_arm-ci ./executable/AIRAPP.exe --cd=.

kill -9 $PID