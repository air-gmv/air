#!/bin/bash
configure
make clean && make
./killqemu.bash &
/opt/qemu/bin/qemu-system-arm -monitor telnet::45454,server,nowait -M arm-generic-fdt-7series -serial mon:stdio -display none -dtb /home/air/workspace/dtb/system.dtb -kernel executable/AIRAPP.exe |& tee testresult.txt 



