#!/bin/bash

#pkill qemu-system-arm
#/opt/qemu/bin/qemu-system-arm -M arm-generic-fdt-7series -serial mon:stdio -display none -dtb /home/air/workspace/dtb/system.dtb -s -kernel ./executable/AIRAPP.exe -S &
/opt/grmon-pro-2.0.92/bin/grmon -u -nb -eth 192.168.0.30 -gdb -c runAIRAPP.bash -echo &

sparc-rtems5-gdb -x $AIR/../utils/gitlab-runner/gdb_arm_rvs_coverage-ci ./executable/AIRAPP.exe --cd=.
#pkill qemu-system-arm
