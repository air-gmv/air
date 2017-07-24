#!/bin/bash
make clean
make
echo Copying files to remote computer...
sshpass -p "latitudee5440" scp executable/XKYAPP.exe ngmp@$(AIR_RUNNER_IP):~/xky/examples/iop_example/executable/IOP_gmvs.exe
echo Files copied
