#!/bin/bash
cd ../.. && make && cd examples/router_example
make clean
make
FILENAME="SPWRTR.exe"
echo Copying files to remote computer...
sshpass -p "$(AIR_RUNNER_PASSWORD)" scp executable/XKYAPP.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/xky/examples/router_example/executable/$FILENAME
sshpass -p "$(AIR_RUNNER_PASSWORD)" scp iop/p0.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/xky/examples/router_example/iop/p0.exe
sshpass -p "$(AIR_RUNNER_PASSWORD)" scp sampling/p1.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/xky/examples/router_example/sampling/p1.exe
sshpass -p "$(AIR_RUNNER_PASSWORD)" scp queuing/p2.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/xky/examples/router_example/queuing/p2.exe
#sshpass -p "$(AIR_RUNNER_PASSWORD)" scp p0/p0.num $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/xky/examples/hello_world/p0VM/p0_408.num
echo Files copied $FILENAME
#cd p0
#sparc-rtems-objdump -d p0.exe > assembly.txt 
#cd ..
#cd executable
sparc-rtems-objdump -d iop/p0.exe > p0-dissasembly.txt 
#sparc-rtems-nm XKYAPP.exe > HELLO408.num 
