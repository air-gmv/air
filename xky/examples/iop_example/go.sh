#!/bin/bash
#make clean
#make
echo Copying files to remote computer...
sshpass -p "latitudee5440" scp executable/XKYAPP.exe ngmp@$(AIR_RUNNER_IP):~/xky/examples/iop_example/executable/CPSW.exe
#sshpass -p "latitudee5440" scp p0/p0.exe ngmp@$(AIR_RUNNER_IP):~/xky/examples/hello_world/p0VM/p0_408.exe
#sshpass -p "latitudee5440" scp p0/p0.num ngmp@$(AIR_RUNNER_IP):~/xky/examples/hello_world/p0VM/p0_408.num
echo Files copied
#cd p0
#sparc-rtems-objdump -d p0.exe > assembly.txt 
#cd ..
#cd executable
#sparc-rtems-objdump -d XKYAPP.exe > HELLO408.txt 
#sparc-rtems-nm XKYAPP.exe > HELLO408.num 
