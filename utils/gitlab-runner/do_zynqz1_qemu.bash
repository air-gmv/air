#!/bin/bash
configure
make clean && make
./killqemu.bash &
/opt/qemu/bin/qemu-system-arm -monitor telnet::45454,server,nowait -M arm-generic-fdt-7series -serial mon:stdio -display none -dtb /home/air/workspace/dtb/system.dtb -kernel executable/AIRAPP.exe |& tee testresult.txt 


cmd="./testcheck_arm.py"
$cmd
status=$?
if [ $status -eq 0 ]; then
    echo TEST RTEMS5 OK
else
    echo TEST RTEMS5 FAILED EXITING
    exit 1
fi
echo RTEMS5 test ended
