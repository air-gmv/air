#!/bin/bash
make clean
make
echo Copying files to remote computer...
sshpass -p "latitudee5440" scp executable/XKYAPP.exe ngmp@83.240.244.231:~/xky/examples/iop_example/executable/IOP_gmvs.exe
echo Files copied
