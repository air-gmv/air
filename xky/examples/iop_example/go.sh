#!/bin/bash
make clean
make
echo Copying files to remote computer...
sshpass -p "$(AIR_RUNNER_PASSWORD)" scp executable/XKYAPP.exe $(AIR_RUNNER_USER)@$(AIR_RUNNER_IP):~/xky/examples/iop_example/executable/IOP_gmvs.exe
echo Files copied
