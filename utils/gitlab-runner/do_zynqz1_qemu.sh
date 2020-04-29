#!/bin/bash
killall qemu-system-arm
killall tee
./killqemu.sh &
/home/taste/work/qemu/arm-softmmu/qemu-system-arm -monitor telnet::45454,server,nowait -M arm-generic-fdt-7series -serial mon:stdio -display none -dtb /home/taste/work/qemu/system.dtb -kernel executable/AIRAPP.exe |& tee testresult.txt



