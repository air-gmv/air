#!/bin/bash

# Iterate examples directories and perform coverage if the build is successful (AIRAPP.exe exists)
for d in $AIR_ROOT/examples/* $AIR_ROOT/examples/private-example/private/validation/*; do
	echo $d
	if [[ -f "$d/executable/AIRAPP.exe" ]] # If the example was built
	then
		pkill qemu-system-arm
		# Start QEMU in xterm terminal
		xterm -e "qemu-system-aarch64 -M arm-generic-fdt-7series -serial mon:stdio -display none -dtb $HOME/dtb/system.dtb -s -kernel $d/executable/AIRAPP.exe -S" &
		
		# Start gdb server and execute commands in the file gdb_arm_rvs_coverage
		xterm -e "arm-rtems5-gdb -x $AIR_ROOT/rvs_air/scripts/gdb_arm_rvs_coverage $d/executable/AIRAPP.exe --cd=$d"
		pkill qemu-system-arm
	fi
done
