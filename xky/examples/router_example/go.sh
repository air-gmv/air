#!/bin/bash
cd ../.. && make && cd examples/router_example
make clean
make
FILENAME="SPWRTR.exe"
echo Copying files to remote computer...
sshpass -p "latitudee5440" scp executable/XKYAPP.exe ngmp@83.240.244.231:~/xky/examples/iop_example/executable/$FILENAME
#sshpass -p "latitudee5440" scp p0/p0.exe ngmp@83.240.244.231:~/xky/examples/hello_world/p0VM/p0_408.exe
#sshpass -p "latitudee5440" scp p0/p0.num ngmp@83.240.244.231:~/xky/examples/hello_world/p0VM/p0_408.num
echo Files copied $FILENAME
#cd p0
#sparc-rtems-objdump -d p0.exe > assembly.txt 
#cd ..
#cd executable
#sparc-rtems-objdump -d XKYAPP.exe > HELLO408.txt 
#sparc-rtems-nm XKYAPP.exe > HELLO408.num 
